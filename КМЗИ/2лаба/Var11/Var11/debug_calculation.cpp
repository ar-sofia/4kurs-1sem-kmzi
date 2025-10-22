#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <bitset>

using namespace std;

uint8_t s_block_znach[16] = { 2, 10, 3, 4, 5, 11, 14, 9, 6, 12, 13, 15, 1, 0, 8, 7 };

uint16_t s_block_debug(uint16_t input_block)
{
    cout << "  S-block input: " << hex << input_block << " (" << bitset<16>(input_block) << ")" << endl;
    
    uint8_t parts[4];
    for (int i = 0; i < 4; ++i) {
        parts[i] = (input_block >> (4 * (3 - i))) & 0x0F;
        cout << "    Part " << i << ": " << hex << (int)parts[i] << " (from bits " << (4 * (3 - i)) << "-" << (4 * (3 - i) + 3) << ")" << endl;
    }

    uint16_t result = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t sbox_output = s_block_znach[parts[i]];
        cout << "    S-box[" << hex << (int)parts[i] << "] = " << hex << (int)sbox_output << endl;
        result |= (sbox_output << (4 * (3 - i)));
    }
    
    cout << "  S-block output: " << hex << result << " (" << bitset<16>(result) << ")" << endl;
    return result;
}

uint32_t sp_network_round_debug(uint32_t input_block, uint32_t round_key)
{
    cout << "\n=== SP-Network Round ===" << endl;
    cout << "Input block: " << hex << input_block << " (" << bitset<32>(input_block) << ")" << endl;
    cout << "Round key:   " << hex << round_key << " (" << bitset<32>(round_key) << ")" << endl;
    
    uint32_t state = input_block;
    
    // 1. XOR с раундовым ключом
    state = state ^ round_key;
    cout << "After XOR:   " << hex << state << " (" << bitset<32>(state) << ")" << endl;
    
    // 2. S-блок для 32-битного блока (применяем к каждому 16-битному полублоку)
    uint16_t left = static_cast<uint16_t>(state >> 16);
    uint16_t right = static_cast<uint16_t>(state & 0xFFFF);
    
    cout << "Left half:   " << hex << left << " (" << bitset<16>(left) << ")" << endl;
    cout << "Right half:  " << hex << right << " (" << bitset<16>(right) << ")" << endl;
    
    cout << "\nProcessing left half:" << endl;
    left = s_block_debug(left);
    cout << "\nProcessing right half:" << endl;
    right = s_block_debug(right);
    
    state = (static_cast<uint32_t>(left) << 16) | right;
    cout << "\nAfter S-box: " << hex << state << " (" << bitset<32>(state) << ")" << endl;
    
    // 3. P-блок для 32-битного блока
    cout << "\nP-block permutation:" << endl;
    const int A = 23;
    const int B = 6;
    const int m = 32;
    uint32_t result = 0;

    for (int i = 0; i < 32; i++) {
        int new_pos = (A * i + B) % m;
        if (state & (1U << i)) {
            result |= (1U << new_pos);
            cout << "  Bit " << i << " -> " << new_pos << endl;
        }
    }
    
    cout << "After P-box: " << hex << result << " (" << bitset<32>(result) << ")" << endl;
    
    return result;
}

int main()
{
    // Читаем ключ
    ifstream keyFile("example_key.in", ios::binary);
    uint32_t key;
    keyFile.read(reinterpret_cast<char *>(&key), sizeof(key));
    keyFile.close();
    key = (key >> 24) | ((key >> 8) & 0x0000FF00) | ((key << 8) & 0x00FF0000) | (key << 24);
    
    cout << "Key (after byte swap): " << hex << key << " (" << bitset<32>(key) << ")" << endl;
    
    // Читаем входные данные
    ifstream inputFile("example3.in", ios::binary);
    inputFile.seekg(0, ios::end);
    size_t fileSize = inputFile.tellg();
    size_t len = (fileSize + 3) / 4;
    inputFile.seekg(0, ios::beg);
    
    vector<uint32_t> plaintext(len);
    inputFile.read(reinterpret_cast<char *>(plaintext.data()), fileSize);
    inputFile.close();
    
    for (size_t i = 0; i < len; ++i) {
        plaintext[i] = (plaintext[i] >> 24) | ((plaintext[i] >> 8) & 0x0000FF00)
                | ((plaintext[i] << 8) & 0x00FF0000) | (plaintext[i] << 24);
    }
    
    cout << "\nInput data (" << fileSize << " bytes):" << endl;
    for (size_t i = 0; i < plaintext.size(); ++i) {
        cout << "Block " << i << ": " << hex << plaintext[i] << " (" << bitset<32>(plaintext[i]) << ")" << endl;
    }
    
    // Вычисляем SP-сеть для каждого блока
    vector<uint32_t> cipher_text;
    for (size_t i = 0; i < len; i++) {
        cout << "\n" << string(50, '=') << endl;
        cout << "BLOCK " << i << endl;
        cout << string(50, '=') << endl;
        cipher_text.push_back(sp_network_round_debug(plaintext[i], key));
    }
    
    cout << "\n" << string(50, '=') << endl;
    cout << "FINAL RESULTS" << endl;
    cout << string(50, '=') << endl;
    for (size_t i = 0; i < cipher_text.size(); ++i) {
        cout << "Block " << i << ": " << hex << cipher_text[i] << endl;
        
        // Выводим в формате байтов как в файле
        uint32_t bytes = cipher_text[i];
        cout << "As bytes: " << hex << (bytes & 0xFF) << " " << ((bytes >> 8) & 0xFF) << " " 
             << ((bytes >> 16) & 0xFF) << " " << ((bytes >> 24) & 0xFF) << endl;
    }
    
    return 0;
}
