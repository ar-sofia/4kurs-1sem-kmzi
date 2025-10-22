#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <bitset>

using namespace std;

uint8_t s_block_znach[16] = { 2, 10, 3, 4, 5, 11, 14, 9, 6, 12, 13, 15, 1, 0, 8, 7 };

uint16_t s_block(uint16_t input_block)
{
    uint8_t parts[4];
    for (int i = 0; i < 4; ++i) {
        parts[i] = (input_block >> (4 * (3 - i))) & 0x0F;
    }

    uint16_t result = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t sbox_output = s_block_znach[parts[i]];
        result |= (sbox_output << (4 * (3 - i)));
    }
    return result;
}

uint16_t p_block(uint16_t input_block)
{
    const int A = 23;
    const int B = 6;
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
    uint16_t f_k = input_block ^ rkey;
    f_k = s_block(f_k);
    f_k = p_block(f_k);
    return f_k;
}

uint32_t feistel(uint32_t plaintext_block, uint16_t round_key)
{
    uint16_t left = static_cast<uint16_t>(plaintext_block >> 16);
    uint16_t right = static_cast<uint16_t>(plaintext_block & 0xFFFF);
    uint16_t f_result = round_function(right, round_key);
    
    uint16_t new_left = right;
    uint16_t new_right = left ^ f_result;
    
    return (static_cast<uint32_t>(new_left) << 16) | new_right;
}

uint32_t tau_permutation(uint32_t input_block)
{
    uint16_t left = static_cast<uint16_t>(input_block >> 16);
    uint16_t right = static_cast<uint16_t>(input_block & 0xFFFF);
    return (static_cast<uint32_t>(right) << 16) | left;
}

uint32_t block_encryption(uint32_t key, uint32_t plain_block)
{
    uint32_t state = plain_block;

    uint16_t k1 = static_cast<uint16_t>(key >> 16);
    uint16_t k2 = static_cast<uint16_t>(key & 0xFFFF);

    // Генерация раундовых ключей
    uint16_t round_key1 = k2 ^ 8755;
    
    // k1<1 - циклический сдвиг влево на 1 бит
    uint16_t round_key2 = (k1 << 1) | (k1 >> 15);
    
    // (k1<3)|(k2^19914)
    uint16_t k1_shifted = (k1 << 3) | (k1 >> 13);
    uint16_t k2_xored = k2 ^ 19914;
    uint16_t round_key3 = k1_shifted | k2_xored;
    
    uint16_t round_key4 = k2;
    
    // k1^29867,k1 - 32-битный ключ для отбеливания
    uint32_t round_key5 = (static_cast<uint32_t>(k1 ^ 29867) << 16) | k1;

    // Последовательность раундов: tftfffw
    state = tau_permutation(state);        // t
    state = feistel(state, round_key1);    // f
    state = tau_permutation(state);        // t
    state = feistel(state, round_key2);    // f
    state = feistel(state, round_key3);    // f
    state = feistel(state, round_key4);    // f
    state = state ^ round_key5;            // w

    return state;
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
    key = (key >> 24) | ((key >> 8) & 0x0000FF00) | ((key << 8) & 0x00FF0000) | (key << 24);
    
    cout << "Key: " << hex << key << " (" << bitset<32>(key) << ")" << endl;

    // Чтение входных данных - БЕЗ перестановки байтов
    filename = "example3.in";
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
    
    for (size_t i = 0; i < len; ++i) {
        plaintext[i] = (plaintext[i] >> 24) | ((plaintext[i] >> 8) & 0x0000FF00)
                | ((plaintext[i] << 8) & 0x00FF0000) | (plaintext[i] << 24);
    }
    
    cout << "Plaintext (" << fileSize << " bytes):" << endl;
    for (size_t i = 0; i < plaintext.size(); ++i) {
        cout << "Block " << i << ": " << hex << plaintext[i] << " (" << bitset<32>(plaintext[i]) << ")" << endl;
    }

    vector<uint32_t> cipher_text = encrypt(key, plaintext, fileSize);

    // Запись результата с перестановкой байтов
    const char *outputFilename = "example33.out";
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