#include "totem.h"

using namespace std;

// MemberInfo
MemberInfo::MemberInfo() {}
MemberInfo::MemberInfo(string s) {
    vector<string> split = split_string(s,",");
    if (split.size() < 5) return;
    id = get_int_from_string(split[0]);
    ring_id = get_int_from_string(split[1]);
    my_aru = get_int_from_string(split[2]);
    high_delivered = get_int_from_string(split[3]);
    received_flg = get_int_from_string(split[4]);
}
MemberInfo::MemberInfo(unsigned int id): id(id) {}
MemberInfo::MemberInfo(unsigned int id, unsigned int ring_id, unsigned int my_aru, unsigned int high_delivered, bool received_flg):
    id(id),ring_id(ring_id),my_aru(my_aru),high_delivered(high_delivered),received_flg(received_flg) {}

// Converts values of MemberInfo to string separated by ','.
string MemberInfo::convert_to_string() {
    return to_string(id) + "," + to_string(ring_id) + "," + to_string(my_aru) + "," + to_string(high_delivered) + "," + 
        to_string(received_flg);
}


int get_int_from_string(std::string s) {
    int i = 0;
    try {
        i = stoi(s);
    }
    catch(...) {}
    return i;
}
std::vector<std::string> split_string(std::string s, std::string delimiter) {
    size_t pos = 0;
    std::string token;
    std::vector<std::string> split;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        split.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    split.push_back(s);
    return split;
}

// Converts vector of unsigned int to string separated by ','.
string convert_to_string(vector<unsigned int> vec) {
    string s = "";
    if (vec.size() == 0) return s;
    for (unsigned int i: vec) s += to_string(i) + ",";
    if (s.size() == 0) return "";
    return s.substr(0,s.length()-1);
}

// Converts vector of bool to string separated by ','.
string convert_to_string(vector<bool> vec) {
    string s = "";
    if (vec.size() == 0) return s;
    for (bool i: vec) s += to_string(i) + ",";
    if (s.size() == 0) return "";
    return s.substr(0,s.length()-1);
}

// Converts vector of MemberInfo to string where MemberInfo objects are separated by ',', and internal MemberInfo values separated by '!'.
string convert_to_string(vector<MemberInfo> vec) {
    string s = "";
    if (vec.size() == 0) return s;
    for (MemberInfo i: vec) s += i.convert_to_string() + "!";
    if (s.size() == 0) return "";
    return s.substr(0,s.length()-1);
}

// Converts string of ',' separated values into vector of unsigned int.
vector<unsigned int> string_to_vector(string s) {
    vector<unsigned int> vec;
    if (s.size() == 0) return vec;
    for (string a : split_string(s,",")) vec.push_back(get_int_from_string(a));
    return vec;
}

// Converts string of ',' separated values into list of unsigned int.
list<unsigned int> string_to_list(string s) {
    list<unsigned int> l;
    if (s.size() == 0) return l;
    for (string a : split_string(s,",")) l.push_back(get_int_from_string(a));
    return l;
}

// Converts string of ',' and '!' separated values into vector of MemberInfo objects.
vector<MemberInfo> string_to_vector_member(string s) {
    vector<MemberInfo> vec;
    if (s.size() == 0) return vec;
    for (string a : split_string(s,"!")) vec.push_back(MemberInfo(a));
    return vec;
}

// RegularMessage
RegularMessage::RegularMessage(string message) {
    vector<string> split = split_string(message);
    if (split.size() < 6) return;
    sender_id = get_int_from_string(split[1]);
    ring_id = get_int_from_string(split[2]);
    seq = get_int_from_string(split[3]);
    conf_id = get_int_from_string(split[4]);
    contents = split[5];
} 

// JoinMessage
JoinMessage::JoinMessage(string message) {
    vector<string> split = split_string(message);
    if (split.size() < 5) return;
    sender_id = get_int_from_string(split[1]);
    proc_set = string_to_vector(split[2]);
    fail_set = string_to_vector(split[3]);
    ring_seq = get_int_from_string(split[4]);
}

// RecoveryMessage
RecoveryMessage::RecoveryMessage(string message) {
    vector<string> split = split_string(message);
    if (split.size() < 8) return;
    sender_id = get_int_from_string(split[1]);
    ring_id = get_int_from_string(split[2]);
    old_ring_id = get_int_from_string(split[3]);
    seq = get_int_from_string(split[4]);
    old_seq = get_int_from_string(split[5]);
    conf_id = get_int_from_string(split[6]);
    contents = split[7];
} 
RecoveryMessage::RecoveryMessage(unsigned int sender_id, unsigned int ring_id, unsigned int old_ring_id, unsigned int seq, unsigned int old_seq, 
    string contents): old_ring_id(old_ring_id), old_seq(old_seq) {
    this->sender_id = sender_id;
    this->ring_id = ring_id;
    this->seq = seq;
    this->contents = contents;
}
RecoveryMessage::RecoveryMessage(RegularMessage message, unsigned int new_ring_id, unsigned int new_seq):
    old_ring_id(message.get_ring_id()), old_seq(message.get_seq()) {
    this->sender_id = message.get_sender_id();
    this->ring_id = new_ring_id;
    this->seq = new_seq;
    this->contents = message.get_contents();
}

// ApplicationMessage
ApplicationMessage::ApplicationMessage(string message) {
    vector<string> split = split_string(message);
    if (split.size() < 2) return;
    contents = split[1];
}

// Token
// Increments 'seq' and returns the incremented value.
unsigned int Token::increment_seq() {
    if (aru == seq) aru++;
    return ++seq;    
}

// RegularToken
RegularToken::RegularToken(string message) {
    vector<string> split = split_string(message);
    if (split.size() < 7) return;
    ring_id = get_int_from_string(split[1]);
    token_seq = get_int_from_string(split[2]);
    seq = get_int_from_string(split[3]);
    aru = get_int_from_string(split[4]);
    aru_id = get_int_from_string(split[5]);
    rtr = string_to_list(split[6]);
}
RegularToken::RegularToken(unsigned int ring_id, unsigned int token_seq, unsigned int seq, unsigned int aru, unsigned int aru_id, list<unsigned int> rtr) {
    this->ring_id = ring_id;
    this->token_seq = token_seq;
    this->seq = seq;
    this->aru = aru;
    this->aru_id = aru_id;
    this->rtr = rtr;
}
// Adds to 'rtr' all values larger than 'aru', and smaller or equal to 'seq', if they are not already present.
void RegularToken::add_missing_to_rtr(unsigned int aru) {
    for (unsigned int i = aru + 1; i < seq + 1; i++) {
        if (find(rtr.begin(), rtr.end(), i) == rtr.end()) {
            rtr.push_back(i);
        }
    }
}

// CommitToken
CommitToken::CommitToken(string message) {
    vector<string> split = split_string(message);
    if (split.size() < 8) return;
    ring_id = get_int_from_string(split[1]);
    token_seq = get_int_from_string(split[2]);
    seq = get_int_from_string(split[3]);
    aru = get_int_from_string(split[4]);
    aru_id = get_int_from_string(split[5]);
    memb_list = string_to_vector_member(split[6]);
    memb_index = get_int_from_string(split[7]);
}
CommitToken::CommitToken(RegularToken token) {
    this->ring_id = token.get_ring_id();
    this->token_seq = token.get_token_seq();
    this->seq = token.get_seq();
    this->aru = token.get_aru();
    this->aru_id = token.get_aru_id();
}
// Initilizes 'memb_list' with empty MemberInfo object with the IDs present int 'memb_id'. 
void CommitToken::initialize_memb_list(vector<unsigned int> memb_id) {
    for (unsigned int i: memb_id) {
        memb_list.push_back(MemberInfo(i));
    }
}
// Returns vector of unsigned int with IDs of members in 'memb_list'.
vector<unsigned int> CommitToken::get_memb_id() {
    vector<unsigned int> memb_id;
    for (MemberInfo i: memb_list) {
        memb_id.push_back(i.id);
    }
    return memb_id;
}
// Swap element in 'memb_list' with same ID as 'info'.
void CommitToken::set_memb_list(MemberInfo info) {
    for (int i = 0; i < memb_list.size(); i++) {
        if (memb_list[i].id == info.id) {
            memb_list[i] = info;
            return;
        }
    }
}
// Returns lowest aru for the members in 'memb'.
unsigned int CommitToken::get_low_ring_aru(vector<unsigned int> memb) {
    int low_ring_aru = -1; 
    for (MemberInfo m: memb_list) {
        if (find(memb.begin(),memb.end(),m.id) != memb.end()) {
            if (low_ring_aru == -1) low_ring_aru = m.my_aru;
            else if (m.my_aru < low_ring_aru) low_ring_aru = m.my_aru;
        }
    }
    if (low_ring_aru == -1) return 0;
    return low_ring_aru;
}
// Returns highest delivered message for the members in 'memb'.
unsigned int CommitToken::get_high_ring_delivered(vector<unsigned int> memb) {
    unsigned int high_ring_delivered = 0; 
    for (MemberInfo m: memb_list) {
        if (find(memb.begin(),memb.end(),m.id) != memb.end()) {
            if (m.high_delivered > high_ring_delivered) high_ring_delivered = m.high_delivered;
        }
    }
    return high_ring_delivered;
}

// RecoveryToken
RecoveryToken::RecoveryToken(string message) {
    vector<string> split = split_string(message);
    if (split.size() < 8) return;
    ring_id = get_int_from_string(split[1]);
    token_seq = get_int_from_string(split[2]);
    seq = get_int_from_string(split[3]);
    aru = get_int_from_string(split[4]);
    aru_id = get_int_from_string(split[5]);
    rtr = string_to_list(split[6]);
    retrans_flg = get_int_from_string(split[7]);
}
RecoveryToken::RecoveryToken(CommitToken token) {
    this->ring_id = token.get_ring_id();
    this->token_seq = token.get_token_seq();
    this->seq = token.get_seq();
    this->aru = token.get_aru();
    this->aru_id = token.get_aru_id();
}

///// Totem /////
///// CORE FUNCTIONS /////
#pragma region
Totem::Totem() {
    consensus.reserve(MAX_NUMBER_OF_NODES+1);
    for (int i = 0; i < MAX_NUMBER_OF_NODES+1; i++) consensus.push_back(false);
}
Totem::Totem(unsigned int my_id): my_id(my_id) {
    consensus.reserve(MAX_NUMBER_OF_NODES+1);
    for (int i = 0; i < MAX_NUMBER_OF_NODES+1; i++) consensus.push_back(false);
    consensus[my_id] = true;
    my_proc_set = {this->my_id};
}
void Totem::start() {
    fail_to_rcv_const = FAIL_TO_RECEIVE_VALUE;

    consensus[my_id] = true;
    my_proc_set = {this->my_id};
    shift_to_gather();
}
void Totem::shift_to_gather() {
    join_message = JoinMessage(my_id,my_proc_set,my_fail_set,my_ring_id);
    log_this_totem("Join message sent:",false,2,join_message.serialize(),false);
    broadcast_message(&join_message);
    cancel_token_loss_timer();
    cancel_token_retransmission_timer();
    reset_join_timer();
    reset_consensus_timer();

    for (int i = 0; i < consensus.size(); i++) consensus[i] = false;
    consensus[my_id] = true;

    state = GATHER_STATE;   
    log_this_totem("Shift to gather state",false,2,
        "proc_set=" + convert_to_string(my_proc_set) + ",fail_set=" + convert_to_string(my_fail_set));
}
void Totem::shift_to_commit() {
    set_my_received_flg();

    MemberInfo info = MemberInfo(my_id,my_ring_id,my_aru,my_aru,my_received_flg);
    commit_token.set_memb_list(info);
    commit_token.set_memb_index(my_id);
    commit_token.set_token_seq(0);

    my_token_seq = 0;
    old_ring_id = my_ring_id;
    my_ring_id = commit_token.get_ring_id();
    my_memb = commit_token.get_memb_id();
    log_this("Commit token sent: " + commit_token.serialize(),false,3);
    transmit_token(&commit_token);


    cancel_join_timer();
    cancel_consensus_timer();
    reset_token_loss_timer();
    reset_token_retransmission_timer();

    state = COMMIT_STATE;
    log_this_totem("Shift to commit state",false,2,"proc_set=" + convert_to_string(my_proc_set) + ",fail_set=" 
        + convert_to_string(my_fail_set));
}
void Totem::shift_to_recovery() {
    set_my_received_flg();
    commit_token.set_seq(0);
    commit_token.set_memb_index(my_id);
    commit_token.set_memb_list(MemberInfo(MemberInfo(my_id,my_ring_id,my_aru,my_aru,my_received_flg)));
    log_this_totem("Commit token sent: " + commit_token.serialize(),false,2,"",false);
    transmit_token(&commit_token);

    my_new_memb = commit_token.get_memb_id();
    my_trans_memb = get_equal_id(my_new_memb,my_memb);

    for (MemberInfo m: commit_token.get_memb_list()) {
        if (find(my_trans_memb.begin(),my_trans_memb.end(),m.id) != my_trans_memb.end() &&
            !m.received_flg) {
            my_deliver_memb = my_trans_memb;
            low_ring_aru = commit_token.get_low_ring_aru(my_deliver_memb);
            high_ring_delivered = commit_token.get_high_ring_delivered(my_deliver_memb);
            for (RegularMessage m: receive_message_queue) {
                if (m.get_seq() > low_ring_aru) retrans_message_queue.push_back(m);
            }
            break;
        }
    }

    old_aru = my_aru;
    my_aru = 0;
    my_aru_count = 0;
    reset_token_loss_timer();
    reset_token_retransmission_timer();

    receive_message_queue.clear();

    state = RECOVERY_STATE;
    log_this_totem("Shift to recovery state",false,2,"proc_set=" + convert_to_string(my_proc_set) + ",fail_set=" +
        convert_to_string(my_fail_set));
}
void Totem::shift_to_operational() {
    if (count(my_deliver_memb.begin(),my_deliver_memb.end(),my_id) > 0) {
        deliver_out_of_order_messages(&receive_recovery_message_queue);
    }
    
    my_memb = my_new_memb;
    my_proc_set = my_memb;
    my_fail_set.clear();
    my_new_memb.clear();
    my_trans_memb.clear();
    my_deliver_memb.clear();
    retrans_message_queue.clear();
    receive_recovery_message_queue.clear();
    for (int i = 0; i < consensus.size(); i++) consensus[i] = false;
    consensus[my_id] = true;
    my_retrans_flg_count = 0;
    my_install_seq = 0;

    state = OPERATIONAL_STATE;
    log_this_totem("Shift to operational state, proc_set=" + convert_to_string(my_proc_set) + ",fail_set=" + 
        convert_to_string(my_fail_set),false,2,"");
}
int Totem::receive(string message, string address, uint16_t port) {
    int type = -1;
    int id = -1;
    JoinMessage join;
    RegularMessage regular;
    if (message.find_first_of("|") != string::npos) type = get_int_from_string(message.substr(0,message.find_first_of("|")));
    if (state != OPERATIONAL_STATE) log_this_totem("Totem received message ", false, 2,"type=" + to_string(type) + ",state=" + 
        to_string(state),false);
    log_this_name(message,3,"log_totem_message_received");
    switch(type) {
        case REGULAR_MESSAGE:
            regular = RegularMessage(message);
            if (state==OPERATIONAL_STATE) op_receive(regular);
            else if (state==GATHER_STATE) ga_receive(regular);
            else if (state==COMMIT_STATE) co_receive(regular);
            else if (state==RECOVERY_STATE) re_receive(regular);
            id = regular.get_sender_id();
            break;
        case JOIN_MESSAGE:
            join = JoinMessage(message);
            if (state==OPERATIONAL_STATE) op_receive(join);
            else if (state==GATHER_STATE) ga_receive(join);
            else if (state==COMMIT_STATE) co_receive(join);
            else if (state==RECOVERY_STATE) re_receive(join);
            id = join.get_sender_id();
            break;
        case RECOVERY_MESSAGE:
            if (state==RECOVERY_STATE) re_receive(RecoveryMessage(message));
            break;
        case REGULAR_TOKEN:
            if (state==OPERATIONAL_STATE) op_receive(RegularToken(message));
            else if (state==GATHER_STATE) ga_receive(RegularToken(message));
            break;
        case COMMIT_TOKEN:
            if (state==GATHER_STATE) ga_receive(CommitToken(message));
            else if (state==COMMIT_STATE) co_receive(CommitToken(message));
            else if (state==RECOVERY_STATE) re_receive(CommitToken(message));
            break;
        case RECOVERY_TOKEN:
            if (state==RECOVERY_STATE) re_receive(RecoveryToken(message));
            else if (state==OPERATIONAL_STATE) op_receive(RegularToken(message));
            else if (state==GATHER_STATE) ga_receive(RegularToken(message));
            else if (state==COMMIT_STATE) co_receive(RegularToken(message));
            break;
        case APPLICATION_MESSAGE:
            deliver_message(ApplicationMessage(message),address,port);
            break;
    }
    return id;
}
void Totem::token_loss_timeout() {
    log_this_totem("Token loss timeout",false,2,"state=" + to_string(state),false);
    switch(state) {
        case OPERATIONAL_STATE:
            op_token_loss_timeout();
            break;
        case GATHER_STATE:
            ga_token_loss_timeout();
            break;
        case COMMIT_STATE:
            co_token_loss_timeout();
            break;
        case RECOVERY_STATE:
            re_token_loss_timeout();
            break;
    }
}
void Totem::token_retransmission_timeout() {
    log_this_totem("Token retransmission timeout",false,2,"state=" + to_string(state),false);
    cancel_token_retransmission_timer();
    switch(state) {
        case OPERATIONAL_STATE:
            op_token_retransmission_timeout();
            break;
        case GATHER_STATE:
            break;
        case COMMIT_STATE:
            break;
        case RECOVERY_STATE:
            re_token_retransmission_timeout();
            break;
    }
}
void Totem::join_timeout() {
    log_this_totem("Join timeout",false,2,"state=" + to_string(state),false);
    cancel_join_timer();
    switch(state) {
        case OPERATIONAL_STATE:
            break;
        case GATHER_STATE:
            ga_join_timeout();
            break;
        case COMMIT_STATE:
            co_token_loss_timeout();
            break;
        case RECOVERY_STATE:
            break;
    }
}
void Totem::consensus_timeout() {
    log_this_totem("Consensus timeout",false,2,"state=" + to_string(state),false);
    cancel_consensus_timer();
    switch(state) {
        case OPERATIONAL_STATE:
            break;
        case GATHER_STATE:
            ga_consensus_timeout();
            break;
        case COMMIT_STATE:
            break;
        case RECOVERY_STATE:
            break;
    }
}
#pragma endregion

///// PUBLIC FUNCTIONS /////
unsigned int Totem::get_member() {
    if (my_memb[0] == my_id) return get_next_memb();
    else return my_memb[0];
}

///// PRIVATE FUNCTIONS /////
#pragma region
void Totem::set_my_received_flg() {
    vector<MemberInfo> member = commit_token.get_memb_list();
    int largest = 0;
    unsigned int r = 0;
    for (int i = 0; i < member.size(); i++) {
        if (member[i].my_aru > largest) largest = member[i].my_aru;
        if (member[i].ring_id > r) r = member[i].ring_id;
    }
    my_received_flg = largest == my_aru;

    if (my_ring_id != r || not_part_of_old_ring) {
        my_received_flg = true;
        not_part_of_old_ring = true;
    }

}
bool Totem::remove_new_ring_messages() {
    for (int i = 0; i < receive_message_queue.size(); i++) {
        if (receive_message_queue[i].get_ring_id() == my_ring_id) {
            receive_message_queue.erase(receive_message_queue.begin()+i);
            return false;
        }
    }
    return true;
}
unsigned int Totem::get_next_memb() {
    vector<unsigned int>::iterator pos = find(my_memb.begin(),my_memb.end(),my_id);
    if (pos+1 == my_memb.end()) return my_memb[0];
    else return *(pos+1);
}
void Totem::delete_from_receive_message_queue(unsigned int aru) {
    vector<int> to_be_deleted;
    for (int i = 0; i < receive_message_queue.size(); i++) {
        if (receive_message_queue[i].get_seq() <= aru) {
            receive_message_queue.erase(receive_message_queue.begin() + i);
            delete_from_receive_message_queue(aru);
            return;
        }
    }
}
bool Totem::update_aru() {
    for (int i = 0; i < receive_recovery_message_queue.size(); i++) {
        if (receive_recovery_message_queue[i].get_seq() == my_aru + 1) {
            my_aru++;
            return update_aru();
        }
    }
    for (int i = 0; i < receive_message_queue.size(); i++) {
        if (receive_message_queue[i].get_seq() == my_aru + 1) {
            my_aru++;
            return update_aru();
        }
    }
    return true;
}
bool Totem::update_old_aru() {
    for (int i = 0; i < receive_recovery_message_queue.size(); i++) {
        if (receive_recovery_message_queue[i].get_old_seq() == old_aru + 1) {
            old_aru++;
            return update_old_aru();
        }
    }
    return true;
}
bool Totem::deliver_out_of_order_messages(vector<RegularMessage>* queue) {
    for (int i = 0; i < queue->size(); i++) {
        if ((*queue)[i].get_seq() == my_aru + 1) {
            deliver_message((*queue)[i]);
            return false;
        }
    }
    return true;
}
bool Totem::deliver_out_of_order_recovery_messages(vector<RegularMessage>* queue) {
    for (int i = 0; i < queue->size(); i++) {
        if ((*queue)[i].get_seq() == old_aru + 1) {
            deliver_recovery_message((*queue)[i]);
            return false;
        }
    }
    return true;
}
void Totem::deliver_out_of_order_messages(vector<RecoveryMessage>* queue) {
    vector<RegularMessage> regular_queue;
    for (int i = 0; i < queue->size(); i++) {
        if ((*queue)[i].get_old_seq() > old_aru) regular_queue.push_back((*queue)[i].get_regular_message());
    }

    while(!deliver_out_of_order_recovery_messages(&regular_queue)) {
        continue;
    }
}
bool Totem::check_for_consensus() {
    for (unsigned int i: get_difference_set(my_proc_set,my_fail_set)) {
        if (!consensus[i]) {
            return false;
        }
    }
    return true;
}
void Totem::add_to_my_fail_set(unsigned int id) {
    if (count(my_fail_set.begin(),my_fail_set.end(),id) == 0)
        my_fail_set = merge_vectors(my_fail_set,{id});
}
#pragma endregion

///// OPERATIONAL STATE /////
#pragma region
void Totem::op_receive(RegularMessage message) {
    if (message.get_ring_id() != my_ring_id) {
        op_receive_foreign_message(message.get_sender_id());
        return;
    }
    cancel_token_retransmission_timer();
    if (message.get_seq() > my_aru) {
        receive_message_queue.push_back(message);
    }
    
    if (message.get_seq() == my_aru + 1) {
        deliver_message(message);
        while(!deliver_out_of_order_messages(&receive_message_queue)) {
            continue;
        }
    }
}
void Totem::op_receive(RegularToken token) {
    log_this_totem("Token received: " + token.serialize(), false,3, "", true);
    int allowed_msg = MAX_MSG_SENT;
    if (!check_token(token)) return;

    // Retransmit messages from rtr
    for (unsigned int i: token.get_rtr()) {
        for (int j = 0; j < receive_message_queue.size(); j++) {
            if (i == receive_message_queue[j].get_seq() && allowed_msg > 0) {
                broadcast_message(&(receive_message_queue[j]));
                token.remove_rtr_entry(i);
                allowed_msg--;
            }
        }
    }

    if (allowed_msg >= 2 && state == OPERATIONAL_STATE) mls();

    // Sending messages from queue
    while(get_new_message_queue_size() > 0 && allowed_msg > 0) { 
        RegularMessage message = RegularMessage(my_id, token.get_ring_id(), token.increment_seq(), get_new_message_queue_front());
        broadcast_message(&message);
        receive_message_queue.push_back(message);
        if (my_aru == message.get_seq() - 1) {
            deliver_message(message);
        }
        new_message_queue_pop();
        allowed_msg--;
    }

    
    if (my_aru < token.get_aru() || my_id == token.get_aru_id() || token.get_aru_id() == 0) {
        token.set_aru(my_aru);
        if (token.get_aru() == token.get_seq()) token.set_aru_id(0);
        else token.set_aru_id(my_id);
    }
    else if (my_id == token.get_aru_id()) token.set_aru(my_aru);
    
    if (token.get_aru() == last_token_aru && token.get_aru_id() != 0) my_aru_count++;
    else my_aru_count = 0;
    if (my_aru_count > fail_to_rcv_const && token.get_aru_id() == my_id) {
        //add_to_my_fail_set(token.get_aru_id());
        log_this_totem("(my_aru_count > fail_to_rcv_const && token.get_aru_id() == my_id",false,3,"",false);
        shift_to_gather();
    }
    else {
        if (token.get_seq() > my_aru) {
            token.add_missing_to_rtr(my_aru);
        }
        cancel_token_loss_timer();
        cancel_token_retransmission_timer();
        token.increment_token_seq();
        
        thread thread1 = thread(transmit_regular_token,token,get_next_memb(),my_memb.size());
        thread1.detach();
        
        reset_token_loss_timer();
        reset_token_retransmission_timer();
    }
    // Delete safe messages from receive_message_queue
    if (token.get_aru() >= last_token_aru) delete_from_receive_message_queue(last_token_aru);
    else delete_from_receive_message_queue(token.get_aru());
    my_token_seq = token.get_token_seq();
    last_token_aru = token.get_aru();
    
    regular_token = token;
}
void Totem::op_receive(JoinMessage message) {
    log_this_totem("Join received in OP",false,2,message.serialize(),false);
    ga_receive(message);
}
void Totem::op_receive_foreign_message(unsigned int sender_id) {
    log_this_totem("Foreign message received in OP",false,2, "sender_id=" + to_string(sender_id),false);
    my_proc_set = merge_vectors(my_proc_set,{sender_id});
    shift_to_gather();
}
void Totem::op_token_retransmission_timeout() {
    transmit_token(&regular_token);
    reset_token_retransmission_timer();
}
#pragma endregion

///// GATHER STATE /////
#pragma region
void Totem::ga_receive(RegularMessage message) {
    if (message.get_ring_id() != my_ring_id) {
        ga_receive_foreign_message(message.get_sender_id());
        return;
    }
    op_receive(message);
}
void Totem::ga_receive(JoinMessage message) {
    log_this_totem("Join received in GA",false,2,message.serialize() + ",consensus=" + convert_to_string(consensus),false);
    if (message.get_sender_id() == my_id) return;
    vector<unsigned int> message_fail_set = message.get_fail_set();
    if (is_set_equal(my_proc_set,message.get_proc_set()) && is_set_equal(my_fail_set,message.get_fail_set())) {
        consensus[message.get_sender_id()] = true;
        vector<unsigned int> difference = get_difference_set(my_proc_set,my_fail_set);
        if (difference.size() == 0);
        else if (my_id <= difference[0]) {
            if (check_for_consensus()) {
                commit_token = CommitToken(regular_token);
                commit_token.set_ring_id(max(my_ring_id,message.get_ring_seq())+4);
                commit_token.initialize_memb_list(difference);
                log_this_totem("Creating new commit token: " + commit_token.serialize(),false,2,"",false);
                shift_to_commit();
            }
        }
    }
    else if (is_subset(my_proc_set,message.get_proc_set()) && is_subset(my_fail_set,message.get_fail_set()));
    else if (count(my_fail_set.begin(),my_fail_set.end(),message.get_sender_id()));
    else if (count(message_fail_set.begin(),message_fail_set.end(),my_id));
    else {
        my_proc_set = merge_vectors(my_proc_set,message.get_proc_set());
        if (count(message_fail_set.begin(), message_fail_set.end(), my_id)) {
            add_to_my_fail_set(message.get_sender_id());
        }
        else {
            my_fail_set = merge_vectors(my_fail_set,message_fail_set);
        }
        shift_to_gather();
    }
    if (state == OPERATIONAL_STATE) shift_to_gather();
}
void Totem::ga_receive(CommitToken token) {
    log_this_totem("CommitToken received in GA",false,2,token.serialize(),false);
    vector<unsigned int> difference = get_difference_set(my_proc_set,my_fail_set);
    if (is_set_equal(difference,token.get_memb_id()) && token.get_ring_id() > my_ring_id) {
        commit_token = token;
        shift_to_commit();
    }
}
void Totem::ga_receive_foreign_message(unsigned int sender_id) {
    log_this_totem("Foreign message received in GA",false,2, "sender_id=" + to_string(sender_id),false);
    if (find(my_proc_set.begin(),my_proc_set.end(), sender_id) == my_proc_set.end()) {
        my_proc_set = merge_vectors(my_proc_set,{sender_id});
        shift_to_gather();
    }
}
void Totem::ga_join_timeout() {
    join_message = JoinMessage(my_id,my_proc_set,my_fail_set,my_ring_id);
    broadcast_message(&join_message);
    reset_join_timer();
}
void Totem::ga_consensus_timeout(bool from_token_loss) {
    vector<unsigned int> difference = get_difference_set(my_proc_set,my_fail_set);
    if (!check_for_consensus()) {
        for (unsigned int i: difference) {
            if (!consensus[i]) {
                add_to_my_fail_set(i);
            }
        }
        shift_to_gather();
    }
    else {
        for (int i = 0; i < consensus.size(); i++) {
            consensus[i] = false;
        }
        consensus[my_id] = true;
        reset_token_loss_timer();
        if (from_token_loss) shift_to_gather();
    }
    if (difference.size() == 1) {
        commit_token = CommitToken(regular_token);
        commit_token.set_ring_id(my_ring_id+4);
        commit_token.initialize_memb_list(difference);
        log_this_totem("Creating new commit token: " + commit_token.serialize(),false,2,"",false);
        shift_to_commit();
    }
}
void Totem::ga_token_loss_timeout() {
    if (token_loss_timer_state == GATHER_STATE) ga_consensus_timeout(true);
}
void Totem::ga_print() {
    cout << "my_proc_set=";
    for (unsigned int i:my_proc_set) {
        cout << i << ",";
    }
    cout << "my_fail_set=";
    for (unsigned int i:my_fail_set) {
        cout << i << ",";
    }
    cout << "consensus=";
    for (unsigned int i:consensus) {
        cout << i << ",";
    }
    cout << endl;
}
#pragma endregion

///// COMMIT STATE /////
#pragma region
void Totem::co_receive(RegularMessage message) {
    if (message.get_ring_id() == my_ring_id) op_receive(message);
}
void Totem::co_receive(JoinMessage message) {
    log_this_totem("Join received in CO",false,2,message.serialize(),false);
    if (find(my_new_memb.begin(),my_new_memb.end(),message.get_sender_id()) != my_new_memb.end() 
        && message.get_ring_seq() >= my_ring_id) {
        ga_receive(message);
        shift_to_gather();
    }
}
void Totem::co_receive(CommitToken token) {
    log_this_totem("CommitToken received in CO",false,2,token.serialize(),false);
    if (token.get_ring_id() == my_ring_id) {
        commit_token = token;
        shift_to_recovery();
    }
}
void Totem::co_print() {
    cout << "my_proc_set=" << convert_to_string(my_proc_set) << ",my_fail_set=" << convert_to_string(my_fail_set) << 
        ",consensus=";
    for (unsigned int i:consensus) {
        cout << i << ",";
    }
    cout << endl;
}
#pragma endregion

///// RECOVERY STATE /////
#pragma region
void Totem::re_receive(RegularMessage message) {
    if (message.get_ring_id() != my_ring_id) return;

    reset_token_retransmission_timer();

    if (message.get_seq() > my_aru) {
        receive_message_queue.push_back(message);
        if (message.get_seq() == my_aru + 1) my_aru++;
    }
}
void Totem::re_receive(RecoveryToken token) {
    log_this_totem("RecoveryToken received in RE",false,2,token.serialize(),false);
    int allowed_msg = MAX_MSG_SENT;
    
    if (!check_token(token.get_regular_token())) return;

    // Retransmit messages from rtr
    for (unsigned int i: token.get_rtr()) {
        for (int j = 0; j < receive_recovery_message_queue.size(); j++) {
            if (i == receive_recovery_message_queue[j].get_seq() && allowed_msg > 0) {
                broadcast_message(&(receive_recovery_message_queue[j]));
                token.remove_rtr_entry(i);
                allowed_msg--;
            }
        }
    }
    
    // Sending messages from queue
    while(retrans_message_queue.size() > 0 && allowed_msg > 0) {
        RecoveryMessage message = RecoveryMessage(retrans_message_queue.front(),my_ring_id,token.increment_seq());
        broadcast_message(&message);
        receive_recovery_message_queue.push_back(message);
        if (my_aru == message.get_seq() - 1) {
            my_aru = message.get_seq();
        }

        retrans_message_queue.pop_front();
        allowed_msg--;
    }

    update_aru();

    if (my_aru < token.get_aru() || my_id == token.get_aru_id() || token.get_aru_id() == 0) {
        token.set_aru(my_aru);
        if (token.get_aru() == token.get_seq()) token.set_aru_id(0);
        else token.set_aru_id(my_id);
    }
    else if (my_id == token.get_aru_id()) token.set_aru(my_aru);
    
    if (token.get_aru() == last_token_aru && token.get_aru_id() != 0) my_aru_count++;
    else my_aru_count = 0;

    if (my_aru_count > fail_to_rcv_const && token.get_aru_id() == my_id) {
        //add_to_my_fail_set(token.get_aru_id());
        shift_to_gather();
    }
    else {
        if (token.get_seq() > my_aru) {
            token.add_missing_to_rtr(my_aru);
        }
        token.increment_token_seq();
        transmit_token(&token);
        reset_token_loss_timer();
        reset_token_retransmission_timer();
    }

    my_token_seq = token.get_token_seq();
    last_token_aru = token.get_aru();

    if (retrans_message_queue.size() > 0) {
        if (!token.get_retrans_flg()) {
            set_retrans_flag = true;
            token.set_retrans_flg(true);
        }
    }
    else if (token.get_retrans_flg() && set_retrans_flag) {
        token.set_retrans_flg(false);
        set_retrans_flag = false;
    }

    if (!token.get_retrans_flg()) my_retrans_flg_count++;
    else my_retrans_flg_count = 0;

    if (my_retrans_flg_count == 2) my_install_seq = token.get_seq();
    if (my_retrans_flg_count >= 2 && my_aru >= my_install_seq && !my_received_flg) {
        my_received_flg = true;
        my_deliver_memb = my_trans_memb;
    }
    if (token.get_aru() >= my_install_seq) token_aru_bigger_than_my_install_seq_count++;
    else token_aru_bigger_than_my_install_seq_count = 0;

    if (my_retrans_flg_count >= 3 && token_aru_bigger_than_my_install_seq_count >= 2) shift_to_operational();

    recovery_token = token;
    transmit_token(&recovery_token);
}
void Totem::re_receive(JoinMessage message) {
    log_this_totem("Join received in RE",false,2,message.serialize(),false);
    if (find(my_new_memb.begin(),my_new_memb.end(),message.get_sender_id()) != my_new_memb.end() 
        && message.get_ring_seq() >= my_ring_id) {
        co_receive(message);
        re_token_loss_timeout();
    }
}
void Totem::re_receive(CommitToken token) {
    log_this_totem("CommitToken received in RE",false,2,token.serialize(),false);
    vector<unsigned int> difference = get_difference_set(my_proc_set,my_fail_set);
    if (my_id <= difference[0]) {
        recovery_token = RecoveryToken(token);
        if (retrans_message_queue.size() > 0) {
            set_retrans_flag = true;
            recovery_token.set_retrans_flg(true);
        }
        else {
            set_retrans_flag = false;
            recovery_token.set_retrans_flg(false);
        }
        
        transmit_token(&recovery_token);
        reset_token_loss_timer();
        reset_token_retransmission_timer();
    }
}
void Totem::re_receive(RecoveryMessage message) {
    if (message.get_ring_id() != my_ring_id) return

    reset_token_retransmission_timer();
    if (message.get_seq() > my_aru) {
        receive_recovery_message_queue.push_back(message);
        for (int i = 0; i < retrans_message_queue.size(); i++) {
            if (retrans_message_queue[i].get_seq() == message.get_seq()) {
                retrans_message_queue.erase(retrans_message_queue.begin() + i);
            }
        }
        if (message.get_seq() == my_aru + 1) my_aru++;
    }
}
void Totem::re_token_retransmission_timeout() {
    transmit_token(&recovery_token);
    reset_token_retransmission_timer();
}
void Totem::re_token_loss_timeout() {
    remove_new_ring_messages();

    deque<RegularMessage> empty;
    swap(retrans_message_queue,empty);

    update_old_aru();

    shift_to_gather();
}
void Totem::re_print() {
    cout << "my_id=" << my_id << ",my_retrans_flg_count=" << my_retrans_flg_count << ",token_aru_bigger_than_my_install_seq_count=" << 
        token_aru_bigger_than_my_install_seq_count << endl;
}
#pragma endregion

///// TIMEOUT FUNCTIONS /////
#pragma region
void Totem::reset_token_loss_timer() {
    token_loss_timer = true;
    token_loss_timer_start_time = get_current_time();
    if (state != OPERATIONAL_STATE) log_this_totem("set_token_loss_timer()",false,2,"",false);
    token_loss_timer_state = state;
}
void Totem::reset_token_retransmission_timer() {
    token_retransmission_timer = true;
    token_retransmission_timer_start_time = get_current_time();
    if (state != OPERATIONAL_STATE) log_this_totem("set_token_retransmission_timer()",false,2,"",false);
}
void Totem::reset_join_timer() {
    join_timer = true;
    join_timer_start_time = get_current_time();
    if (state != OPERATIONAL_STATE) log_this_totem("set_join_timer()",false,2,"",false);
}
void Totem::reset_consensus_timer() {
    consensus_timer = true;
    consensus_timer_start_time = get_current_time();
    if (state != OPERATIONAL_STATE) log_this_totem("set_consensus_timer()",false,2,"",false);
}
void Totem::cancel_token_loss_timer() {
    token_loss_timer = false;    
}
void Totem::cancel_token_retransmission_timer() {
    token_retransmission_timer = false;  
}
void Totem::cancel_join_timer() {
    join_timer = false;   
}
void Totem::cancel_consensus_timer() {
    consensus_timer = false; 
}
#pragma endregion