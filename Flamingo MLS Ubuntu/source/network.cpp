#include "network.h"
#include <poll.h>

using namespace std;

// Initializes and returns socket
int initialize_socket() {
	int sockfd;

	log_this("Initializing socket", false, 4);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

// Binding socket to address and port
void bind_socket(int socket, int port) {
	struct sockaddr_in addr;

	log_this("Binding socket " + to_string(socket) + " to " + SOCKET_ADDRESS + ":" + to_string(port) + " and mulitcast address " 
		+ MULTICAST_ADDRESS_EXTERNAL, false, 1);
	
	// Forcefully attaching socket to address and port
	int opt = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        log_this("Error adding option SO_REUSEADDR", false, 0);
	}
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        log_this("Error adding option SO_REUSEPORT", false, 0);
	}
	
    // Allows broadcast
   	if (setsockopt(socket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt))) {
        log_this("Error adding option SO_BROADCAST", false, 0);
	}

	ip_mreq mreq;
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDRESS_EXTERNAL.data());
	mreq.imr_interface.s_addr = inet_addr(SOCKET_ADDRESS.data());

	if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_IF,(char *)&mreq.imr_interface.s_addr,
        sizeof(mreq.imr_interface.s_addr)) < 0) {
		perror("Error binding multicast group address IP_MULTICAST_IF");
	} 

	if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*) &mreq, sizeof(mreq)) < 0) {
		perror("Error binding multicast group address IP_ADD_MEMBERSHIP");
	}
	
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}

// Waiting to receive packet to buffer
int receive_packet(int sockfd, char* buffer, char* address, uint16_t* port, int timeout) {
	int numbytes;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	char s[INET_ADDRSTRLEN];

	addr_len = sizeof(their_addr);

	struct pollfd pfd = {.fd = sockfd, .events = POLLIN};
	if (timeout && !poll(&pfd, 1, timeout)) return -1;

	if ((numbytes = recvfrom(sockfd, buffer, MAX_BUFFER_LENGTH - 1, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1) {
		log_this("Error using recvfrom function", false, 0);
	}
	
	char message[1024];
	sprintf(message,"Got packet from %s", inet_ntop(their_addr.ss_family, &(((struct sockaddr_in*)&their_addr)->sin_addr), s, sizeof s));
	sprintf(message,"Packet is %d bytes long", numbytes);
	buffer[numbytes] = '\0';
    
	if (address == NULL || port == NULL) {
		return numbytes;
	}

    sprintf(address,"%s", inet_ntop(their_addr.ss_family, &(((struct sockaddr_in*)&their_addr)->sin_addr), s, sizeof s));
    *port = ntohs((((struct sockaddr_in*)&their_addr)->sin_port));

	string buffer_string = buffer;
	string address_string = address;
	log_this_name("Received from " + address_string + ":" + to_string(*port) + " - " + buffer_string.substr(0,numbytes),3,"log_receive_packet");

    return numbytes;
}

// Sending message to specified address and port
void send_packet(int socket, string message, string address, int port) {
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	int numbytes;

	log_this("Sending packet from socket " + to_string(socket) + " to " + address + ":" + to_string(port) + ". Message: " + message, false, 4);

	char* char_address = address.data();
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(char_address);
	addr.sin_port = htons(port);

	char* char_message = message.data();
	log_this_name(message + ",dst=" + address + ":" + to_string(port),3,"log_send_packet");

	if ((numbytes = sendto(socket, char_message, message.length(), 0, (struct sockaddr*)&addr, addrlen)) == -1) {
		log_this("Error sending packet from socket " + to_string(socket) + " to " + address + ":" + to_string(port) + ". Message: " + message,false,0);
	}
}
void broadcast_handshake(int socket, string message) {
	if (USE_MULTICAST) {
		send_packet(socket, message, MULTICAST_ADDRESS_EXTERNAL, PORT_HANDSHAKE);
		if (!LOCAL_MACHINE && !TEST_SETUP) return;
    	for (int i = 1; i <= PORTS_IN_USE; i++) {
        	send_packet(socket, message, MULTICAST_ADDRESS_EXTERNAL, PORT_HANDSHAKE+i);
    	}
	}
	else {
		string address_prefix = "192.168.12.";
		for (int i = UNICAST_DRONE_START; i < UNICAST_DRONE_END; i++) {
			send_packet(socket, message, address_prefix + to_string(i), PORT_HANDSHAKE);
		}
		for (int i = UNICAST_GCS_START; i < UNICAST_GCS_END; i++) {
			send_packet(socket, message, address_prefix + to_string(i), PORT_HANDSHAKE);
		}
	}
}
bool sent_from_self(string address, uint16_t port) {
	if (LOCAL_MACHINE) return port == PORT_HANDSHAKE + global_id;
	return address.compare(id_to_address[global_id].address) == 0;
}
unsigned int get_id_from_address(string address, uint16_t port) {
	for (auto const& [key, val] : id_to_address) {
		if (!val.address.compare(address) && val.port == port) {
			return key;
		}
	}
	return 0;
}
unsigned int get_id_from_address_application_messages(string address, uint16_t port) {
	if (LOCAL_MACHINE && !TEST_SETUP) {
		return port - PORT_LOCAL_RECEIVE;
	}
	else if (TEST_SETUP) {
		if (port - PORT_LOCAL_RECEIVE == 0) return 1;
		return port - PORT_LOCAL_RECEIVE;
	}

	for (auto const& [key, val] : id_to_address) {
		if (!val.address.compare(address)) {
			return key;
		}
	}
	return 0;
}
string convert_buffer_to_string(void *ptr, int buflen) {
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  stringstream ss;
	
  for (i=0; i<buflen; i++) {
	ss << buf[i];
  }
  return ss.str();
}