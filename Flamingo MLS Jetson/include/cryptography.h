#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include "general.h"

//OpenSSL
#include <openssl/engine.h>
#include <openssl/conf.h>

void create_certificate(EVP_PKEY* public_key, EVP_PKEY* signing_key, char* file_name, char** info);
EVP_PKEY* generate_EC_key(int curve = NID_X9_62_prime256v1);
EVP_PKEY* import_EC_key(std::string key);
void convert_pem_to_der(std::string input, std::string output);
void convert_der_to_pem(std::string input, std::string output);
int get_common_name_from_certificate(char *cn, size_t size, std::string cert_path);

#endif