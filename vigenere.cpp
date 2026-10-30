#include <iostream>
#include <string>
using namespace std;

string vigenere_cipher(string message, string key, bool decrypt) {
  int position = 0;
  for (char &letter : message) {
    char start;
    if (letter >= 'A' && letter <= 'Z') start = 'A';
    else if (letter >= 'a' && letter <= 'z') start = 'a';
    else continue;
    int shift = key[position % key.size()] - 'A';
    if (decrypt) shift = -shift;
    letter = start + (letter - start + shift + 26) % 26;
    position++;
  }
  return message;
}

int main() {
  string message, key;
  cout << "Enter the message: ";
  getline(cin, message);
  cout << "Enter the key: ";
  getline(cin, key);
  if (key.empty()) {
    cout << "The key cannot be empty.\n";
    return 1;
  }
  for (char &letter : key) {
    if (letter >= 'a' && letter <= 'z') letter -= 'a' - 'A';
    if (letter < 'A' || letter > 'Z') {
      cout << "The key must contain only letters.\n";
      return 1;
    }
  }
  string encrypted = vigenere_cipher(message, key, false);
  cout << "Encrypted: " << encrypted << endl;
  cout << "Decrypted: " << vigenere_cipher(encrypted, key, true)
       << endl;
  return 0;
}
