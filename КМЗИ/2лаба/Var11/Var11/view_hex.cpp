#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int main() {
    ifstream file("example33.out", ios::binary);
    if (!file) {
        cout << "Cannot open file" << endl;
        return 1;
    }
    
    cout << "example33.out hex content:" << endl;
    char byte;
    int count = 0;
    while (file.read(&byte, 1)) {
        cout << hex << setfill('0') << setw(2) << (unsigned char)byte << " ";
        count++;
        if (count % 8 == 0) cout << endl;
    }
    cout << endl << "Total bytes: " << dec << count << endl;
    return 0;
}
