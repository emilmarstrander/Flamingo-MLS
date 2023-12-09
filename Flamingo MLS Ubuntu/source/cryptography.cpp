#include "cryptography.h"

using namespace std;

void create_certificate(EVP_PKEY* public_key, EVP_PKEY* signing_key, char* file_name, char** info) {
	X509 * x509;
	X509_NAME * name;
	FILE * f;

	string s = file_name;
	log_this("Creating certificate to file " + s, false, 3);
	
	x509 = X509_new();
	X509_gmtime_adj(X509_get_notBefore(x509), 0);
	X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);
	X509_set_pubkey(x509, public_key);
	name = X509_get_subject_name(x509);

	X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char *) info[0], -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *) info[1], -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *) info[2], -1, -1, 0);
	X509_set_issuer_name(x509, name);
	X509_sign(x509, signing_key, EVP_sha256());

	f = fopen(file_name, "wb");
	PEM_write_X509(f, x509);
	fclose(f);
}

EVP_PKEY* generate_EC_key(int curve) {
	EC_KEY *tempEcKey = NULL;
    EVP_PKEY *peerkey = NULL;

	log_this("Generating EC key with curve " + to_string(curve), false, 3);

    tempEcKey = EC_KEY_new_by_curve_name(curve);
	EC_KEY_generate_key(tempEcKey);
    if (!EC_KEY_check_key(tempEcKey)) {
		log_this("Error when generating key", false, 0);
	}

    peerkey = EVP_PKEY_new();
	EVP_PKEY_assign_EC_KEY(peerkey, tempEcKey);

	return peerkey;
}

EVP_PKEY* import_EC_key(string key) {
	EC_KEY *tempEcKey = NULL;
	EVP_PKEY *peerkey = NULL;

	log_this("Importing public key", false, 3);

	// Initializing with the correct curve
    tempEcKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
	 
	// Convert hex to oct
	unsigned char octets[65];
    for (int i = 0; i < key.length(); i += 2) {
        string byte = key.substr(i, 2);
        octets[i/2] = static_cast<unsigned char>(stoi(byte, nullptr, 16));
    }

	if (!EC_KEY_oct2key(tempEcKey, octets, 65, NULL)) {
		log_this("Error when importing public key", false, 0);
	}
	if (!EC_KEY_check_key(tempEcKey)) {
		log_this("Error: Key invalid", false, 0);
	}

    peerkey = EVP_PKEY_new();
	EVP_PKEY_assign_EC_KEY(peerkey, tempEcKey);

	return peerkey;
}

void convert_pem_to_der(string input, string output) {
	log_this("Converting PEM file " + input + " to DER file " + output, false, 4);

	// Read PEM file
    ifstream pemFile(input);
    string pem((istreambuf_iterator<char>(pemFile)), istreambuf_iterator<char>());

    // Convert PEM to DER
    BIO *bio = BIO_new_mem_buf(pem.c_str(), pem.length());
    X509 *x509 = PEM_read_bio_X509(bio, NULL, 0, NULL);
    if (x509 == NULL) log_this("Error reading PEM file",false,0);
    
    int length = i2d_X509(x509, NULL);
    if (length <= 0) log_this("Error converting PEM to DER", false, 0);   
    
    unsigned char *der = new unsigned char[length];
    unsigned char *p = der;
    i2d_X509(x509, &p);

    // Write DER file
    ofstream derFile(output, ios::binary);
    derFile.write((char *)der, length);
    derFile.close();
	remove(input.data());

    delete[] der;
    BIO_free(bio);
    X509_free(x509);
}

void convert_der_to_pem(string input, string output) {
	log_this("Converting DER file " + input + " to PEM file " + output, false, 4);

	// Read DER file
    BIO* der = BIO_new_file(input.data(), "rb");
    X509* x509 = d2i_X509_bio(der, NULL);

    // Write PEM file
    BIO* pem = BIO_new_file(output.data(), "wb");
    PEM_write_bio_X509(pem, x509);

	remove(input.data());
    BIO_free(der);
    BIO_free(pem);
    X509_free(x509);
}

int get_common_name_from_certificate(char *cn, size_t size, string cert_path) {
    OPENSSL_no_config();
    X509* cert;
    bool result = false;

    FILE *fp = fopen(cert_path.data(), "rb");
    if (!fp) {
        log_this("Error opening cert file: " + cert_path, false, 0);
    }
    else {
        cert = PEM_read_X509_AUX(fp, NULL, NULL, NULL);
        if (!cert) {
            log_this("Error reading PEM cert from " + cert_path, false, 0);
        }
        else {
            X509_NAME_oneline(X509_get_subject_name(cert), cn, size);
            X509_free(cert);
            result = true;
        }
        fclose(fp);
    }
    
    CONF_modules_unload(1);
    CONF_modules_free();

    return result;
}