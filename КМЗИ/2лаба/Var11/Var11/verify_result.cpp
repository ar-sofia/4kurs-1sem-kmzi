#include <iostream>
#include <iomanip>

using namespace std;

int main() {
    // Ожидаемые результаты (в hex)
    uint32_t expected[] = {0x454f1eed, 0x035d0729, 0x817e0ea9};
    
    // Hex содержимое файла
    unsigned char file_content[] = {0x0E, 0x4F, 0x1E, 0xED, 0x03, 0x5D, 0x07, 0x29, 0x81, 0x7E, 0x0E, 0xA9};
    
    cout << "Expected results:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "Block " << i << ": " << hex << expected[i] << endl;
    }
    cout << endl;
    
    cout << "File content (hex):" << endl;
    for (int i = 0; i < 12; i++) {
        cout << hex << setfill('0') << setw(2) << (unsigned int)file_content[i] << " ";
        if ((i + 1) % 4 == 0) cout << endl;
    }
    cout << endl;
    
    // Проверяем соответствие
    cout << "Checking correspondence:" << endl;
    for (int i = 0; i < 3; i++) {
        uint32_t file_block = (file_content[i*4+3] << 24) | (file_content[i*4+2] << 16) | 
                             (file_content[i*4+1] << 8) | file_content[i*4];
        cout << "Block " << i << ": Expected " << hex << expected[i] 
             << ", File " << file_block << " - " 
             << (expected[i] == file_block ? "MATCH" : "MISMATCH") << endl;
    }
    
    return 0;
}
