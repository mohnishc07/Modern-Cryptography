#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

int hex_value(char digit) {
  if (digit >= '0' && digit <= '9') return digit - '0';
  if (digit >= 'a' && digit <= 'f') return digit - 'a' + 10;
  if (digit >= 'A' && digit <= 'F') return digit - 'A' + 10;
  return -1;
}

int main() {
  string message, keyHex, key, encrypted, decrypted;
  cout << "Enter the message: ";
  getline(cin, message);
  cout << "Enter the one-time key in hex (2 digits per byte): ";
  getline(cin, keyHex);
  if (message.empty() || keyHex.size() != 2 * message.size()) {
    cout << "Use a nonempty message and one key byte per message byte.\n";
    return 1;
  }
  for (int i = 0; i < (int)keyHex.size(); i += 2) {
    int high = hex_value(keyHex[i]);
    int low = hex_value(keyHex[i + 1]);
    if (high == -1 || low == -1) {
      cout << "Invalid hexadecimal key.\n";
      return 1;
    }
    key += char(high * 16 + low);
  }
  ostringstream output;
  output << hex << uppercase << setfill('0');
  for (int i = 0; i < (int)message.size(); i++) {
    unsigned char value = (unsigned char)message[i] ^
                          (unsigned char)key[i];
    encrypted += char(value);
    output << setw(2) << (int)value;
  }
  for (int i = 0; i < (int)encrypted.size(); i++) {
    decrypted += char((unsigned char)encrypted[i] ^
                      (unsigned char)key[i]);
  }
  cout << "Ciphertext (hex): " << output.str() << endl;
  cout << "Decrypted: " << decrypted << endl;
  return 0;
}
