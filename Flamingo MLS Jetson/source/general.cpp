#include "general.h"

using namespace std;

// MessageObject
MessageObject::MessageObject() {}
MessageObject::MessageObject(string message) {
	if (message.size() == 0) return;
	type = message[0];
	this->content=message.substr(1);
}
MessageObject::MessageObject(string address, uint16_t port, string message): address(address), port(port) {
	if (message.size() == 0) return;
	type = message[0];
	this->content=message.substr(1);
}
MessageObject::MessageObject(char type, string address, uint16_t port, string content): type(type), address(address), port(port),
	 content(content) {}
string MessageObject::get_message() {return type + content;}

// Logs to file
// LOG_LEVEL 0: Error messages
// LOG_LEVEL 1: Program flow
// LOG_LEVEL 2: More detailed program flow
// LOG_LEVEL 3: Detailed program operation and message content
// LOG_LEVEL 4: More detailed program operation
void log_this(string message, bool print, int log_level) {
	lock_guard<mutex> lock(log_token);
	if (global_id == 0) return;
	if (log_level > LOG_LEVEL) return;

	ofstream stream;
	if (log_count <= NUMBER_OF_LOG_ELEMENTS) {
		stream.open("log/log" + to_string(global_id) + ".1.txt", ios_base::app);
		if (log_count == NUMBER_OF_LOG_ELEMENTS) remove(("log/log" + to_string(global_id) + ".2.txt").data());
	}
	else if (log_count > NUMBER_OF_LOG_ELEMENTS && log_count <= NUMBER_OF_LOG_ELEMENTS*2) {
		stream.open("log/log" + to_string(global_id) + ".2.txt", ios_base::app);
	}
	else if (log_count > NUMBER_OF_LOG_ELEMENTS*2) {
		remove(("log/log" + to_string(global_id) + ".1.txt").data());
		stream.open("log/log" + to_string(global_id) + ".1.txt", ios_base::app);
		log_count = 0;
	}
	log_count++;

	time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());

	char* char_message = message.data();

	auto myid = this_thread::get_id();
	stringstream ss;
	ss << myid;
	string mystring = ss.str();
	
	if (stream.is_open()) {
		string time_string = to_string(get_current_time().count());
		stream << std::ctime(&time) << "time=" << time_string.substr(time_string.length()-7) <<",id=" << global_id << ",thread=" << mystring.substr(mystring.length()-4)  << " " << char_message << endl;
		stream.close();
	}

	if (print == true && PRINT_TO_CONSOLE) {
		terminal_token.lock();
		puts(char_message);
		terminal_token.unlock();
	}
}
void log_message(string message, int log_level) {
	lock_guard<mutex> lock(log_token);
	if (global_id == 0) return;
	if (log_level > LOG_LEVEL) return;
	
	ofstream stream;
	stream.open("log/log_message" + to_string(global_id) + ".txt", ios_base::app);

	char* char_message = message.data();
	
	if (stream.is_open()) {
		stream << char_message << endl;
		stream.close();
	}
}
void log_this_totem(string message, bool print, int log_level,string additional,bool use_log_this) {
	if (log_level > LOG_LEVEL) return;
	if (global_id == 0) return;
	event_counter++;
	
	if (use_log_this) log_this("Totem: " + message + " " + to_string(event_counter));
	lock_guard<mutex> lock(log_token);
	
	
	ofstream stream;
	stream.open("log/log_totem" + to_string(global_id) + ".txt", ios_base::app);

	char* char_message = message.data();
	
	if (stream.is_open()) {
		stream << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() << ",id=" 
			<< global_id  << ",event=" << event_counter << " " << char_message << " " << additional << endl;
		stream.close();
	}

	if (print == true) {
		terminal_token.lock();
		puts(char_message);
		terminal_token.unlock();
	}
}
void log_this_name(string message, int log_level=3, string file_name="log") {
	if (log_level > LOG_LEVEL) return;

	ofstream stream;
	stream.open("log/" + file_name + to_string(global_id) + ".txt", ios_base::app);

	time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());

	char* char_message = message.data();
	
	if (stream.is_open()) {
		stream << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() % 10000000 <<",id=" << global_id  << " " << char_message << endl;
		stream.close();
	}
}

// Reads configuration file. Defualt file is ./program.conf
void read_configuration_file(string file) {
    ifstream is_file(file);
    string line;

    while(getline(is_file, line)) {
        istringstream is_line(line);
        string key;
        if(getline(is_line, key, '=')) {
            string value;
            if(std::getline(is_line, value)) {
                if (!key.compare("PORT_APPLICATION")) PORT_APPLICATION = stoi(value);
                else if (!key.compare("PORT_LOCAL_TRANSMIT")) PORT_LOCAL_TRANSMIT = stoi(value);
				else if (!key.compare("PORT_LOCAL_RECEIVE")) PORT_LOCAL_RECEIVE = stoi(value);
				else if (!key.compare("PORT_HANDSHAKE")) PORT_HANDSHAKE = stoi(value);
				else if (!key.compare("LOG_LEVEL")) LOG_LEVEL = stoi(value);
				else if (!key.compare("DELETE_LOG")) DELETE_LOG = stoi(value);
				else if (!key.compare("UPDATE_INTERVAL")) UPDATE_INTERVAL = stoi(value);
				else if (!key.compare("MAX_MSG_SENT")) MAX_MSG_SENT = stoi(value);
				else if (!key.compare("FAIL_TO_RECEIVE_VALUE")) FAIL_TO_RECEIVE_VALUE = stoi(value);
				else if (!key.compare("MAX_NUMBER_OF_NODES")) MAX_NUMBER_OF_NODES = stoi(value);
				else if (!key.compare("TOKEN_LOSS_TIMEOUT_VALUE")) TOKEN_LOSS_TIMEOUT_VALUE = stoi(value);
				else if (!key.compare("RETRANSMISSION_TIMEOUT_VALUE")) RETRANSMISSION_TIMEOUT_VALUE = stoi(value);
				else if (!key.compare("JOIN_TIMEOUT_VALUE")) JOIN_TIMEOUT_VALUE = stoi(value);
				else if (!key.compare("CONSENSUS_TIMEOUT_VALUE")) CONSENSUS_TIMEOUT_VALUE = stoi(value);
				else if (!key.compare("TOKEN_ROUND_INTERVAL")) TOKEN_ROUND_INTERVAL = stoi(value);
				else if (!key.compare("NUMBER_OF_LOG_ELEMENTS")) NUMBER_OF_LOG_ELEMENTS = stoi(value);
				else if (!key.compare("WAIT_FOR_WELCOME_TIMEOUT")) WAIT_FOR_WELCOME_TIMEOUT = stoi(value);
				else if (!key.compare("LOCAL_MACHINE")) LOCAL_MACHINE = stoi(value);
				else if (!key.compare("GLOBAL_ID")) global_id = stoi(value);
				else if (!key.compare("TEST_SETUP")) TEST_SETUP = stoi(value);
				else if (!key.compare("MLS_HANDLING_ERROR_RESET_TIMER")) MLS_HANDLING_ERROR_RESET_TIMER = stoi(value);
				else if (!key.compare("MLS_HANDLING_ERROR_COUNTER_THRESHOLD")) MLS_HANDLING_ERROR_COUNTER_THRESHOLD = stoi(value);
				else if (!key.compare("JOIN_SESSION_TIMEOUT")) JOIN_SESSION_TIMEOUT = stoi(value);
				else if (!key.compare("CREATE_OWN_GROUP")) CREATE_OWN_GROUP = stoi(value);
				else if (!key.compare("NO_UPDATE_THRESHOLD")) NO_UPDATE_THRESHOLD = stoi(value);
				else if (!key.compare("REMOVE_MEMBERS_NOT_UPDATING")) REMOVE_MEMBERS_NOT_UPDATING = stoi(value);
				else if (!key.compare("REMOVE_MLS_MEMBERS_NOT_IN_TOTEM_GROUP")) REMOVE_MLS_MEMBERS_NOT_IN_TOTEM_GROUP = stoi(value);
				else if (!key.compare("MAX_BUFFER_LENGTH")) MAX_BUFFER_LENGTH = stoi(value);
				else if (!key.compare("MULTICAST_ADDRESS_EXTERNAL")) MULTICAST_ADDRESS_EXTERNAL = value;
				else if (!key.compare("SOCKET_ADDRESS")) SOCKET_ADDRESS = value;
				else if (!key.compare("BROADCAST_ADDRESS_INTERNAL")) BROADCAST_ADDRESS_INTERNAL = value;
				else if (!key.compare("SEND_APPLICATION_MESSAGES_TO_SELF")) SEND_APPLICATION_MESSAGES_TO_SELF = stoi(value);
				else if (!key.compare("RECEIVE_PACKET_HANDSHAKE_TIMEOUT")) RECEIVE_PACKET_HANDSHAKE_TIMEOUT = stoi(value);
				else if (!key.compare("NO_MESSAGE_SENT_THRESHOLD")) NO_MESSAGE_SENT_THRESHOLD = stoi(value);
				else if (!key.compare("REMOVE_MEMBERS_NOT_SENDING_MESSAGES")) REMOVE_MEMBERS_NOT_SENDING_MESSAGES = stoi(value);
				else if (!key.compare("APPLICATION_MESSAGE_HEARTBEAT_INTERVAL")) APPLICATION_MESSAGE_HEARTBEAT_INTERVAL = stoi(value);
				else if (!key.compare("REMOVE_MEMBER_FAIL_THRESHOLD")) REMOVE_MEMBER_FAIL_THRESHOLD = stoi(value);
				else if (!key.compare("PRINT_TO_CONSOLE")) PRINT_TO_CONSOLE = stoi(value);
				else if (!key.compare("MAX_EPOCH_STATE_HISTORY_SIZE")) MAX_EPOCH_STATE_HISTORY_SIZE = stoi(value);
				else if (!key.compare("REMOVE_DUPLICATES")) REMOVE_DUPLICATES = stoi(value);
				else if (!key.compare("RECENT_MESSAGE_THRESHOLD_FOR_UPDATE")) RECENT_MESSAGE_THRESHOLD_FOR_UPDATE = stoi(value);
				else if (!key.compare("BROADCAST_FOREIGN_MESSAGE")) BROADCAST_FOREIGN_MESSAGE = stoi(value);
				else if (!key.compare("USE_MULTICAST")) USE_MULTICAST = stoi(value);
				else if (!key.compare("UNICAST_DRONE_START")) UNICAST_DRONE_START = stoi(value);
				else if (!key.compare("UNICAST_DRONE_END")) UNICAST_DRONE_END = stoi(value);
				else if (!key.compare("UNICAST_GCS_START")) UNICAST_GCS_START = stoi(value);
				else if (!key.compare("UNICAST_GCS_END")) UNICAST_GCS_END = stoi(value);
            } 
        }
    }
}
void log_configuration_file(string file) {
    ifstream is_file(file);
    string line;

    while(getline(is_file, line)) {
        istringstream is_line(line);
        string key;
        if(getline(is_line, key, '=')) {
            string value;
            if(std::getline(is_line, value)) {
                if (!key.compare("PORT_APPLICATION")) {log_this(key + "=" + to_string(PORT_APPLICATION),false,1);}
                else if (!key.compare("PORT_LOCAL_TRANSMIT")) {log_this(key + "=" + to_string(PORT_LOCAL_TRANSMIT),false,1);}
				else if (!key.compare("PORT_LOCAL_RECEIVE")) {log_this(key + "=" + to_string(PORT_LOCAL_RECEIVE),false,1);}
				else if (!key.compare("PORT_HANDSHAKE")) {log_this(key + "=" + to_string(PORT_HANDSHAKE),false,1);}
				else if (!key.compare("LOG_LEVEL")) {log_this(key + "=" + to_string(LOG_LEVEL),false,1);}
				else if (!key.compare("DELETE_LOG")) {log_this(key + "=" + to_string(DELETE_LOG),false,1);}
				else if (!key.compare("UPDATE_INTERVAL")) {log_this(key + "=" + to_string(UPDATE_INTERVAL),false,1);}
				else if (!key.compare("MAX_MSG_SENT")) {log_this(key + "=" + to_string(MAX_MSG_SENT),false,1);}
				else if (!key.compare("FAIL_TO_RECEIVE_VALUE")) {log_this(key + "=" + to_string(FAIL_TO_RECEIVE_VALUE),false,1);}
				else if (!key.compare("MAX_NUMBER_OF_NODES")) {log_this(key + "=" + to_string(MAX_NUMBER_OF_NODES),false,1);}
				else if (!key.compare("TOKEN_LOSS_TIMEOUT_VALUE")) {log_this(key + "=" + to_string(TOKEN_LOSS_TIMEOUT_VALUE),false,1);}
				else if (!key.compare("RETRANSMISSION_TIMEOUT_VALUE")) {log_this(key + "=" + to_string(RETRANSMISSION_TIMEOUT_VALUE),false,1);}
				else if (!key.compare("JOIN_TIMEOUT_VALUE")) {log_this(key + "=" + to_string(JOIN_TIMEOUT_VALUE),false,1);}
				else if (!key.compare("CONSENSUS_TIMEOUT_VALUE")) {log_this(key + "=" + to_string(CONSENSUS_TIMEOUT_VALUE),false,1);}
				else if (!key.compare("TOKEN_ROUND_INTERVAL")) {log_this(key + "=" + to_string(TOKEN_ROUND_INTERVAL),false,1);}
				else if (!key.compare("NUMBER_OF_LOG_ELEMENTS")) {log_this(key + "=" + to_string(NUMBER_OF_LOG_ELEMENTS),false,1);}
				else if (!key.compare("WAIT_FOR_WELCOME_TIMEOUT")) {log_this(key + "=" + to_string(WAIT_FOR_WELCOME_TIMEOUT),false,1);}
				else if (!key.compare("LOCAL_MACHINE")) {log_this(key + "=" + to_string(LOCAL_MACHINE),false,1);}
				else if (!key.compare("GLOBAL_ID")) {log_this(key + "=" + to_string(global_id),false,1);}
				else if (!key.compare("TEST_SETUP")) {log_this(key + "=" + to_string(TEST_SETUP),false,1);}
				else if (!key.compare("MLS_HANDLING_ERROR_RESET_TIMER")) {log_this(key + "=" + to_string(MLS_HANDLING_ERROR_RESET_TIMER),false,1);}
				else if (!key.compare("MLS_HANDLING_ERROR_COUNTER_THRESHOLD")) {log_this(key + "=" + to_string(MLS_HANDLING_ERROR_COUNTER_THRESHOLD),false,1);}
				else if (!key.compare("JOIN_SESSION_TIMEOUT")) {log_this(key + "=" + to_string(JOIN_SESSION_TIMEOUT),false,1);}
				else if (!key.compare("CREATE_OWN_GROUP")) {log_this(key + "=" + to_string(CREATE_OWN_GROUP),false,1);}
				else if (!key.compare("NO_UPDATE_THRESHOLD")) {log_this(key + "=" + to_string(NO_UPDATE_THRESHOLD),false,1);}
				else if (!key.compare("REMOVE_MEMBERS_NOT_UPDATING")) {log_this(key + "=" + to_string(REMOVE_MEMBERS_NOT_UPDATING),false,1);}
				else if (!key.compare("REMOVE_MLS_MEMBERS_NOT_IN_TOTEM_GROUP")) {log_this(key + "=" + to_string(REMOVE_MLS_MEMBERS_NOT_IN_TOTEM_GROUP),false,1);}
				else if (!key.compare("MAX_BUFFER_LENGTH")) {log_this(key + "=" + to_string(MAX_BUFFER_LENGTH),false,1);}
				else if (!key.compare("MULTICAST_ADDRESS_EXTERNAL")) {log_this(key + "=" + MULTICAST_ADDRESS_EXTERNAL,false,1);}
				else if (!key.compare("SOCKET_ADDRESS")) {log_this(key + "=" + SOCKET_ADDRESS,false,1);}
				else if (!key.compare("BROADCAST_ADDRESS_INTERNAL")) {log_this(key + "=" + BROADCAST_ADDRESS_INTERNAL,false,1);}
				else if (!key.compare("SEND_APPLICATION_MESSAGES_TO_SELF")) {log_this(key + "=" + to_string(SEND_APPLICATION_MESSAGES_TO_SELF),false,1);}
				else if (!key.compare("RECEIVE_PACKET_HANDSHAKE_TIMEOUT")) {log_this(key + "=" + to_string(RECEIVE_PACKET_HANDSHAKE_TIMEOUT),false,1);}
				else if (!key.compare("NO_MESSAGE_SENT_THRESHOLD")) {log_this(key + "=" + to_string(NO_MESSAGE_SENT_THRESHOLD),false,1);}
				else if (!key.compare("REMOVE_MEMBERS_NOT_SENDING_MESSAGES")) {log_this(key + "=" + to_string(REMOVE_MEMBERS_NOT_SENDING_MESSAGES),false,1);}
				else if (!key.compare("APPLICATION_MESSAGE_HEARTBEAT_INTERVAL")) {log_this(key + "=" + to_string(APPLICATION_MESSAGE_HEARTBEAT_INTERVAL),false,1);}
				else if (!key.compare("REMOVE_MEMBER_FAIL_THRESHOLD")) {log_this(key + "=" + to_string(REMOVE_MEMBER_FAIL_THRESHOLD),false,1);}
				else if (!key.compare("PRINT_TO_CONSOLE")) {log_this(key + "=" + to_string(PRINT_TO_CONSOLE),false,1);}
				else if (!key.compare("MAX_EPOCH_STATE_HISTORY_SIZE")) {log_this(key + "=" + to_string(MAX_EPOCH_STATE_HISTORY_SIZE),false,1);}
				else if (!key.compare("REMOVE_DUPLICATES")) {log_this(key + "=" + to_string(REMOVE_DUPLICATES),false,1);}
				else if (!key.compare("RECENT_MESSAGE_THRESHOLD_FOR_UPDATE")) {log_this(key + "=" + to_string(RECENT_MESSAGE_THRESHOLD_FOR_UPDATE),false,1);}
				else if (!key.compare("BROADCAST_FOREIGN_MESSAGE")) {log_this(key + "=" + to_string(BROADCAST_FOREIGN_MESSAGE),false,1);}
				else if (!key.compare("USE_MULTICAST")) {log_this(key + "=" + to_string(USE_MULTICAST),false,1);}
				else if (!key.compare("UNICAST_DRONE_START")) {log_this(key + "=" + to_string(UNICAST_DRONE_START),false,1);}
				else if (!key.compare("UNICAST_DRONE_END")) {log_this(key + "=" + to_string(UNICAST_DRONE_END),false,1);}
				else if (!key.compare("UNICAST_GCS_START")) {log_this(key + "=" + to_string(UNICAST_GCS_START),false,1);}
				else if (!key.compare("UNICAST_GCS_END")) {log_this(key + "=" + to_string(UNICAST_GCS_END),false,1);}
			} 
        }	
    }
}
string convert_to_string(vector<MessageObject> q) {
	vector<MessageObject> q2(q);
	string s = "";
	if (q.empty()) return "empty";
	while (!q2.empty()) {
		s += q2.front().type; 
		s += "!";
		if (q2.front().content.size() >= 20) s += q2.front().content.substr(q2.front().content.size()-20) + ",";
		q2.erase(q2.begin());
	}
	if (s.size() == 0) return "";
	return s.substr(0,s.size()-1);
}
string convert_to_string(vector<string> q) {
	vector<string> q2(q);
	string s = "";
	if (q.empty()) return "empty";
	while (!q2.empty()) {
		s += q2.front().substr(0,1); 
		s += "!";
		if (q2.front().size() >= 20) s += q2.front().substr(q2.front().size()-20) + ",";
		q2.erase(q2.begin());
	}
	if (s.size() == 0) return "";
	return s.substr(0,s.size()-1);
}

// Converts list of unsigned int to string separated by ','.
string convert_to_string(list<unsigned int> l) {
    string s = "";
    if (l.size() == 0) return s;
    for (unsigned int i: l) s += to_string(i) + ",";
    if (s.size() == 0) return "";
    return s.substr(0,s.length()-1);
}
string get_thread_id_as_string(thread* t) {
	auto myid = t->get_id();
	stringstream ss;
	ss << myid;
	return ss.str().substr(ss.str().length() - 4);
}
bool is_set_equal(vector<unsigned int> set1, vector<unsigned int> set2) {
        sort(set1.begin(),set1.end());
        sort(set2.begin(),set2.end());
        return set1 == set2;
    }
bool is_subset(vector<unsigned int> set_largest, vector<unsigned int> set_smallest) {
	if (set_smallest.size() == 0) return true;
	if (set_smallest.size() > set_largest.size()) return false;
	sort(set_largest.begin(),set_largest.end());
	sort(set_smallest.begin(),set_smallest.end());
	int j = 0;
	for (int i = 0; i < set_largest.size(); i++) {
		if (set_smallest[j] == set_largest[i]) {
			j++;
			if (j == set_smallest.size()) return true;
		}
	}
	return false;
}
vector<unsigned int> get_difference_set(vector<unsigned int> set_largest, vector<unsigned int> set_smallest) {
	vector<unsigned int> new_set;
	sort(set_largest.begin(),set_largest.end());
	sort(set_smallest.begin(),set_smallest.end());
	if (set_smallest.size() == 0) return set_largest;
	
	int j = 0;
	for (int i = 0; i < set_largest.size(); i++) {
		if (set_smallest[j] != set_largest[i]) {
			new_set.push_back(set_largest[i]);
		}
		else {
			j++;
			if (j == set_smallest.size()) {
				for (int k = i+1; k < set_largest.size(); k++) new_set.push_back(set_largest[k]);
				return new_set;
			}
		}
	}
	return new_set;
}
vector<unsigned int> merge_vectors(vector<unsigned int> v1, vector<unsigned int> v2) {
	vector<unsigned int> merged;
	merged.reserve(v1.size() + v2.size());
	merged.insert(merged.end(), v1.begin(), v1.end());
	merged.insert(merged.end(), v2.begin(), v2.end());
	sort(merged.begin(), merged.end());
	merged.erase(unique(merged.begin(), merged.end() ), merged.end());
	return merged;
}
vector<unsigned int> get_equal_id(vector<unsigned int> v1, vector<unsigned int> v2) {
	vector<unsigned int> equal;
	for (unsigned int i: v1) {
		for (unsigned int j: v2) {
			if (i == j) {
				equal.push_back(i);
				break;
			}
		}
	}
	return equal;
}
chrono::milliseconds get_current_time() {
	return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
}
chrono::milliseconds get_time_since(chrono::milliseconds ms) {
	return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()-ms);
}
string generate_random_string(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}
vector<unsigned char> read_binary_file(string file_name) {
	log_this("Reading binary file " + file_name, false, 4);
	
	// Open binary file
    ifstream binaryFile(file_name, ios::binary);

    // Get the file length
    binaryFile.seekg(0, binaryFile.end);
    int length = binaryFile.tellg();
    binaryFile.seekg(0, binaryFile.beg);

    // Read the file contents into a vector
    vector<unsigned char> buffer(length);
    binaryFile.read((char *)buffer.data(), length);

    // Close the file
    binaryFile.close();
	remove(file_name.data());

	return buffer;
}