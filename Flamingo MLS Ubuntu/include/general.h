#ifndef GENERAL_H
#define GENERAL_H

// General
#include <iostream>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
#include <chrono>

// Network parameters
inline int PORT_APPLICATION = 4900;
inline int PORT_HANDSHAKE = 4950;
inline int PORT_LOCAL_TRANSMIT = 4000;
inline int PORT_LOCAL_RECEIVE = 4100;
inline std::string MULTICAST_ADDRESS_EXTERNAL = "239.0.0.1";
inline std::string BROADCAST_ADDRESS_INTERNAL = "127.0.0.1";
inline std::string SOCKET_ADDRESS = "192.168.56.1";
inline int SEND_APPLICATION_MESSAGES_TO_SELF = 0;
inline int USE_MULTICAST = 1;
inline int UNICAST_DRONE_START = 1;
inline int UNICAST_DRONE_END = 20;
inline int UNICAST_GCS_START = 90;
inline int UNICAST_GCS_END = 95;

// Program parameters
inline int LOG_LEVEL = 1;
inline int LOCAL_MACHINE = 0;
inline int DELETE_LOG = 1;
inline int NUMBER_OF_LOG_ELEMENTS = 100000;
inline int PRINT_TO_CONSOLE = 1;
inline int MAX_BUFFER_LENGTH = 100000;
inline int MAX_NUMBER_OF_NODES = 100;
inline int RECEIVE_PACKET_HANDSHAKE_TIMEOUT = 1;

// Totem parameters
inline int TOKEN_ROUND_INTERVAL=10;
inline int MAX_MSG_SENT = 5;
inline int FAIL_TO_RECEIVE_VALUE = 5;
inline int TOKEN_LOSS_TIMEOUT_VALUE = 500;
inline int RETRANSMISSION_TIMEOUT_VALUE = 200;
inline int JOIN_TIMEOUT_VALUE = 200;
inline int CONSENSUS_TIMEOUT_VALUE = 500;
inline int BROADCAST_FOREIGN_MESSAGE = 1;

// MLS parameters
inline int UPDATE_INTERVAL = 100;
inline int WAIT_FOR_WELCOME_TIMEOUT = 1000;
inline int JOIN_SESSION_TIMEOUT = 5000;
inline int MLS_HANDLING_ERROR_RESET_TIMER = 2000;
inline int MLS_HANDLING_ERROR_COUNTER_THRESHOLD = 10;
inline int NO_UPDATE_THRESHOLD = 5000;
inline int NO_MESSAGE_SENT_THRESHOLD = 5000;
inline int APPLICATION_MESSAGE_HEARTBEAT_INTERVAL = 4000;
inline int REMOVE_MEMBER_FAIL_THRESHOLD = 3;
inline int REMOVE_MEMBERS_NOT_UPDATING = 1;
inline int REMOVE_MLS_MEMBERS_NOT_IN_TOTEM_GROUP = 1;
inline int REMOVE_MEMBERS_NOT_SENDING_MESSAGES = 1;
inline int REMOVE_DUPLICATES = 1;
inline int MAX_EPOCH_STATE_HISTORY_SIZE = 10;
inline int RECENT_MESSAGE_THRESHOLD_FOR_UPDATE = 5000;

// Testing parameters
inline int CREATE_OWN_GROUP = 0;
inline int TEST_SETUP = 0;

// Program variables
inline int event_counter = 0;
inline int log_count = 0;
inline std::mutex terminal_token;
inline int global_id;
inline std::mutex log_token;

// Class for handling message content, address, port and type in a single object
class MessageObject {
public:
	char type;
	std::string address;
    uint16_t port;
	std::string content;
	MessageObject();
	MessageObject(std::string message);
	MessageObject(std::string address, uint16_t port, std::string message);
	MessageObject(char type, std::string address, uint16_t port, std::string content);
	std::string get_message();
};

void log_this(std::string message, bool print=false, int log_level=1);
void log_this_totem(std::string message, bool print, int log_level,std::string additional,bool use_log_this=true);
void log_message(std::string message, int log_level=3);
void log_this_name(std::string message, int log_level, std::string file_name);
void read_configuration_file(std::string file="./program.conf");
void log_configuration_file(std::string file="./program.conf");
std::string convert_to_string(std::vector<MessageObject> q);
std::string convert_to_string(std::vector<std::string> q);
std::string convert_to_string(std::list<unsigned int> l);
std::string get_thread_id_as_string(std::thread* t);
bool is_set_equal(std::vector<unsigned int> set1, std::vector<unsigned int> set2);
bool is_subset(std::vector<unsigned int> set_largest, std::vector<unsigned int> set_smallest);
std::vector<unsigned int> get_difference_set(std::vector<unsigned int> set_largest, std::vector<unsigned int> set_smallest);
std::vector<unsigned int> merge_vectors(std::vector<unsigned int> v1, std::vector<unsigned int> v2);
std::vector<unsigned int> get_equal_id(std::vector<unsigned int> v1, std::vector<unsigned int> v2);
std::chrono::milliseconds get_current_time();
std::chrono::milliseconds get_time_since(std::chrono::milliseconds ms);
std::string generate_random_string(const int len);
std::vector<unsigned char> read_binary_file(std::string file_name);

#endif