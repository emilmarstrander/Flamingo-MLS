#ifndef NETWORK_H
#define NETWORK_H

#include "general.h"
#include <arpa/inet.h>

#define PORTS_IN_USE 20 // Specifies number of ports broadcast simulation is sent to when using local machine.

inline std::map<unsigned int, MessageObject> id_to_address;

int initialize_socket();
void bind_socket(int socket, int port);
int receive_packet(int sockfd, char* buffer, char* address=NULL, uint16_t* port=NULL, int timeout=0);
void send_packet(int socket, std::string message, std::string address, int port);
void broadcast_handshake(int socket, std::string message);
bool sent_from_self(std::string address, uint16_t port);
unsigned int get_id_from_address(std::string address, uint16_t port);
unsigned int get_id_from_address_application_messages(std::string address, uint16_t port);
std::string convert_buffer_to_string(void *ptr, int buflen);

#endif