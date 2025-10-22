#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

void printHexFile(const char* filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "Cannot open file: " << filename << endl;
        return;
    }
    
    cout << "File: " << filename << endl;
    char byte;
    int count = 0;
    while (file.read(&byte, 1)) {
        if (count % 16 == 0) {
            cout << endl << setfill('0') << setw(8) << hex << count << ": ";
        }
        cout << setfill('0') << setw(2) << hex << (unsigned char)byte << " ";
        count++;
    }
    cout << endl << "Total bytes: " << dec << count << endl << endl;
}

int main() {
    printHexFile("example2.in");
    printHexFile("example2.out");
    printHexFile("example22.out");
    printHexFile("example_key.in");
    return 0;
}
