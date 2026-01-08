#include "PasswordService.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iostream>

std::string bytesToHex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

std::string PasswordService::generateSalt() {
    unsigned char salt[16];
    if (!RAND_bytes(salt, sizeof(salt))) {
        throw std::runtime_error("Failed to generate salt");
    }
    return bytesToHex(salt, sizeof(salt));
}

std::string PasswordService::hashPassword(const std::string& password, const std::string& salt) {
    std::string salted = password + salt;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted.data()), salted.size(), hash);
    return bytesToHex(hash, SHA256_DIGEST_LENGTH);
}

std::pair<std::string, std::string> PasswordService::hashPasswordWithSalt(const std::string& password) {
    std::string salt = generateSalt();
    std::string hash = hashPassword(password, salt);
    return { hash, salt };
}

bool PasswordService::verifyPassword(const std::string& password, const std::string& hashedPassword, const std::string& salt) {
    std::string newHash = hashPassword(password, salt);
    std::cout << "DEBUG: Input password = '" << password << "'" << std::endl;
    std::cout << "DEBUG: Salt = '" << salt << "'" << std::endl;
    std::cout << "DEBUG: Generated hash = '" << newHash << "'" << std::endl;
    std::cout << "DEBUG: Stored hash   = '" << hashedPassword << "'" << std::endl;
    std::cout << "DEBUG: Match? " << (newHash == hashedPassword ? "YES" : "NO") << std::endl;
    return newHash == hashedPassword;
}