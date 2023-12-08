// Application internal
#include "general.h"
#include "network.h"
#include "totem.h"
#include "mls.h"

using namespace std;
using namespace mls;

#define USE_UPDATE_PROPOSAL 0
#define HEARTBEAT_STRING "heartbeat"
#define CONFIGURATION_FILE "flamingo_mls.conf"

// Global variables
#pragma region
int handshake_socket, mls_handling_error_count = 0, remove_member_fail_count = 0;
bool handshake_active = false, goto_join_session = false;
vector<MessageObject> received_buffer, received_key_packages;
vector<string> send_broadcast_buffer;
vector<uint32_t> remove_members;
vector<tuple<unsigned int,uint32_t,uint32_t>> mls_group;
map<unsigned int, chrono::milliseconds> time_of_last_update;
map<unsigned int, chrono::milliseconds> time_of_last_message;
mutex session_token, received_buffer_token, received_message_token;
chrono::milliseconds first_mls_handling_error, last_update = 0ms, time_of_last_error = get_current_time();
Totem totem;
Session session;

bool create_own_group_flag = true; // Used to have the node create its own group
#pragma endregion

///// Message handling functions
#pragma region
void send_application_message(string message, string address, uint16_t port) {
	ApplicationMessage m;
	m.set_contents(message);
	send_packet(handshake_socket, m.serialize(), address, port);
}

void add_to_received_buffer(MessageObject m) {
	lock_guard<mutex> lock(received_buffer_token);
	received_buffer.push_back(m);
}

MessageObject get_from_received_buffer() {
	while (received_buffer.size() == 0 && !goto_join_session) {this_thread::sleep_for(1ms);}
	if (goto_join_session) return MessageObject();
	lock_guard<mutex> lock(received_buffer_token);
	MessageObject m = received_buffer.front();
	received_buffer.erase(received_buffer.begin());
	return m;
}

void add_to_send_broadcast_buffer(string m) {
	send_broadcast_buffer.push_back(m);
}
#pragma endregion

///// Session handling functions
#pragma region
void reset_time_of_last_update() {
	map<unsigned int, chrono::milliseconds>::iterator it = time_of_last_update.begin();
	while (it != time_of_last_update.end()) {
		it->second = get_current_time();
		it++;
	}
}

void reset_time_of_last_application_message_sent() {
	map<unsigned int, chrono::milliseconds>::iterator it = time_of_last_message.begin();
	while (it != time_of_last_message.end()) {
		it->second = get_current_time();
		it++;
	}
}

unsigned int find_member_to_join() {
	vector<unsigned int> totem_group = totem.get_my_memb();
	vector<tuple<unsigned int,uint32_t,uint32_t>> mls_group = get_mls_group(&session);
	vector<unsigned int> mls_group_id, difference;

	if (mls_group.empty()) return global_id;

	if (mls_group.size() > totem_group.size() / 2) return global_id;

	for (int i = 0; i < mls_group.size(); i++) mls_group_id.push_back(get<0>(mls_group[i]));

	if (mls_group_id.size() <= totem_group.size()) {
		difference = get_difference_set(totem_group,mls_group_id);
		return difference[0];
	}

	return global_id;
}

void join_session() {
	MessageObject received_message;
	string message;
	
	if (create_own_group_flag && CREATE_OWN_GROUP) {
		create_own_group_flag = false;
		log_this("Deliberately creating own group",true,1);
		return;
	}

	chrono::milliseconds join_session_initialized = get_current_time();

	while(true) {
		unsigned int id = find_member_to_join();
		if (id == global_id) break;

		// Initializes new client with new key package
		Client client = initialize_client(global_id);
		PendingJoin join = client.start_join();
		message = create_mls_message(KEYPACKAGE,join.key_package());

		// Sends key package to a member of the MLS group
		MessageObject mo = id_to_address[id];
		send_application_message(message,mo.address,mo.port);
		auto key_package = tls::get<KeyPackage>(join.key_package());
		log_this("Key package sent to " + mo.address + ":" + to_string(mo.port) + " with signature " 
			+ to_hex(key_package.signature).substr(10,5),false,1);

		chrono::milliseconds package_sent = get_current_time();

		// Receiving welcome message
		do {
			while (received_buffer.size() == 0 && get_time_since(package_sent).count() <= WAIT_FOR_WELCOME_TIMEOUT) {
				this_thread::sleep_for(1ms);
			}

			if (get_time_since(package_sent).count() > WAIT_FOR_WELCOME_TIMEOUT) break;

			received_message = get_from_received_buffer();

			if (check_mls_type(received_message.type,WELCOME)) {
				auto welcome = tls::get<Welcome>(from_hex(received_message.content));
				log_this("Received welcome message from " + received_message.address + ":" 
					+ to_string(received_message.port) + " with mark "
					+ to_hex(welcome.encrypted_group_info).substr(0,5),false,1);
				
				if (handle_welcome_message(&join,&session,received_message.content)) {
					reset_time_of_last_update();
					reset_time_of_last_application_message_sent();
					remove_member_fail_count = 0;
					goto end;
				}
				else continue;
			}
		} while (true);
	
		if (get_time_since(join_session_initialized).count() > JOIN_SESSION_TIMEOUT) {
			break;
		}
	}
	end:

	mls_group = get_mls_group(&session);

	log_this("Joined session with group_id=" + to_hex(session.group_info().group_context.group_id) + ",epoch=" + to_string(session.epoch()) 
		+ ",confirmation_tag=" + to_hex(session.group_info().confirmation_tag).substr(0,6) + ",mls_group{id,index} = " 
		+ convert_to_string(mls_group), true, 1);
}

void mls_handling_error() {
	if (get_time_since(time_of_last_error).count() > MLS_HANDLING_ERROR_RESET_TIMER) {
		mls_handling_error_count = 0;
	}
	time_of_last_error = get_current_time();
	if (mls_handling_error_count == 0) first_mls_handling_error = get_current_time();

	mls_handling_error_count++;

	if (mls_handling_error_count > MLS_HANDLING_ERROR_COUNTER_THRESHOLD && 
			get_time_since(first_mls_handling_error).count() > TOKEN_ROUND_INTERVAL) {
		log_this("MLS handling error count exceeded, joining new session",false,1);
		handshake_active = false;
		join_session();
		handshake_active = true;
		mls_handling_error_count = 0;
	}
}

void remove_members_not_updating() {
	bool check = false;
	if (mls_group.empty()) return;
	for (tuple<unsigned int,uint32_t,uint32_t> i : mls_group) {
		if (time_of_last_update.find(get<0>(i)) == time_of_last_update.end()) {
			time_of_last_update[get<0>(i)] = get_current_time();
			continue;
		}
		if (get_time_since(time_of_last_update[get<0>(i)]).count() > NO_UPDATE_THRESHOLD && get<0>(i) != global_id) {
			remove_members.push_back(get<1>(i));
			log_this("Adding {" + to_string(get<0>(i)) + "," + to_string(get<1>(i)) + "," + to_string(get<2>(i)) 
				+ "} to remove_members, because the member has not updated in " 
				+ to_string(get_time_since(time_of_last_update[get<0>(i)]).count()) + "ms",false,1);
		}
	}
}

void remove_mls_members_not_in_totem_group() {
	vector<unsigned int> totem_group = totem.get_my_memb();

	if (mls_group.empty()) return;

	for (int i = 0; i < mls_group.size(); i++) {
		if (find(totem_group.begin(),totem_group.end(),get<0>(mls_group[i])) == totem_group.end()) {
			remove_members.push_back(get<1>(mls_group[i]));
			log_this("Adding {" + to_string(get<0>(mls_group[i])) + "," + to_string(get<1>(mls_group[i])) + "," 
				+ to_string(get<2>(mls_group[i])) + "} to remove_members, because the member is not in totem group",false,1);
		}
	}
}

void remove_members_not_sending_application_messages() {
	bool check = false;
	if (mls_group.empty()) return;

	for (tuple<unsigned int,uint32_t,uint32_t> i : mls_group) {
		if (time_of_last_message.find(get<0>(i)) == time_of_last_message.end()) {
			time_of_last_message[get<0>(i)] = get_current_time();
			continue;
		}
		if (get_time_since(time_of_last_message[get<0>(i)]).count() > NO_MESSAGE_SENT_THRESHOLD && get<0>(i) != global_id) {
			remove_members.push_back(get<1>(i));
			log_this("Adding {" + to_string(get<0>(i)) + "," + to_string(get<1>(i)) + "," + to_string(get<2>(i)) 
				+ "} to remove_members, because the member has not sent a message in " 
				+ to_string(get_time_since(time_of_last_message[get<0>(i)]).count()) + "ms",false,1);
		}
	}
}

void remove_duplicates() {
	// Removing old instances of the node
	bool first_instance = false;
	if (!mls_group.empty()) {
		for (int i = 0; i < mls_group.size(); i++) {
			if (get<0>(mls_group[i]) == global_id && get<2>(mls_group[i]) != session.index().val) {
				remove_members.push_back(get<1>(mls_group[i]));
				log_this("Adding {" + to_string(get<0>(mls_group[i])) + "," + to_string(get<1>(mls_group[i])) + "," 
					+ to_string(get<2>(mls_group[i])) + "} to remove_members, because it's a duplicate of this node",false,1);
			}
		}
	}
}

bool check_for_stable_group() {
	if (mls_group.empty()) return false;

	vector<unsigned int> totem_group = totem.get_my_memb();

	if (get_time_since(time_of_last_error).count() < MLS_HANDLING_ERROR_RESET_TIMER) {
		log_this("Group not stable. Time since last MLS handling error is " + to_string(get_time_since(time_of_last_error).count()) 
			+ " ms", false, 1);
		return false;
	}

	if (totem_group.size() != mls_group.size()) {
		log_this("Group not stable. Totem group and MLS group are not the same size.", false, 1);
		return false;
	}
	
	for (tuple<unsigned int,uint32_t,uint32_t> i : mls_group) {
		if (get<0>(i) != global_id && get_time_since(time_of_last_message[get<0>(i)]).count() > RECENT_MESSAGE_THRESHOLD_FOR_UPDATE) {
			log_this("Group not stable. Node " + to_string(get<0>(i)) + " has not sent message in "
				+ to_string(get_time_since(time_of_last_message[get<0>(i)]).count()) + " ms", false, 1);
			return false;
		}
		if (find(totem_group.begin(),totem_group.end(),get<0>(i)) == totem_group.end()) {
			log_this("Group not stable. Node " + to_string(get<0>(i)) + " is not in Totem group", false, 1);
			return false;
		}
	}

	return true;
}
#pragma endregion

///// Defining Totem methods
#pragma region
void Totem::broadcast_message(Message* message) {
	log_this_totem("Totem broadcast message " + message->serialize().substr(0,50) + "(substring)",false,3,"");
	log_this_name(message->serialize(),3,"log_totem_message_sent");
	broadcast_handshake(handshake_socket,message->serialize());
}

void Totem::transmit_token(Token* token) {
	MessageObject m = id_to_address[get_next_memb()];
	if (state != OPERATIONAL_STATE) log_this_totem("Token sent to " + to_string(get_next_memb()) + " with content: " + token->serialize().substr(0,50) + "(substring)",false,2,"",false);
    log_this_name(token->serialize() + ",dst=" + m.address + ":" + to_string(m.port),3,"log_totem_message_sent");
	send_packet(handshake_socket,token->serialize(),m.address,m.port);
}

// Transmits regular token after a delay.
void Totem::transmit_regular_token(RegularToken regular_token, unsigned int send_to_id, int group_size) {
	this_thread::sleep_for(TOKEN_ROUND_INTERVAL / group_size * 1ms);
	MessageObject m = id_to_address[send_to_id];
	log_this_totem("Token sent to " + to_string(send_to_id),false,3," with content: " + regular_token.serialize(),true);
	log_this_name(regular_token.serialize() + ",dst=" + m.address + ":" + to_string(m.port),3,"log_totem_message_sent");
    send_packet(handshake_socket,regular_token.serialize(),m.address,m.port);
}

void Totem::deliver_message(RegularMessage message) {
    my_aru = message.get_seq();
	string address;
	uint16_t port;
    
    if (DELIVER_TO_SELF || message.get_sender_id() != my_id) { 
		address = id_to_address[message.get_sender_id()].address;
		port = id_to_address[message.get_sender_id()].port;
		log_this("Deliver regular message: " + message.get_contents().substr(0,50) + "(substring)",false,3);
        add_to_received_buffer(MessageObject(address,port,message.get_contents()));
    }
}

void Totem::deliver_message(ApplicationMessage message, string address, uint16_t port) {
	log_this("Deliver application message: " + message.get_contents().substr(0,50) + "(substring)",false,3);
	add_to_received_buffer(MessageObject(address,port,message.get_contents()));
}

void Totem::deliver_recovery_message(RegularMessage message) {
    old_aru = message.get_seq();
    
    if (DELIVER_TO_SELF || message.get_sender_id() != my_id) { 
        add_to_received_buffer(MessageObject(message.get_contents()));
    }
}

void Totem::mls() {
	string message;
	bytes proposal,commit,welcome;
	tuple<bytes, bytes> info;
	chrono::milliseconds ms;
	int count;

	log_this("Totem::mls() begin",false,3);

	if (BROADCAST_FOREIGN_MESSAGE) broadcast_handshake(handshake_socket,"broadcast");
	
	if (!handshake_active) return;

	lock_guard<mutex> lock(session_token);
	if (REMOVE_MEMBERS_NOT_UPDATING) remove_members_not_updating();
	if (REMOVE_MLS_MEMBERS_NOT_IN_TOTEM_GROUP) remove_mls_members_not_in_totem_group();
	if (REMOVE_MEMBERS_NOT_SENDING_MESSAGES) remove_members_not_sending_application_messages();
	if (REMOVE_DUPLICATES) remove_duplicates();

	// If one or more members should be removed, create remove proposal and commit for the members
	if (remove_members.size() && state == OPERATIONAL_STATE && remove_member_fail_count < REMOVE_MEMBER_FAIL_THRESHOLD) {
		sort(remove_members.begin(), remove_members.end());
		auto last = std::unique(remove_members.begin(), remove_members.end());
		remove_members.erase(last, remove_members.end());

		log_this("Trying to remove the following members: " + convert_to_string(remove_members),false,1);

		if (mls_group.size() - remove_members.size() < 2) {
			auto index = find(remove_members.begin(), remove_members.end(), 0);
			if (index != remove_members.end()) {
				log_this("Avoid removing member at index 0", false, 1);
				remove_members.erase(index);
			}

			goto_join_session = true;
			remove_members.clear();
			return;
		}

		// Go through members and create remove proposals
		count = 0;
		for (uint32_t i: remove_members) {
			proposal = create_remove_proposal(&session, i);
			if (!handle_handshake_data(&session, proposal) || proposal.empty()) {
				count++;
				continue;
			}
			message = create_mls_message(REMOVE,proposal);
			add_to_send_broadcast_buffer(message);
			log_this("Remove proposal sent for " + to_string(i),false,1);
		}

		if (count < remove_members.size()) {
			// Commit remove proposals and send commit.
			commit = create_commit(&session);

			if (!commit.empty()) {
				message = create_mls_message(COMMIT,commit);
				add_to_send_broadcast_buffer(message);
				remove_members.clear();

				log_this("Commit of Remove proposal sent to broadcast buffer, mls_group{id,index} = " + convert_to_string(mls_group),false,1);

				return;
			}
			else remove_member_fail_count++;
		
		}
		else remove_member_fail_count++;
		remove_members.clear();
	}
	else if (remove_member_fail_count >= REMOVE_MEMBER_FAIL_THRESHOLD) {
		log_this("remove_member_fail_count=" + to_string(remove_member_fail_count),false,1);
		goto_join_session = true;
		remove_members.clear();
	}

	// If one or more key packages are received, create add proposal and commit for the new member
	if (received_key_packages.size() && state == OPERATIONAL_STATE) {
		log_this("Trying to add " + to_string(received_key_packages.size()) + " members", false, 1);

		// Go through key packages and create add proposals
		count = 0;
		for (MessageObject mo: received_key_packages) {
			proposal = create_add_proposal(&session,mo.content);

			if (!handle_handshake_data(&session, proposal) || proposal.empty()) {
				count++;
				continue;
			}

			message = create_mls_message(ADD,proposal);
			add_to_send_broadcast_buffer(message);
			log_this("Add proposal sent",false,1);
		}
		
		// Commit add proposals and send welcome message and commit.
		if (count < received_key_packages.size()) {
			info = create_commit_and_welcome(&session);
			welcome = get<0>(info);
			commit = get<1>(info);
			if (welcome.empty()) {
				received_key_packages.clear();
				return;
			}
			message = create_mls_message(WELCOME,welcome);
			for (MessageObject mo: received_key_packages) {
				send_application_message(message, mo.address, mo.port);
				auto welcome_object = tls::get<Welcome>(welcome);
				log_this("Welcome message sent to " + mo.address + ":" + to_string(mo.port) + " with mark " 
				+ to_hex(welcome_object.encrypted_group_info).substr(0,5),false,1);
			}

			message = create_mls_message(COMMIT,commit);
			add_to_send_broadcast_buffer(message);
			reset_time_of_last_application_message_sent();
			remove_member_fail_count = 0;
			log_this("Commit of Add proposal sent to broadcast buffer, mls_group{id,index} = " + convert_to_string(mls_group),false,1);
		} 
		received_key_packages.clear();
		return;
	}

	// If time since we sent last update is less than UPDATE_INTERVAL, the function returns.
	if (get_time_since(last_update).count() < UPDATE_INTERVAL || state != OPERATIONAL_STATE) return;
	last_update = get_current_time();

	if (!check_for_stable_group()) return;

	// Create update proposal.
	#if USE_UPDATE_PROPOSAL
	proposal = create_update_proposal(&session);
	if (proposal.empty()) return;

	message = create_mls_message(UPDATE,proposal);
	add_to_send_broadcast_buffer(message);
	log_this("Update proposal added to broadcast buffer",false,1);

	// Handle update proposal. If error, return.
	if (!handle_handshake_data(&session, proposal)) return;

	#endif

	// Create and send commit.
	commit = create_commit(&session);
	if (commit.empty()) return;

	message = create_mls_message(COMMIT,commit);
	add_to_send_broadcast_buffer(message);
	log_this("Commit of update proposal sent",false,1);
}

size_t Totem::get_new_message_queue_size() {
	return send_broadcast_buffer.size();
}

string Totem::get_new_message_queue_front() {
	return send_broadcast_buffer.front();
}

void Totem::new_message_queue_pop() {
	send_broadcast_buffer.erase(send_broadcast_buffer.begin());
}
#pragma endregion

///// Thread functions
#pragma region
void thread_totem_func() {
	char buffer[MAX_BUFFER_LENGTH],address[100];
    uint16_t port;
	string received, address_string;
	totem.start();

	while(true) {
		if (totem.token_loss_timer && get_time_since(totem.token_loss_timer_start_time).count() >= TOKEN_LOSS_TIMEOUT_VALUE)
			totem.token_loss_timeout();

		if (totem.token_retransmission_timer && 
				get_time_since(totem.token_retransmission_timer_start_time).count() >= RETRANSMISSION_TIMEOUT_VALUE) 
			totem.token_retransmission_timeout();

		if (totem.join_timer && get_time_since(totem.join_timer_start_time).count() >= JOIN_TIMEOUT_VALUE) totem.join_timeout();

		if (totem.consensus_timer && get_time_since(totem.consensus_timer_start_time).count() >= CONSENSUS_TIMEOUT_VALUE) 
			totem.consensus_timeout();

		if (receive_packet(handshake_socket, buffer, address, &port, RECEIVE_PACKET_HANDSHAKE_TIMEOUT) < 0) continue;
		
		received = buffer;
		address_string = address;
		log_this("Message received from " + address_string + ":" + to_string(port) + ":" + received.substr(0,50) + "(substring)",false,3);
		
		int id = totem.receive(received, address, port);
		if (id >= 0) {	
			if (id_to_address.find(id) == id_to_address.end()) {
				log_this("Added to id_to_address id " + to_string(id) + ": " + address + ":" + to_string(port), false, 1);
				id_to_address[id] = MessageObject(address,port,"");
			}
		}
	}
}
void thread_local_receive_func() {
	int socket = initialize_socket();
	char buffer[MAX_BUFFER_LENGTH], address[100];
	uint16_t recv_port, port_bind;

	if (LOCAL_MACHINE) port_bind = PORT_LOCAL_RECEIVE + global_id;
	else port_bind = PORT_LOCAL_RECEIVE;

	bind_socket(socket, port_bind);
	
	log_this("Local receive at port " + to_string(port_bind) + ", sending to " + MULTICAST_ADDRESS_EXTERNAL + ":" + to_string(PORT_APPLICATION),
		true,1);

	while(1) {
		string received;
		int length = receive_packet(socket, buffer, address, &recv_port, APPLICATION_MESSAGE_HEARTBEAT_INTERVAL);
		if (length < 0) {
			received = HEARTBEAT_STRING;
			log_this("Sending heartbeat",false,2);
		}
		else received = convert_buffer_to_string(buffer, length);
		session_token.lock();
		try {
			string encrypted = protect_data(received, &session);
			send_packet(socket, encrypted, MULTICAST_ADDRESS_EXTERNAL, PORT_APPLICATION);
		} catch(const exception& e) {
			string s = address;
			string error_message = e.what();
			log_this("Error: Unable to encrypt message from " + s + ":" + to_string(recv_port) + " '" + received + "':" + error_message,false,0);
		}
		session_token.unlock();
	}
}
void thread_local_transmit_func() {
	int socket = initialize_socket();
	char buffer[MAX_BUFFER_LENGTH], address[100];
	uint16_t recv_port, port_send, local_receive_func_src_port;

	if (LOCAL_MACHINE) {
		port_send = PORT_LOCAL_TRANSMIT + global_id;
		local_receive_func_src_port = PORT_LOCAL_RECEIVE + global_id;
	}
	else {
		port_send = PORT_LOCAL_TRANSMIT;
		local_receive_func_src_port = PORT_LOCAL_RECEIVE;
	}

	bind_socket(socket, PORT_APPLICATION);
	
	log_this("Receiving at port " + to_string(PORT_APPLICATION) + ", local transmit to " + BROADCAST_ADDRESS_INTERNAL + ":" + to_string(port_send),true,1);

	while(1) {
		receive_packet(socket, buffer, address, &recv_port);
		unsigned int sender_id = get_id_from_address_application_messages(address,recv_port);
		if (SEND_APPLICATION_MESSAGES_TO_SELF || sender_id != global_id) {
			string encrypted = buffer;

			session_token.lock();
			try {
				string decrypted = unprotect_data(encrypted, &session);
				if (decrypted.compare(HEARTBEAT_STRING) != 0) send_packet(socket, decrypted, BROADCAST_ADDRESS_INTERNAL, port_send);
				
				epoch_t message_epoch = tls::get<MLSMessage>(from_hex(encrypted)).epoch();
				if (message_epoch < session.epoch()) {
					string s = address;
					log_this("Message from " + s + ":" + to_string(recv_port) + " is using epoch " + to_string(message_epoch)
						+ ". Current epoch is " + to_string(session.epoch()), false, 2);
					mls_handling_error();
				}
				else {
					time_of_last_message[sender_id] = get_current_time();
				}
			} catch (const exception& e) {
				string s = address;
				string error_message = e.what();
				log_this("Unable to decrypt message from " + s + ":" + to_string(recv_port) + ":" + error_message, false, 2);
				mls_handling_error();
			}
			session_token.unlock();
		}
	}
}
#pragma endregion

int main(int argc, char** argv) {
	///// SETUP /////
	#pragma region
	read_configuration_file(CONFIGURATION_FILE);

    if (argc == 2) {
        global_id = atoi(argv[1]);
    }

	if (DELETE_LOG) {
		remove(("log/log" + to_string(global_id) + ".1.txt").data());
		remove(("log/log" + to_string(global_id) + ".2.txt").data());
		remove(("log/log_message" + to_string(global_id) + ".txt").data());
		remove(("log/log_totem" + to_string(global_id) + ".txt").data());
		remove(("log/log_totem_message_received" + to_string(global_id) + ".txt").data());
		remove(("log/log_totem_message_sent" + to_string(global_id) + ".txt").data());
		remove(("log/log_send_packet" + to_string(global_id) + ".txt").data());
		remove(("log/log_receive_packet" + to_string(global_id) + ".txt").data());
	}

	log_this("Build time = " __DATE__ " " __TIME__,true,0);

	log_configuration_file(CONFIGURATION_FILE);

	// Initializing variables
    uint16_t port;
    string address;
	bytes message;
	MessageObject received_message;
	handshake_socket = initialize_socket();
	totem.set_id(global_id);

	// Initializing socket, client and session
	if (LOCAL_MACHINE) bind_socket(handshake_socket, PORT_HANDSHAKE + global_id);
	else bind_socket(handshake_socket, PORT_HANDSHAKE);

    session = create_session();

	thread thread_totem(thread_totem_func);
	log_this("thread_totem created: " + get_thread_id_as_string(&thread_totem),false,1);
	
	while(totem.get_state() != OPERATIONAL_STATE) {this_thread::sleep_for(1ms);}
	join_session();

	handshake_active = true;

    thread thread_local_receive(thread_local_receive_func);
	thread thread_local_transmit(thread_local_transmit_func);

	log_this("thread_local_receive created: " + get_thread_id_as_string(&thread_local_receive), false, 1);
	log_this("thread_local_transmit created: " + get_thread_id_as_string(&thread_local_transmit), false, 1);

	#pragma endregion
    while(true) {
		session_token.unlock();
		
		log_this("Epoch " + to_string(session.epoch()) + ",confirmation_tag=" + to_hex(session.group_info().confirmation_tag).substr(0,6) 
			+ ",mls_group=" + convert_to_string(mls_group),true,2);
		
		received_message = get_from_received_buffer();

		if (goto_join_session) {
			goto_join_session = false;
			handshake_active = false;
			session_token.lock();
			session = create_session();
			join_session();
			handshake_active = true;
			continue;
		}
		
        message = from_hex(received_message.content);
		address = received_message.address;
		port = received_message.port;
		log_message("Epoch " + to_string(session.epoch()));
		log_message(received_message.content);
		
		session_token.lock(); 
        if (check_mls_type(received_message.type,KEYPACKAGE)) {
			auto key_package = tls::get<KeyPackage>(from_hex(received_message.content));
            log_this("Received key package from " + address + ":" + to_string(port) + " with signature " 
				+ to_hex(key_package.signature).substr(10,5),false,1);
			received_key_packages.push_back(received_message);
        }
		else if (check_mls_type(received_message.type,COMMIT)) {
            log_this("Received commit from " + address + ":" + to_string(port),false,1);
			if (handle_handshake_data(&session, message)) {
				time_of_last_update[get_id_from_address(address,port)] = get_current_time();
				mls_group = get_mls_group(&session);
				log_this("Epoch " + to_string(session.epoch()), false, 1);
			}
			else {
				mls_handling_error();
    		}
        }
		else if (sent_from_self(address,port)) {
			log_this("Received packet sent from myself",false,3);
		}
		else if (check_mls_type(received_message.type,UPDATE)) {
			log_this("Received update proposal from " + address + ":" + to_string(port),false,1);
			if (handle_handshake_data(&session, message)) {
				time_of_last_update[get_id_from_address(address,port)] = get_current_time();
			}
			else mls_handling_error();
		}
		else if (check_mls_type(received_message.type,ADD)) {
			log_this("Received add proposal from " + address + ":" + to_string(port),false,1);
            if (!handle_handshake_data(&session, message)) mls_handling_error();
			else {
				reset_time_of_last_update();
				reset_time_of_last_application_message_sent();
				remove_member_fail_count = 0;
			}
		}
		else if (check_mls_type(received_message.type,REMOVE)) {
			log_this("Received remove proposal from " + address + ":" + to_string(port),false,1);
            if (!handle_handshake_data(&session, message)) {
				mls_handling_error();
			} 
        }
		else if (check_mls_type(received_message.type,WELCOME)) {
			log_this("Received an unexpected welcome message from " + address + ":" + to_string(port),false,1);
		}
		else {
			log_this("Received packet from " + address + ":" + to_string(port) + " and could not determine type:\ntype=" 
				+ received_message.type + "\ncontent=" + received_message.content.substr(0,50) + "(substring)",false,1);
		}
	}
}