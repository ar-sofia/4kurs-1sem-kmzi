#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <bitset>

using namespace std;

uint8_t s_block_znach[16] = { 2, 10, 3, 4, 5, 11, 14, 9, 6, 12, 13, 15, 1, 0, 8, 7 };

// S-блок для 32-битного блока
uint32_t s_block_32(uint32_t input_block)
{
    uint32_t result = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t nibble = (input_block >> (4 * (7 - i))) & 0x0F;
        uint8_t sbox_output = s_block_znach[nibble];
        result |= (static_cast<uint32_t>(sbox_output) << (4 * (7 - i)));
    }
    return result;
}

// P-блок для 32-битного блока
uint32_t p_block_32(uint32_t input_block)
{
    const int A = 23;
    const int B = 6;
    const int m = 32;
    uint32_t result = 0;

    for (int i = 0; i < 32; i++) {
        int new_pos = (A * i + B) % m;
        if (input_block & (1U << i)) {
            result |= (1U << new_pos);
        }
    }
    return result;
}

uint32_t sp_network_round(uint32_t input_block, uint32_t round_key)
{
    // XOR с раундовым ключом
    uint32_t state = input_block ^ round_key;
    
    // Применение S-блока к 32-битному блоку
    state = s_block_32(state);
    
    // Применение P-блока к 32-битному блоку
    state = p_block_32(state);
    
    return state;
}

uint32_t block_encryption(uint32_t key, uint32_t plain_block)
{
    // Один раунд SP-сети с исходным ключом как раундовым ключом
    return sp_network_round(plain_block, key);
}

vector<uint32_t> encrypt(uint32_t key, vector<uint32_t> &plaintext, size_t fileSize)
{
    size_t len = (fileSize + 3) / 4;
    vector<uint32_t> cipher_text;

    // Шифруем все полные блоки
    for (size_t i = 0; i < len; i++) {
        cipher_text.push_back(block_encryption(key, plaintext[i]));
    }
    
    // Дополнение типа 1: добавляем блок с длиной последнего блока
    uint32_t length_block = fileSize % 4;
    if (length_block == 0 && fileSize > 0) {
        length_block = 4; // Если блок был полный, длина = 4
    }
    cipher_text.push_back(block_encryption(key, length_block));

    return cipher_text;
}

int main()
{
    // Чтение ключа - БЕЗ перестановки байтов
    const char *filename = "example_key.in";
    ifstream inputFile(filename, ios::binary);
    if (!inputFile) {
        cerr << "Error: Could not open key file " << filename << endl;
        return 1;
    }
    
    uint32_t key;
    inputFile.read(reinterpret_cast<char *>(&key), sizeof(key));
    inputFile.close();
    
    // Преобразование ключа из little-endian в big-endian
    key = (key >> 24) | ((key >> 8) & 0x0000FF00) | ((key << 8) & 0x00FF0000) | (key << 24);
    
    cout << "Key: " << hex << key << " (" << bitset<32>(key) << ")" << endl;

    // Чтение входных данных - БЕЗ перестановки байтов
    filename = "example1.in";
    ifstream File(filename, ios::binary);
    if (!File) {
        cerr << "Error: Could not open input file " << filename << endl;
        return 1;
    }

    File.seekg(0, ios::end);
    size_t fileSize = File.tellg();
    size_t len = (fileSize + 3) / 4;
    File.seekg(0, ios::beg);
    
    vector<uint32_t> plaintext(len);
    File.read(reinterpret_cast<char *>(plaintext.data()), fileSize);
    File.close();
    
    // Преобразование входных данных из little-endian в big-endian
    for (size_t i = 0; i < len; ++i) {
        plaintext[i] = (plaintext[i] >> 24) | ((plaintext[i] >> 8) & 0x0000FF00)
                | ((plaintext[i] << 8) & 0x00FF0000) | (plaintext[i] << 24);
    }
    
    cout << "Plaintext (" << fileSize << " bytes):" << endl;
    for (size_t i = 0; i < plaintext.size(); ++i) {
        cout << "Block " << i << ": " << hex << plaintext[i] << " (" << bitset<32>(plaintext[i]) << ")" << endl;
    }

    vector<uint32_t> cipher_text = encrypt(key, plaintext, fileSize);

    // Запись результата с преобразованием обратно в little-endian
    const char *outputFilename = "example11_deepseek.out";
    ofstream outputFile(outputFilename, ios::binary);
    if (outputFile.is_open()) {
        for (size_t i = 0; i < cipher_text.size(); ++i) {
            uint32_t encryptedWord = (cipher_text[i] >> 24) | ((cipher_text[i] >> 8) & 0x0000FF00)
                    | ((cipher_text[i] << 8) & 0x00FF0000) | (cipher_text[i] << 24);
            outputFile.write(reinterpret_cast<const char *>(&encryptedWord), sizeof(encryptedWord));
        }
        outputFile.close();
        cout << "Output written to " << outputFilename << endl;
    } else {
        cerr << "Error: Could not open output file " << outputFilename << endl;
    }

    cout << "Cipher_text:" << endl;
    for (size_t i = 0; i < cipher_text.size(); ++i) {
        cout << "Block " << i << ": " << hex << cipher_text[i] << " (" << bitset<32>(cipher_text[i]) << ")" << endl;
    }

    return 0;
}