#include <openssl/sha.h>

std::string sha256(const std::string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    std::stringstream ss;
    for (unsigned char i : hash)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
    return ss.str();
}
