# Node parameters
GLOBAL_ID: ID of the node. Used to identify node by Totem and MLS. Must be unique.

# Network parameters
PORT_APPLICATION: Port used to send MLS Application messages to other nodes.
PORT_HANDSHAKE: Port used to send MLS Handshake messages and Totem messages to other nodes.
PORT_LOCAL_TRANSMIT: Port used to transmit Application messages internally to the drone system.
PORT_LOCAL_RECEIVE: Port used to receive Application messages internally from the drone system.
MULTICAST_ADDRESS_EXTERNAL: Address used to transmit Application and Handshake messages to other nodes.
BROADCAST_ADDRESS_INTERNAL: Address used to transmit messages locally.
SOCKET_ADDRESS: Address of network interface used on the local machine.
SEND_APPLICATION_MESSAGES_TO_SELF: If 1, application messages from the drone system will be sent back to the drone system when multicasted.

# Program parameters
LOG_LEVEL: Specifies log level written to log files
LOCAL_MACHINE: If 1, the program will simulate multicast by transmitting unicast messages to each node.
DELETE_LOG: If 1, previous logs will be deleted every time program starts.
NUMBER_OF_LOG_ELEMENTS: Number of log elements recorded before old log file is deleted.
PRINT_TO_CONSOLE: If 1, the program will print to console.
MAX_BUFFER_LENGTH: Max length of udp receive buffer.
MAX_NUMBER_OF_NODES: Maximum number of nodes the program should handle. As of now only used by allocate resources by Totem.
RECEIVE_PACKET_HANDSHAKE_TIMEOUT: Specifies timeout when waiting for handshake messages from socket. This will determine how oftem Totem timers are checked.

# Totem parameters
TOKEN_ROUND_INTERVAL: How much time the token uses for going one time through the ring.
MAX_MSG_SENT: Max messages sent by a single node when having the regular token.
FAIL_TO_RECEIVE_VALUE: Determines how many rounds a node can fail to receive all messages, before it is added to my_fail_set.
TOKEN_LOSS_TIMEOUT_VALUE: Timeout value for token loss.
RETRANSMISSION_TIMEOUT_VALUE: Timeout value for retransmission of token.
JOIN_TIMEOUT_VALUE: Timeout value for not receiving join messages.
CONSENSUS_TIMEOUT_VALUE: Timout value for not reaching consensus in the gather state.

# MLS parameters
UPDATE_INTERVAL: How often the node should send MLS update messages.
WAIT_FOR_WELCOME_TIMEOUT: How long the node should wait for welcome message before sending new key package
JOIN_SESSION_TIMEOUT: How long the node should try to join new session before quiting and using the old session.
MLS_HANDLING_ERROR_RESET_TIMER: How long between MLS handling errors before counter is reset.
MLS_HANDLING_ERROR_COUNTER_THRESHOLD: How many MLS handling errors before trying to join new session
NO_UPDATE_THRESHOLD: How long a node can go without updating before it is removed from the MLS group.
NO_MESSAGE_SENT_THRESHOLD: How long the node should wait when receiving MLS handling errors, before joining new session.
APPLICATION_MESSAGE_HEARTBEAT_INTERVAL: How often the node sends heartbeat application messages, if no other message is sent.
REMOVE_MEMBERS_NOT_UPDATING: If 1, members not updating MLS key material will be removed from the group.
REMOVE_MLS_MEMBERS_NOT_IN_TOTEM_GROUP: If 1, members in MLS group but not in Totem group will be removed.
REMOVE_MEMBERS_NOT_SENDING_MESSAGES: If 1, members not sending application messages will be removed.
REMOVE_MEMBER_FAIL_THRESHOLD: Determines number of times the node tries to remove another node, before giving up.
REMOVE_DUPLICATES: If 1, the node will remove duplicates of itself in the MLS group.
MAX_EPOCH_STATE_HISTORY_SIZE: Number of epoch states that are kept. If 0, all epochs are kept.

# Testing parameters
CREATE_OWN_GROUP: If 1, the node will not try to join another session when starting up.
TEST_SETUP: If 1, special test setup is used.