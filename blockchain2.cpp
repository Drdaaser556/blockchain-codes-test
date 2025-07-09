#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <string>
#include <iomanip>

// ================= Pure C++ SHA-256 =================
#include <cstring>
#include <stdint.h>

class SHA256 {
public:
    static std::string hash(const std::string& input);

private:
    static const uint32_t k[];
    static const uint32_t H0[];
    static uint32_t rotr(uint32_t x, uint32_t n);
    static void processChunk(const uint8_t* chunk, uint32_t* H);
    static void padMessage(const std::string& input, std::vector<uint8_t>& padded);
};

const uint32_t SHA256::k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
    0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
    0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
    0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
    0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
    0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
    0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
    0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
    0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const uint32_t SHA256::H0[8] = {
    0x6a09e667, 0xbb67ae85,
    0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c,
    0x1f83d9ab, 0x5be0cd19
};

uint32_t SHA256::rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

void SHA256::padMessage(const std::string& input, std::vector<uint8_t>& padded) {
    padded.assign(input.begin(), input.end());
    padded.push_back(0x80);
    while ((padded.size() + 8) % 64 != 0)
        padded.push_back(0x00);
    uint64_t bitLength = (uint64_t)input.size() * 8;
    for (int i = 7; i >= 0; --i)
        padded.push_back((bitLength >> (i * 8)) & 0xFF);
}

void SHA256::processChunk(const uint8_t* chunk, uint32_t* H) {
    uint32_t w[64];
    for (int i = 0; i < 16; ++i)
        w[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | (chunk[i * 4 + 3]);
    for (int i = 16; i < 64; ++i) {
        uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = H[0], b = H[1], c = H[2], d = H[3];
    uint32_t e = H[4], f = H[5], g = H[6], h = H[7];

    for (int i = 0; i < 64; ++i) {
        uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t temp1 = h + S1 + ch + k[i] + w[i];
        uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        h = g; g = f; f = e; e = d + temp1;
        d = c; c = b; b = a; a = temp1 + temp2;
    }

    H[0] += a; H[1] += b; H[2] += c; H[3] += d;
    H[4] += e; H[5] += f; H[6] += g; H[7] += h;
}

std::string SHA256::hash(const std::string& input) {
    std::vector<uint8_t> padded;
    padMessage(input, padded);

    uint32_t H[8];
    std::memcpy(H, H0, sizeof(H0));

    for (size_t i = 0; i < padded.size(); i += 64)
        processChunk(&padded[i], H);

    std::stringstream ss;
    for (int i = 0; i < 8; ++i)
        ss << std::hex << std::setw(8) << std::setfill('0') << H[i];
    return ss.str();
}
// ============ END Pure SHA-256 ===================

std::string sha256(const std::string& str) {
    return SHA256::hash(str);
}

// ============= BLOCKCHAIN CORE ==============
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
