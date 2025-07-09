#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <string>
#include <iomanip>
#include <openssl/sha.h>  // Needs OpenSSL for SHA-256

std::string sha256(const std::string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    std::stringstream ss;
    for (unsigned char i : hash)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
    return ss.str();
}

class Block {
public:
    int index;
    time_t timestamp;
    std::string data;
    std::string prevHash;
    std::string hash;
    int nonce;

    Block(int idx, std::string data, std::string prevHash)
        : index(idx), data(data), prevHash(prevHash), timestamp(time(0)), nonce(0) {
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::stringstream ss;
        ss << index << timestamp << data << prevHash << nonce;
        return sha256(ss.str());
    }

    void mineBlock(int difficulty) {
        std::string prefix(difficulty, '0');
        while (hash.substr(0, difficulty) != prefix) {
            nonce++;
            hash = calculateHash();
        }
        std::cout << "Block mined: " << hash << std::endl;
    }
};

class Blockchain {
private:
    std::vector<Block> chain;
    int difficulty;

    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0");
    }

public:
    Blockchain() {
        difficulty = 3;
        chain.push_back(createGenesisBlock());
    }

    void addBlock(std::string data) {
        Block newBlock(chain.size(), data, chain.back().hash);
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
    }

    void printChain() const {
        for (const auto& block : chain) {
            std::cout << "Index: " << block.index << "\n"
                      << "Timestamp: " << block.timestamp << "\n"
                      << "Data: " << block.data << "\n"
                      << "Prev Hash: " << block.prevHash << "\n"
                      << "Hash: " << block.hash << "\n"
                      << "Nonce: " << block.nonce << "\n"
                      << "------------------------\n";
        }
    }

    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            const Block& current = chain[i];
            const Block& previous = chain[i - 1];

            if (current.hash != current.calculateHash()) {
                std::cout << "Invalid block hash at index " << i << std::endl;
                return false;
            }
            if (current.prevHash != previous.hash) {
                std::cout << "Invalid previous hash at index " << i << std::endl;
                return false;
            }
        }
        return true;
    }
};

int main() {
    Blockchain myChain;

    myChain.addBlock("Alice pays Bob 10 BTC");
    myChain.addBlock("Bob pays Charlie 5 BTC");
    myChain.addBlock("Charlie pays Dave 2 BTC");

    myChain.printChain();

    std::cout << "\nBlockchain is " << (myChain.isChainValid() ? "VALID" : "INVALID") << "\n";

    return 0;
}