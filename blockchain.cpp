#include <iostream>
#include <ctime>
#include <vector>
#include <sstream>
#include <string>
#include <functional>

class Block {
public:
    int index;
    std::string data;
    std::string prevHash;
    std::string hash;
    time_t timestamp;

    Block(int idx, std::string data, std::string prevHash)
        : index(idx), data(data), prevHash(prevHash), timestamp(time(0)) {
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::stringstream ss;
        ss << index << timestamp << data << prevHash;
        std::hash<std::string> hasher;
        return std::to_string(hasher(ss.str()));
    }
};

class Blockchain {
private:
    std::vector<Block> chain;

    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0");
    }

public:
    Blockchain() {
        chain.push_back(createGenesisBlock());
    }

    Block getLatestBlock() const {
        return chain.back();
    }

    void addBlock(std::string data) {
        Block newBlock(chain.size(), data, getLatestBlock().hash);
        chain.push_back(newBlock);
    }

    void printChain() const {
        for (const auto& block : chain) {
            std::cout << "Index: " << block.index << "\n";
            std::cout << "Timestamp: " << block.timestamp << "\n";
            std::cout << "Data: " << block.data << "\n";
            std::cout << "Previous Hash: " << block.prevHash << "\n";
            std::cout << "Hash: " << block.hash << "\n";
            std::cout << "----------------------------\n";
        }
    }

    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            const Block& current = chain[i];
            const Block& previous = chain[i - 1];

            if (current.hash != current.calculateHash()) return false;
            if (current.prevHash != previous.hash) return false;
        }
        return true;
    }
};

int main() {
    Blockchain myBlockchain;

    myBlockchain.addBlock("First transaction");
    myBlockchain.addBlock("Second transaction");
    myBlockchain.addBlock("Third transaction");

    myBlockchain.printChain();

    std::cout << "Blockchain is " << (myBlockchain.isChainValid() ? "valid" : "invalid") << std::endl;

    return 0;
}
