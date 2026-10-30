#include <iostream>
#include <string>
using namespace std;

string caesar_cipher(string message, int shift) {
  shift = (shift % 26 + 26) % 26;
  for (char &letter : message) {
    if (letter >= 'A' && letter <= 'Z') {
      letter = 'A' + (letter - 'A' + shift) % 26;
    } else if (letter >= 'a' && letter <= 'z') {
      letter = 'a' + (letter - 'a' + shift) % 26;
    }
  }
  return message;
}

int main() {
  string message;
  int shift;
  cout << "Enter the message: ";
  getline(cin, message);
  cout << "Enter the shift: ";
  if (!(cin >> shift)) return 1;
  shift = (shift % 26 + 26) % 26;
  string encrypted = caesar_cipher(message, shift);
  cout << "Encrypted: " << encrypted << endl;
  cout << "Decrypted: " << caesar_cipher(encrypted, -shift) << endl;
  return 0;
}
