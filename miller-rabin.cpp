#include <iostream>
#include <random>
using namespace std;

long long generate_random_test_number(long long limit) {
  static random_device rd;
  static mt19937_64 gen(rd());
  uniform_int_distribution<long long> distribution(2, limit);
  return distribution(gen);
}

long long multiply_mod(long long a, long long b, long long modulus) {
  return (unsigned __int128)a * b % modulus;
}

long long fermats_primality_test(long long exponent, long long base,
                                 long long prime) {
  if (exponent == 0) return 1;
  if (exponent % 2 == 0) {
    return fermats_primality_test(exponent / 2,
                                 multiply_mod(base, base, prime), prime);
  }
  return multiply_mod(base,
                       fermats_primality_test(exponent - 1, base, prime),
                       prime);
}

bool test_sequence(long long prime, long long base) {
  long long d = prime - 1;
  int s = 0;
  while (d % 2 == 0) {
    d /= 2;
    s++;
  }
  long long result = fermats_primality_test(d, base, prime);
  if (result == 1 || result == prime - 1) return true;
  for (int i = 1; i < s; i++) {
    result = multiply_mod(result, result, prime);
    if (result == prime - 1) return true;
    if (result == 1) return false;
  }
  return false;
}

int main() {
  cout << "Enter the prime number you want to do this test for: ";
  long long input;
  if (!(cin >> input)) return 1;
  if (input < 2) {
    cout << input << " is not prime" << endl;
    return 0;
  }
  if (input == 2 || input == 3) {
    cout << input << " is prime" << endl;
    return 0;
  }
  if (input % 2 == 0) {
    cout << input << " is composite (even)" << endl;
    return 0;
  }
  cout << "Enter the number of rounds you want to perform: ";
  int rounds;
  if (!(cin >> rounds) || rounds < 1) {
    cout << "The number of rounds must be positive.\n";
    return 1;
  }
  for (int i = 0; i < rounds; i++) {
    long long test = generate_random_test_number(input - 2);
    if (!test_sequence(input, test)) {
      cout << input << " is composite (witness: " << test
           << ", round " << i + 1 << ")" << endl;
      return 0;
    }
  }
  cout << input << " is probably prime after " << rounds << " rounds"
       << endl;
  return 0;
}
