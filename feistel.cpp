#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

unsigned int round_function(unsigned int right, unsigned int key) {
  unsigned int value = (right + key) & 0xFFFF;
  return ((value << 3) | (value >> 13)) & 0xFFFF;
}

unsigned int feistel_cipher(unsigned int block,
                            vector<unsigned int> keys, bool decrypt) {
  unsigned int left = block >> 16;
  unsigned int right = block & 0xFFFF;
  for (int i = 0; i < (int)keys.size(); i++) {
    int index = decrypt ? (int)keys.size() - 1 - i : i;
    unsigned int next = left ^ round_function(right, keys[index]);
    left = right;
    right = next;
  }
  return (right << 16) | left;
}

int main() {
  unsigned long long block;
  vector<unsigned int> keys(4);
  cout << "Enter a 32-bit block in hex: ";
  if (!(cin >> hex >> block) || block > 0xFFFFFFFFULL) {
    cout << "Invalid 32-bit block.\n";
    return 1;
  }
  cout << "Enter four 16-bit round keys in hex: ";
  for (unsigned int &key : keys) {
    if (!(cin >> hex >> key) || key > 0xFFFF) {
      cout << "Invalid 16-bit round key.\n";
      return 1;
    }
  }
  unsigned int encrypted = feistel_cipher(block, keys, false);
  cout << hex << uppercase << setfill('0');
  cout << "Encrypted: " << setw(8) << encrypted << endl;
  cout << "Decrypted: " << setw(8)
       << feistel_cipher(encrypted, keys, true) << endl;
  return 0;
}
