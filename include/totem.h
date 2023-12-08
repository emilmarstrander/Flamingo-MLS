#ifndef TOTEM_H
#define TOTEM_h

#include "general.h"

#define DELIVER_TO_SELF 1

enum totem_message_type {
    REGULAR_MESSAGE,
    JOIN_MESSAGE,
    RECOVERY_MESSAGE,
    APPLICATION_MESSAGE,
    REGULAR_TOKEN,
    COMMIT_TOKEN,
    RECOVERY_TOKEN,
};
enum state {
    OPERATIONAL_STATE,
    GATHER_STATE,
    COMMIT_STATE,
    RECOVERY_STATE
};

// Class containing information of a member used in CommitToken
class MemberInfo{
public:
    unsigned int id;
    unsigned int ring_id = 0;
    unsigned int my_aru = 0;
    unsigned int high_delivered = 0;
    bool received_flg = 0;
    MemberInfo();
    MemberInfo(std::string s);
    MemberInfo(unsigned int id);
    MemberInfo(unsigned int id, unsigned int ring_id, unsigned int my_aru, unsigned int high_delivered, bool received_flg);
    std::string convert_to_string();
};

std::vector<std::string> split_string(std::string s, std::string delimiter="|");
int get_int_from_string(std::string s);
std::string convert_to_string(std::vector<unsigned int> vec);
std::string convert_to_string(std::vector<bool> vec);
std::string convert_to_string(std::vector<MemberInfo> vec);
std::vector<unsigned int> string_to_vector(std::string s);

// Converts string of ',' separated values into list of unsigned int.
std::list<unsigned int> string_to_list(std::string s);
std::vector<MemberInfo> string_to_vector_member(std::string s);

// Main class for all types of packets for Totem. (Both messages and tokens)
class Packet {
protected:
    int type;
public:
    Packet() {}
    virtual std::string serialize() {return "";}
};

// Parent class for all types of messages for Totem.
class Message: public Packet {
public:
    Message() {}
};

// Class for regular messages in Totem
class RegularMessage: public Message  {
protected:
    int type = REGULAR_MESSAGE;
    unsigned int sender_id;
    unsigned int ring_id;
    unsigned int seq;
    unsigned int conf_id = 0;
    std::string contents;
public:
    RegularMessage() {}
    RegularMessage(std::string message);
    RegularMessage(unsigned int sender_id, unsigned int ring_id, unsigned int seq, std::string contents, unsigned int conf_id=0):
        sender_id(sender_id), ring_id(ring_id), seq(seq), contents(contents), conf_id(conf_id) {}
    unsigned int get_sender_id() {return sender_id;}
    unsigned int get_seq() {return seq;}
    unsigned int get_ring_id() {return ring_id;}
    std::string get_contents() {return contents;}
    // Returns string of values separated by "|".
    std::string serialize() {
        return std::to_string(type) + "|" + std::to_string(sender_id) + "|" + std::to_string(ring_id) + "|" + std::to_string(seq) + "|" + 
            std::to_string(conf_id) + "|" + contents; 
    }
    void print() {
        std::cout << sender_id << "," << ring_id << "," << seq << "," << conf_id << "," << contents << std::endl;
    }
};

// Class for join messages in Totem
class JoinMessage: public Message  {
protected:
    int type = JOIN_MESSAGE;
    unsigned int sender_id;
    std::vector<unsigned int> proc_set;
    std::vector<unsigned int> fail_set;
    unsigned int ring_seq;
public:
    JoinMessage() {}
    JoinMessage(std::string message);
    JoinMessage(unsigned int sender_id, std::vector<unsigned int> proc_set, std::vector<unsigned int> fail_set, unsigned int ring_seq):
        sender_id(sender_id), proc_set(proc_set), fail_set(fail_set), ring_seq(ring_seq) {}
    unsigned int get_sender_id() {return sender_id;}
    std::vector<unsigned int> get_proc_set() {return proc_set;}
    std::vector<unsigned int> get_fail_set() {return fail_set;}
    unsigned int get_ring_seq() {return ring_seq;}
    // Returns string of values separated by "|".
    std::string serialize() {
        return std::to_string(type) + "|" + std::to_string(sender_id) + "|" + convert_to_string(proc_set) + "|" +
            convert_to_string(fail_set) + "|" + std::to_string(ring_seq); 
    }
    void print() {
        std::cout << sender_id << ",proc_set=" << convert_to_string(proc_set) << "fail_set=" << convert_to_string(fail_set) <<
            "ring_seq=" << ring_seq << std::endl;
    }
};

// Class for recovery messages in Totem. This class adds additional values to RegularMessage class.
class RecoveryMessage: public RegularMessage  {
protected:
    int type = RECOVERY_MESSAGE;
    unsigned int old_ring_id;
    unsigned int old_seq;
public:
    RecoveryMessage() {}
    RecoveryMessage(std::string message);
    RecoveryMessage(unsigned int sender_id, unsigned int ring_id, unsigned int old_ring_id, unsigned int seq, unsigned int old_seq, 
        std::string contents);
    RecoveryMessage(RegularMessage message, unsigned int new_ring_id, unsigned int new_seq);
    unsigned int get_old_seq() {return old_seq;}
    unsigned int get_old_ring_id() {return old_ring_id;}
    RegularMessage get_regular_message() {return RegularMessage(sender_id, old_ring_id, old_seq, contents, conf_id);}
    // Returns string of values separated by "|".
    std::string serialize() {
        return std::to_string(type) + "|" + std::to_string(sender_id) + "|" + std::to_string(ring_id) + "|" + std::to_string(old_ring_id) +"|" +
            std::to_string(seq) + "|" + std::to_string(old_seq) + "|" + std::to_string(conf_id) + "|" + contents; 
    }
    void print() {
        std::cout << sender_id << "," << ring_id << "," << old_ring_id << "," << seq << "," << old_seq <<"," << conf_id << "," 
        << contents << std::endl;
    }
};

// Class for application messages in Totem
class ApplicationMessage: public Message {
protected:
    int type = APPLICATION_MESSAGE;
    std::string contents;
public:
    ApplicationMessage() {}
    ApplicationMessage(std::string message);
    void set_contents(std::string contents) {this->contents = contents;}
    std::string get_contents() {return contents;}
    // Returns string of values separated by "|".
    std::string serialize() {
        return std::to_string(type) + "|" + contents; 
    }
};

// Parent class for all types of tokens for Totem.
class Token: public Packet {
protected:
    unsigned int ring_id;
    unsigned int token_seq = 0;
    unsigned int seq = 0;
    unsigned int aru = 0;
    unsigned int aru_id = 0;
public:
    Token() {}
    unsigned int get_seq() {return seq;}
    unsigned int get_token_seq() {return token_seq;}
    unsigned int get_aru() {return aru;}
    unsigned int get_aru_id() {return aru_id;}
    unsigned int get_ring_id() {return ring_id;}
    void increment_token_seq() {token_seq++;}
    unsigned int increment_seq();
    void set_ring_id(unsigned int ring_id) {this->ring_id = ring_id;}
    void set_token_seq(unsigned int token_seq) {this->token_seq = token_seq;}
    void set_seq(unsigned int seq) {this->seq = seq;}
    void set_aru(unsigned int aru) {this->aru = aru;}
    void set_aru_id(unsigned int aru_id) {this->aru_id = aru_id;}
};

// Class for regular token in Totem
class RegularToken: public Token {
protected:
    int type = REGULAR_TOKEN;
    std::list<unsigned int> rtr;
public:
    RegularToken() {}
    RegularToken(std::string message);
    RegularToken(unsigned int ring_id) {this->ring_id = ring_id;}
    RegularToken(unsigned int ring_id, unsigned int token_seq, unsigned int seq, unsigned int aru, unsigned int aru_id, std::list<unsigned int> rtr);
    std::list<unsigned int> get_rtr() {return rtr;}
    // Remove all elements with value 'i' from 'rtr'.
    void remove_rtr_entry(unsigned int i) {rtr.remove(i);}
    void add_missing_to_rtr(unsigned int aru);
    // Returns string of values separated by "|".
    std::string serialize() {
        return std::to_string(type) + "|" + std::to_string(ring_id) + "|" + std::to_string(token_seq) + "|" + std::to_string(seq) + "|" + 
            std::to_string(aru) + "|" + std::to_string(aru_id) +  "|" + convert_to_string(rtr); 
    }
    void print() {
        std::cout << "ring_id=" << ring_id << ",token_seq=" << token_seq << ",seq=" << seq << ",aru=" << aru << ",aru_id=" << aru_id 
            << ",rtr=" << convert_to_string(rtr) << std::endl;
    }
};

// Class for commit token in Totem
class CommitToken: public Token {
protected:
    int type = COMMIT_TOKEN;
    std::vector<MemberInfo> memb_list;
    unsigned int memb_index = 0;
public:
    CommitToken() {}
    CommitToken(std::string message);
    CommitToken(unsigned int ring_id) {
        this->ring_id = ring_id;
    }
    CommitToken(RegularToken token);
    void initialize_memb_list(std::vector<unsigned int> memb_id);
    std::vector<unsigned int> get_memb_id();
    void set_memb_list(MemberInfo info);
    void set_memb_index(unsigned int id) {memb_index = id;}
    std::vector<MemberInfo> get_memb_list() {return memb_list;}
    unsigned int get_low_ring_aru(std::vector<unsigned int> memb);
    unsigned int get_high_ring_delivered(std::vector<unsigned int> memb);
    // Returns string of values separated by "|".
    std::string serialize() {
        return std::to_string(type) + "|" + std::to_string(ring_id) + "|" + std::to_string(token_seq) + "|" + std::to_string(seq) + "|" + 
            std::to_string(aru) + "|" + std::to_string(aru_id) +  "|" + convert_to_string(memb_list) + "|" + std::to_string(memb_index); 
    }
    void print() {
        std::cout << "ring_id=" << ring_id << ",token_seq=" << token_seq << ",seq=" << seq << ",aru=" << aru << ",aru_id=" << aru_id << 
            ",memb_list=" << convert_to_string(memb_list) << "memb_index=" << memb_index << std::endl;
    }
};

// Class for recovery token in Totem. This class adds additional values to RegularToken class
class RecoveryToken: public RegularToken {
    protected:
        int type = RECOVERY_TOKEN;
        bool retrans_flg;
    public:
        RecoveryToken() {}
        RecoveryToken(std::string message);
        RecoveryToken(unsigned int ring_id) {
            this->ring_id = ring_id;
        }
        RecoveryToken(CommitToken token);
        RegularToken get_regular_token() {
            return RegularToken(ring_id,token_seq,seq,aru,aru_id,rtr);
        }
        bool get_retrans_flg() {return retrans_flg;}
        void set_retrans_flg(bool retrans_flg) {this->retrans_flg = retrans_flg;}
        // Returns string of values separated by "|".
        std::string serialize() {
            return std::to_string(type) + "|" + std::to_string(ring_id) + "|" + std::to_string(token_seq) + "|" + std::to_string(seq) + "|" + 
                std::to_string(aru) + "|" + std::to_string(aru_id) +  "|" + convert_to_string(rtr) + "|" + std::to_string(retrans_flg); 
        }
        void print() {
            std::cout << "ring_id=" << ring_id << ",token_seq=" << token_seq << ",seq=" << seq << ",aru=" << aru << ",aru_id=" << aru_id  
                << ",rtr=" << convert_to_string(rtr) << ",retrans_flg=" << retrans_flg << std::endl;
        }
};

class Totem {
    ///// VARIABLE INITIALIZATION /////
    #pragma region
protected:
    int state = GATHER_STATE;
    RegularToken regular_token;
    CommitToken commit_token;
    RecoveryToken recovery_token;
    JoinMessage join_message;
    int token_loss_timer_state = state;

    // OPERATIONAL STATE
    unsigned int my_id;
    unsigned int my_token_seq = 0;
    unsigned int my_aru = 0;
    unsigned int my_aru_count = 0;
    unsigned int last_token_aru = 0;
    std::queue<std::string> new_message_queue;
    std::vector<RegularMessage> receive_message_queue;
    int fail_to_rcv_const = FAIL_TO_RECEIVE_VALUE;

    // GATHER STATE
    unsigned int my_ring_id = 0;
    std::vector<unsigned int> my_proc_set;
    std::vector<unsigned int> my_fail_set;
    std::vector<bool> consensus;

    // COMMIT STATE
    bool my_received_flg = 1;
    bool set_retrans_flag = false;
    unsigned int old_ring_id;
    unsigned int old_aru;
    std::vector<unsigned int> my_memb;

    // RECOVERY STATE
    std::vector<unsigned int> my_new_memb;
    std::vector<unsigned int> my_trans_memb;
    std::vector<unsigned int> my_deliver_memb;
    unsigned int low_ring_aru;
    unsigned int high_ring_delivered;
    unsigned int my_install_seq = 0;
    std::deque<RegularMessage> retrans_message_queue;
    unsigned int my_retrans_flg_count = 0;
    std::vector<RecoveryMessage> receive_recovery_message_queue;
    unsigned int token_aru_bigger_than_my_install_seq_count = 0;
    bool not_part_of_old_ring = false;
    
public:
    bool token_loss_timer=false, token_retransmission_timer=false,join_timer=false,consensus_timer=false;
    std::chrono::milliseconds token_loss_timer_start_time, token_retransmission_timer_start_time, join_timer_start_time,
        consensus_timer_start_time;
    #pragma endregion

    ///// CORE FUNCTIONS /////
    #pragma region
public:
    Totem();
    Totem(unsigned int my_id);
    void set_id(unsigned int my_id){this->my_id = my_id;}
    void start();
    void shift_to_gather();
    void shift_to_commit();
    void shift_to_recovery();
    void shift_to_operational();
    int receive(std::string message, std::string address="", uint16_t port=0);
    void token_loss_timeout();
    void token_retransmission_timeout();
    void join_timeout();
    void consensus_timeout();
    #pragma endregion

    ///// PUBLIC FUNCTIONS /////
    #pragma region
    int get_state() {return state;}
    void add_message_to_queue(std::string content) {
        new_message_queue.push(content);
    }
    std::vector<unsigned int> get_my_memb() {return my_memb;}
    unsigned int get_member();
    #pragma endregion

protected:
    ///// PRIVATE FUNCTIONS /////
    #pragma region
    void set_my_received_flg();
    bool remove_new_ring_messages();
    bool check_token(RegularToken token) {
        return token.get_token_seq() >= my_token_seq && token.get_ring_id() == my_ring_id;
    }
    unsigned int get_next_memb();
    void delete_from_receive_message_queue(unsigned int aru);
    bool update_aru();
    bool update_old_aru();
    bool deliver_out_of_order_messages(std::vector<RegularMessage>* queue);
    bool deliver_out_of_order_recovery_messages(std::vector<RegularMessage>* queue);
    void deliver_out_of_order_messages(std::vector<RecoveryMessage>* queue);
    bool check_for_consensus();
    std::size_t get_new_message_queue_size();
    std::string get_new_message_queue_front();
    void new_message_queue_pop();
    void add_to_my_fail_set(unsigned int id);

    // Sending and deliver functions
    void broadcast_message(Message* message);
    void transmit_token(Token* token);
    void deliver_message(RegularMessage message);
    void deliver_recovery_message(RegularMessage message);
    void deliver_message(ApplicationMessage message, std::string address, uint16_t port);
    void mls();
    static void transmit_regular_token(RegularToken regular_token, unsigned int send_to_id, int group_size);
    
    #pragma endregion

    ///// OPERATIONAL STATE /////
    #pragma region
    void op_receive(RegularMessage message);
    void op_receive(RegularToken token);
    void op_receive(JoinMessage message);
    void op_receive_foreign_message(unsigned int sender_id);
    void op_token_retransmission_timeout();
    void op_token_loss_timeout() {shift_to_gather();}
    void op_print() {
        std::cout << "id=" << my_id << ",my_token_seq=" << my_token_seq << ",my_aru=" << my_aru << ",my_aru_count=" << my_aru_count << std::endl;
    }
    #pragma endregion

    ///// GATHER STATE /////
    #pragma region
    void ga_receive(RegularMessage message);
    void ga_receive(RegularToken token) {op_receive(token);}
    void ga_receive(JoinMessage message);
    void ga_receive(CommitToken token);
    void ga_receive_foreign_message(unsigned int sender_id);
    void ga_join_timeout();
    void ga_consensus_timeout(bool from_token_loss=false);
    void ga_token_loss_timeout();
    void ga_print();
    #pragma endregion

    ///// COMMIT STATE /////
    #pragma region
    void co_receive(RegularMessage message);
    void co_receive(JoinMessage message);
    void co_receive(CommitToken token);
    void co_join_timeout() {/*ga_join_timeout();*/}
    void co_token_loss_timeout() {shift_to_gather();}
    void co_print();
    #pragma endregion

    ///// RECOVERY STATE /////
    #pragma region
    void re_receive(RegularMessage message);
    void re_receive(RecoveryToken token);
    void re_receive(JoinMessage message);
    void re_receive(CommitToken token);
    void re_receive(RecoveryMessage message);
    void re_token_retransmission_timeout();
    void re_token_loss_timeout();
    void re_print();
    #pragma endregion

    ///// TIMEOUT FUNCTIONS /////
    #pragma region
    void reset_token_loss_timer();
    void reset_token_retransmission_timer();
    void reset_join_timer();
    void reset_consensus_timer();
    void cancel_token_loss_timer();
    void cancel_token_retransmission_timer();
    void cancel_join_timer();
    void cancel_consensus_timer();
    #pragma endregion
};

#endif