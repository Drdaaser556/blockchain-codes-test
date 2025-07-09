#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <string>
#include <iomanip>
#include <openssl/sha.h>  // OpenSSL SHA-256

// SHA-256 hashing function using OpenSSL
std::string sha256(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), hash);

    std::stringstream ss;
    for (unsigned char c : hash) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return ss.str();
}

// Block structure
class Block {
public:
    int index;
    time_t timestamp;
    std::string data;
    std::string prevHash;
    std::string hash;
    int nonce;

    Block(int idx, const std::string& data, const std::string& prevHash)
        : index(idx), data(data), prevHash(prevHash), timestamp(std::time(nullptr)), nonce(0) {
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::stringstream ss;
        ss << index << timestamp << data << prevHash << nonce;
        return sha256(ss.str());
    }

    void mineBlock(int difficulty) {
        std::string target(difficulty, '0');
        while (hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculateHash();
        }
        std::cout << "Block mined: " << hash << "\n";
    }
};

// Blockchain class
class Blockchain {
private:
    std::vector<Block> chain;
    int difficulty;

    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0");
    }

public:
    Blockchain(int diff = 3) : difficulty(diff) {
        chain.push_back(createGenesisBlock());
    }

    void addBlock(const std::string& data) {
        Block newBlock(chain.size(), data, chain.back().hash);
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
    }

    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            const Block& curr = chain[i];
            const Block& prev = chain[i - 1];

            if (curr.hash != curr.calculateHash()) {
                std::cout << "Block " << i << " has invalid hash.\n";
                return false;
            }
            if (curr.prevHash != prev.hash) {
                std::cout << "Block " << i << " has invalid previous hash.\n";
                return false;
            }
        }
        return true;
    }

    void printChain() const {
        for (const auto& block : chain) {
            std::cout << "Index: " << block.index << "\n"
                      << "Timestamp: " << block.timestamp << "\n"
                      << "Data: " << block.data << "\n"
                      << "Prev Hash: " << block.prevHash << "\n"
                      << "Hash: " << block.hash << "\n"
                      << "Nonce: " << block.nonce << "\n"
                      << "--------------------------\n";
        }
    }
};

// Main function
int main() {
    Blockchain myChain;

    myChain.addBlock("Alice pays Bob 10 BTC");
    myChain.addBlock("Bob pays Charlie 5 BTC");
    myChain.addBlock("Charlie pays Dave 2 BTC");

    myChain.printChain();

    std::cout << "\nBlockchain is " << (myChain.isChainValid() ? "VALID" : "INVALID") << "\n";

    return 0;
}
