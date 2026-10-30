#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

int multiply(int a, int b) {
  int result = 0;
  while (b > 0) {
    if (b & 1) result ^= a;
    a <<= 1;
    if (a & 0x100) a ^= 0x11B;
    b >>= 1;
  }
  return result;
}

int field_power(int base, int exponent) {
  int result = 1;
  while (exponent > 0) {
    if (exponent & 1) result = multiply(result, base);
    base = multiply(base, base);
    exponent >>= 1;
  }
  return result;
}

int rotate_byte(int value, int amount) {
  return ((value << amount) | (value >> (8 - amount))) & 255;
}

void generate_sboxes(vector<int> &sbox, vector<int> &inverse) {
  for (int i = 0; i < 256; i++) {
    int value = i == 0 ? 0 : field_power(i, 254);
    int substituted = value ^ rotate_byte(value, 1) ^
                      rotate_byte(value, 2) ^ rotate_byte(value, 3) ^
                      rotate_byte(value, 4) ^ 0x63;
    sbox[i] = substituted;
    inverse[substituted] = i;
  }
}

vector<int> expand_key(vector<int> key, const vector<int> &sbox) {
  int rcon = 1;
  while (key.size() < 176) {
    int size = key.size();
    vector<int> temp(key.end() - 4, key.end());
    if (size % 16 == 0) {
      int first = temp[0];
      for (int i = 0; i < 3; i++) temp[i] = sbox[temp[i + 1]];
      temp[3] = sbox[first];
      temp[0] ^= rcon;
      rcon = multiply(rcon, 2);
    }
    for (int i = 0; i < 4; i++) key.push_back(key[size - 16 + i] ^ temp[i]);
  }
  return key;
}

void add_round_key(vector<int> &state, const vector<int> &keys, int round) {
  for (int i = 0; i < 16; i++) state[i] ^= keys[16 * round + i];
}

void substitute(vector<int> &state, const vector<int> &table) {
  for (int &value : state) value = table[value];
}

void shift_rows(vector<int> &state, bool inverse) {
  vector<int> previous = state;
  for (int row = 0; row < 4; row++) {
    for (int column = 0; column < 4; column++) {
      int source = (column + (inverse ? 4 - row : row)) % 4;
      state[4 * column + row] = previous[4 * source + row];
    }
  }
}

void mix_columns(vector<int> &state, bool inverse) {
  int forward[4][4] = {{2,3,1,1}, {1,2,3,1}, {1,1,2,3}, {3,1,1,2}};
  int backward[4][4] = {
    {14,11,13,9}, {9,14,11,13}, {13,9,14,11}, {11,13,9,14}
  };
  vector<int> previous = state;
  for (int column = 0; column < 4; column++) {
    for (int row = 0; row < 4; row++) {
      int result = 0;
      for (int i = 0; i < 4; i++) {
        int factor = inverse ? backward[row][i] : forward[row][i];
        result ^= multiply(factor, previous[4 * column + i]);
      }
      state[4 * column + row] = result;
    }
  }
}

vector<int> aes_cipher(vector<int> state, const vector<int> &keys,
                       const vector<int> &sbox, const vector<int> &inverse,
                       bool decrypt) {
  if (!decrypt) {
    add_round_key(state, keys, 0);
    for (int round = 1; round <= 10; round++) {
      substitute(state, sbox);
      shift_rows(state, false);
      if (round != 10) mix_columns(state, false);
      add_round_key(state, keys, round);
    }
  } else {
    add_round_key(state, keys, 10);
    for (int round = 9; round >= 0; round--) {
      shift_rows(state, true);
      substitute(state, inverse);
      add_round_key(state, keys, round);
      if (round != 0) mix_columns(state, true);
    }
  }
  return state;
}

bool read_hex(string prompt, vector<int> &bytes) {
  string input;
  cout << prompt;
  if (!(cin >> input) || input.size() != 32) return false;
  bytes.clear();
  for (int i = 0; i < 32; i += 2) {
    int value = 0;
    for (int j = 0; j < 2; j++) {
      char digit = input[i + j];
      int number;
      if (digit >= '0' && digit <= '9') number = digit - '0';
      else if (digit >= 'a' && digit <= 'f') number = digit - 'a' + 10;
      else if (digit >= 'A' && digit <= 'F') number = digit - 'A' + 10;
      else return false;
      value = value * 16 + number;
    }
    bytes.push_back(value);
  }
  return true;
}

string to_hex(const vector<int> &bytes) {
  ostringstream output;
  output << hex << uppercase << setfill('0');
  for (int value : bytes) output << setw(2) << value;
  return output.str();
}

int main() {
  vector<int> block, key, sbox(256), inverse(256);
  if (!read_hex("Enter the plaintext (32 hex digits): ", block) ||
      !read_hex("Enter the key (32 hex digits): ", key)) {
    cout << "Each input must contain exactly 32 hex digits.\n";
    return 1;
  }
  generate_sboxes(sbox, inverse);
  vector<int> keys = expand_key(key, sbox);
  vector<int> encrypted = aes_cipher(block, keys, sbox, inverse, false);
  cout << "Encrypted: " << to_hex(encrypted) << endl;
  cout << "Decrypted: "
       << to_hex(aes_cipher(encrypted, keys, sbox, inverse, true)) << endl;
  return 0;
}
