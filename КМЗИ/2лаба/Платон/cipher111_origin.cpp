#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <bitset>

using namespace std;

// S-���� ��� �������� 1
uint8_t s_block_znach[16] = { 12, 7, 2, 5, 14, 0, 13, 10, 15, 1, 11, 8, 9, 3, 4, 6 };

uint16_t s_block(uint16_t input_block)
{
    uint8_t parts[4];
    for (int i = 0; i < 4; ++i) {
        parts[i] = (input_block >> (4 * (3 - i))) & 0x0F;
    }

    uint16_t result = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t sbox_output = s_block_znach[parts[i] & 0x0F];
        result |= (sbox_output << (4 * (3 - i)));
    }
    return result;
}

uint16_t p_block(uint16_t input_block)
{
    const int A = 29;
    const int B = 12;
    const int m = 16;
    uint16_t result = 0;

    for (int i = 0; i < 16; i++) {
        int new_pos = (A * i + B) % m;
        if (input_block & (1 << i)) {
            result |= (1 << new_pos);
        }
    }
    return result;
}

uint16_t round_function(uint16_t input_block, uint16_t rkey)
{
    uint16_t f_k;
    f_k = input_block ^ rkey;
    f_k = s_block(f_k);
    f_k = p_block(f_k);
    return f_k;
}

uint32_t feistel(uint32_t plaintext_block, uint16_t round_key)
{
    uint16_t l = static_cast<uint16_t>(plaintext_block >> 16);
    uint16_t r = static_cast<uint16_t>(plaintext_block & 0xFFFF);
    uint16_t x = round_function(r, round_key);

    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(l ^ x));
}

uint32_t lay_messi(uint32_t plaintext_block, uint16_t round_key)
{
    uint16_t l = static_cast<uint16_t>(plaintext_block >> 16);
    uint16_t r = static_cast<uint16_t>(plaintext_block & 0xFFFF);
    uint16_t x = round_function(l ^ r, round_key);

    return (static_cast<uint32_t>(l ^ x) << 16) | (static_cast<uint32_t>(r ^ x));
}

uint32_t sp_network_32(uint32_t plaintext_block, uint32_t round_key)
{
    uint32_t state = plaintext_block ^ round_key;

    uint32_t result = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t part = (state >> (4 * (7 - i))) & 0x0F;
        uint8_t sbox_output = s_block_znach[part];
        result |= (static_cast<uint32_t>(sbox_output) << (4 * (7 - i)));
    }

    const int A = 29;
    const int B = 12;
    const int m = 32;
    uint32_t p_result = 0;

    for (int i = 0; i < 32; i++) {
        int new_pos = (A * i + B) % m;
        if (result & (1U << i)) {
            p_result |= (1U << new_pos);
        }
    }

    return p_result;
}

uint32_t tau_permutation(uint32_t input_block)
{
    uint16_t l = static_cast<uint16_t>(input_block >> 16);
    uint16_t r = static_cast<uint16_t>(input_block & 0xFFFF);

    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(l));
}

uint32_t block_encryption(uint32_t key, uint32_t plain_block)
{
    uint32_t cipher_block = plain_block;

    uint16_t k1 = static_cast<uint16_t>(key >> 16);
    uint16_t k2 = static_cast<uint16_t>(key & 0xFFFF);

    uint16_t round_key1 = k2;

    uint16_t k2_shifted = (k2 << 8) | (k2 >> 8); 
    uint16_t k1_shifted = (k1 << 12) | (k1 >> 4); 
    uint32_t round_key2 = (static_cast<uint32_t>(k2_shifted) << 16) | k1_shifted;

    uint16_t round_key3 = k1;
    uint16_t round_key4 = k1 ^ (k2 ^ 5993);

    cipher_block = lay_messi(cipher_block, round_key1); // l
    cipher_block = sp_network_32(cipher_block, round_key2); // s
    cipher_block = tau_permutation(cipher_block); // t
    cipher_block = feistel(cipher_block, round_key3); // f
    cipher_block = tau_permutation(cipher_block); // t
    cipher_block = feistel(cipher_block, round_key4); // f

    return cipher_block;
}

vector<uint32_t> encrypt(uint32_t key, vector<uint32_t> &plaintext, size_t fileSize)
{
    size_t len = (fileSize + 3) / 4;
    vector<uint32_t> cipher_text(len);

    for (size_t i = 0; i < len; i++) {
        cipher_text[i] = block_encryption(key, plaintext[i]);
    }

    if (fileSize % 4 != 0) {
        size_t bitsToComplete = 32 - ((fileSize % 4) * 8);
        uint32_t paddingMarker = 1U << (bitsToComplete - 1);
        plaintext[len - 1] = plaintext[len - 1] | paddingMarker;
        cipher_text[len - 1] = block_encryption(key, plaintext[len - 1]);
    } else {
        uint32_t additionalBlock = 0x80000000;
        cipher_text[len - 1] = block_encryption(key, plaintext[len - 1]);
        cipher_text.push_back(block_encryption(key, additionalBlock));
    }
    return cipher_text;
}

int main()
{
    const char *filename = "key10.in";
    ifstream inputFile(filename, ios::binary);
    uint32_t key;
    inputFile.read(reinterpret_cast<char *>(&key), sizeof(key));
    inputFile.close();
    key = (key >> 24) | ((key >> 8) & 0x0000FF00) | ((key << 8) & 0x00FF0000) | (key << 24);
    cout << "Key: " << bitset<32>(key) << endl;

    filename = "test10.in";
    ifstream File(filename, ios::binary);

    File.seekg(0, ios::end);
    size_t fileSize = File.tellg();
    size_t len = (fileSize + 3) / 4;
    File.seekg(0, ios::beg);
    vector<uint32_t> plaintext(len);
    File.read(reinterpret_cast<char *>(plaintext.data()), fileSize);
    File.close();
    for (size_t i = 0; i < len; ++i) {
        plaintext[i] = (plaintext[i] >> 24) | ((plaintext[i] >> 8) & 0x0000FF00)
                | ((plaintext[i] << 8) & 0x00FF0000) | (plaintext[i] << 24);
    }

    cout << "Plaintext:" << endl;
    for (size_t i = 0; i < min(static_cast<size_t>(2), plaintext.size()); ++i) {
        cout << bitset<32>(plaintext[i]) << endl;
    }

    vector<uint32_t> cipher_text = encrypt(key, plaintext, fileSize);

    const char *outputFilename = "test10.out";
    ofstream outputFile(outputFilename, ios::binary);
    if (outputFile.is_open()) {
        for (size_t i = 0; i < cipher_text.size(); ++i) {
            uint32_t encryptedWord = (cipher_text[i] >> 24) | ((cipher_text[i] >> 8) & 0x0000FF00)
                    | ((cipher_text[i] << 8) & 0x00FF0000) | (cipher_text[i] << 24);
            outputFile.write(reinterpret_cast<const char *>(&encryptedWord), sizeof(encryptedWord));
        }
        outputFile.close();
    } else {
        cerr << "Error: Could not open output file " << outputFilename << endl;
    }

    cout << "Cipher_text:" << endl;
    for (size_t i = 0; i < min(static_cast<size_t>(2), cipher_text.size()); ++i) {
        cout << bitset<32>(cipher_text[i]) << endl;
    }

    return 0;
}