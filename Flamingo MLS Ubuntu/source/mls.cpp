#include "mls.h"

using namespace std;
using namespace mls;

string get_info_from_credential(Credential cred) {
	auto x509 = cred.get<X509Credential>();
	string der = "cert" + generate_random_string(10) + ".der";
	string pem = "cert" + generate_random_string(10) + ".pem";
	
	ofstream stream;
	stream.open(der, ios_base::binary);	
	if (stream.is_open()) {
		for(unsigned char i:x509.der_chain[0].data) {
			stream << i;
		}
		stream.close();
	}
	convert_der_to_pem(der,pem);

	char cn[64];
	if (!get_common_name_from_certificate(cn, 64, pem)) {
		remove(pem.data());
		return "";
	}
	remove(pem.data());

	return cn;
}

Client initialize_client(int id) {
	log_this("Initializing drone with id " + to_string(id), false, 1);

	// Creates a CipherSuite object with a specific ciphersuite
	CipherSuite suite_in = CipherSuite(CipherSuite::ID::P256_AES128GCM_SHA256_P256);
	
	// Generates a random Signature key based on the ciphersuite
	SignaturePrivateKey key = SignaturePrivateKey::generate(suite_in);
	//SignaturePrivateKey key = SignaturePrivateKey::derive(suite_in,(bytes) rand());
	//SignaturePrivateKey key = SignaturePrivateKey::derive(suite_in,(bytes) id);

	// Creates a basic Credential object based on the given id
	Credential credential = Credential::basic(id);

    ///// Creates a Credential object with a x509 certificate /////
    #if BASIC_CREDENTIAL == 0
	// Generates random Elliptic Curve key pair
	EVP_PKEY *AS_key_pair = generate_EC_key();

	// Tries to import Elliptic Curve key pair from SignaturePrivateKey-object 'key'
	string key_hex = to_hex(key.public_key.data);
	EVP_PKEY *node_public_key = import_EC_key(key_hex);

	// Creates certificates for AS and node
	string url = "flamingo" + to_string(id);
	string no = "NO";
	string ffi = "FFI";
	char* url_char = url.data();
	char* no_char = no.data();
	char* ffi_char = ffi.data();
	char* info[3] = {no_char,ffi_char,url_char};
	string pem_AS = "cert_AS" + to_string(global_id) + ".pem";
	string pem_node = "cert_node" + to_string(global_id) + ".pem";
	string der_AS = "cert_AS" + to_string(global_id) + ".der";
	string der_node = "cert_node" + to_string(global_id) + ".der";
	create_certificate(AS_key_pair, AS_key_pair, pem_AS.data(), info);
	create_certificate(node_public_key, AS_key_pair, pem_node.data(), info);

	// Converting certificates to .der
	convert_pem_to_der(pem_AS,der_AS);
	vector<unsigned char> cert_AS = read_binary_file(der_AS);
	convert_pem_to_der(pem_node,der_node);
	vector<unsigned char> cert_node = read_binary_file(der_node);

	vector<bytes> der_chain = {cert_node,cert_AS};

	credential = Credential::x509(der_chain);
	#endif

	log_this("Credential size = " + to_string(sizeof(credential)), false, 4);

	// Returns a Client object based on the values generated above
	return Client(suite_in, key, credential);
}

// Converts input to bytes and protects it. Returns a hex string
string protect_data(string input, Session* session) {
	// Convert string input to bytes
	bytes data = from_ascii(input);

	// Encrypts data using session
	bytes encrypted = session->protect(data);
	
	// Converts to hex
	return to_hex(encrypted);
}

// Unprotects input and converts to string
string unprotect_data(string input, Session* session) {
	// Decrypts data using session
	bytes received = session->unprotect(from_hex(input));
	
	// Converts data to character array
	unsigned char* s = received.data();

	// Converts character array to string
	string output = "";
	for (int i = 0; i < received.size(); i++) {
		output += received.at(i);
	}

	return output;
}

// Get leaf nodes of the MLS session
vector<uint32_t> get_leafs(Session* session) {
	vector<uint32_t> nodes;
	auto width = LeafCount((*session).tree().size);
	for (auto i = LeafIndex{ 0 }; i.val < width.val; i.val++) {
		if (!(*session).tree().node_at(i).blank()) {
			nodes.push_back(i.val);
		}
	}

	return nodes;
}

// Get id of members in MLS session
vector<tuple<unsigned int,uint32_t,uint32_t>> get_mls_group(Session* session) {
	vector<uint32_t> leaf_nodes = get_leafs(session);
	vector<tuple<unsigned int,uint32_t,uint32_t>> nodes;
	int j = 0;
	for (uint32_t i:leaf_nodes) {
		OptionalNode node;
		try {
			node = session->tree().node_at((LeafIndex) i);
		}
		catch (const exception& e) {
			string message = e.what();
			log_this("Error retrieving node information: " + message,false,0);
			nodes.clear();
			return nodes;
		}
		string credential = get_info_from_credential(node.leaf_node().credential);
		if (credential.length() == 0) {
			nodes.clear();
			return nodes;
		}
		unsigned int id = stoi(credential.substr(23));
		nodes.emplace_back(id,j,i);
		j++;
	}

	return nodes;
}

string convert_to_string(vector<tuple<unsigned int,uint32_t,uint32_t>> nodes) {
	string output = "";
	for(tuple<unsigned int,uint32_t,uint32_t> i: nodes) {
		output += "{" + to_string(get<0>(i)) + "," + to_string(get<1>(i)) + "," + to_string(get<2>(i)) + "}" + ",";
	}
	return output;
}

// Return string with type and message merged
string create_mls_message(int type, bytes message) {
    return to_string(type) + to_hex(message);
}

// Returns true if message_type equals type
bool check_mls_type(char message_type, int type) {
    return int(message_type - '0') == type;
}

bytes create_update_proposal(Session* session) {
	bytes proposal;
	try {
		proposal = session->update();
	}
	catch (const exception& e) {
		string message = e.what();
		log_this("Error creating update proposal: " + message, false, 0);
		return bytes();
	}
	return proposal;
}

bytes create_add_proposal(Session* session, string key_package) {
	bytes proposal, key_package_bytes = from_hex(key_package);
	try {
		proposal = session->add(key_package_bytes);
	} 
	catch (const exception& e) {
		string message = e.what();
		log_this("Error creating add proposal: " + message,false,0);
		return bytes();
	}
	return proposal;
}

bytes create_remove_proposal(Session* session, uint32_t leaf_index) {
	bytes proposal;
	try {
		proposal = session->remove(leaf_index);
	} 
	catch (const exception& e) {
		string message = e.what();
		log_this("Error creating remove proposal: " + message,false,0);
		return bytes();
	}
	return proposal;
}

bool handle_handshake_data(Session* session, bytes proposal) {
	try {
		session->handle(proposal);
	} 
	catch (const exception& e) {
		string message = e.what();
		log_this("Error handling handshake data: " + message,false,0);
		return false;
	}
	return true;
}

bool handle_welcome_message(PendingJoin* join, Session* session, string welcome) {
	bytes welcome_bytes = from_hex(welcome);
	try {
		*session = join->complete(welcome_bytes);
	}
	catch (const exception& e) {
		string message = e.what();
		log_this("Error processing welcome message: " + message, false, 0);
		return false;
	}
	session->size_of_history = MAX_EPOCH_STATE_HISTORY_SIZE;
	return true;
}

tuple<bytes, bytes> create_commit_and_welcome(Session* session) {
	tuple<bytes, bytes> info;
	try {
		info = session->commit();
	} 
	catch (const exception& e) {
		string message = e.what();
		log_this("Error creating commit: " + message, false, 0);
		return make_tuple(bytes(),bytes());
	}

	return info;
}

bytes create_commit(Session* session) {
	return get<1>(create_commit_and_welcome(session));
}

Session create_session() {
	srand(time(NULL));
	Client client = initialize_client(global_id);
    bytes group_id = bytes{
        static_cast<unsigned char>(rand() % 256), 
        static_cast<unsigned char>(rand() % 256), 
        static_cast<unsigned char>(rand() % 256), 
        static_cast<unsigned char>(rand() % 256)
        };
    Session session = client.begin_session(group_id);
	session.size_of_history = MAX_EPOCH_STATE_HISTORY_SIZE;
	return session;
}