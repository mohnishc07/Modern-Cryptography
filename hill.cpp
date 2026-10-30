#include <iostream>
#include <string>
#include <vector>
using namespace std;

int mod26(int number) {
  return (number % 26 + 26) % 26;
}

string hill_cipher(string message, vector<vector<int>> key) {
  string result;
  for (int i = 0; i < (int)message.size(); i += 2) {
    int first = message[i] - 'A';
    int second = message[i + 1] - 'A';
    result += 'A' + mod26(key[0][0] * first + key[0][1] * second);
    result += 'A' + mod26(key[1][0] * first + key[1][1] * second);
  }
  return result;
}

int main() {
  string input, message;
  vector<vector<int>> key(2, vector<int>(2));
  cout << "Enter the message: ";
  getline(cin, input);
  for (char letter : input) {
    if (letter >= 'a' && letter <= 'z') letter -= 'a' - 'A';
    if (letter >= 'A' && letter <= 'Z') message += letter;
  }
  cout << "Enter the 2 x 2 key matrix (row-wise): ";
  for (auto &row : key) {
    for (int &value : row) {
      if (!(cin >> value)) return 1;
      value = mod26(value);
    }
  }
  int determinant = mod26(key[0][0] * key[1][1] -
                          key[0][1] * key[1][0]);
  int inverse = -1;
  for (int i = 1; i < 26; i++) {
    if (determinant * i % 26 == 1) inverse = i;
  }
  if (inverse == -1) {
    cout << "The key matrix is not invertible modulo 26.\n";
    return 1;
  }
  vector<vector<int>> inverseKey = {
      {mod26(inverse * key[1][1]), mod26(-inverse * key[0][1])},
      {mod26(-inverse * key[1][0]), mod26(inverse * key[0][0])}};
  bool padded = message.size() % 2 == 1;
  if (padded) message += 'X';
  string encrypted = hill_cipher(message, key);
  string decrypted = hill_cipher(encrypted, inverseKey);
  if (padded) decrypted.pop_back();
  cout << "Prepared message: " << message << endl;
  cout << "Encrypted: " << encrypted << endl;
  cout << "Decrypted: " << decrypted << endl;
  return 0;
}
