#ifndef MLS_H
#define MLS_H

#include "general.h"
#include "cryptography.h"

// MLS
#include <mls/session.h>

#define BASIC_CREDENTIAL 0

enum mls_message_type {
	KEYPACKAGE,
	COMMIT,
	WELCOME,
	UPDATE,
	ADD,
	REMOVE
};

std::string get_info_from_credential(mls::Credential cred);
mls::Client initialize_client(int id);
std::string protect_data(std::string input, mls::Session* session);
std::string unprotect_data(std::string input, mls::Session* session);
std::vector<uint32_t> get_leafs(mls::Session* session);
std::vector<std::tuple<unsigned int,uint32_t,uint32_t>> get_mls_group(mls::Session* session);
std::string convert_to_string(std::vector<std::tuple<unsigned int,uint32_t,uint32_t>> nodes);
std::string create_mls_message(int type, bytes message);
bool check_mls_type(char message_type, int type);
bytes_ns::bytes create_update_proposal(mls::Session* session);
bytes_ns::bytes create_add_proposal(mls::Session* session, std::string key_package);
bytes_ns::bytes create_remove_proposal(mls::Session* session, uint32_t leaf_index);
bool handle_handshake_data(mls::Session* session, bytes_ns::bytes proposal);
bool handle_welcome_message(mls::PendingJoin* join, mls::Session* session, std::string welcome);
std::tuple<bytes_ns::bytes, bytes_ns::bytes> create_commit_and_welcome(mls::Session* session);
bytes_ns::bytes create_commit(mls::Session* session);
mls::Session create_session();

#endif