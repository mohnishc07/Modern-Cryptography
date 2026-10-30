#include <iostream>
#include <numeric>
using namespace std;

long long modular_power(long long base, long long exponent,
                        long long modulus) {
  if (exponent == 0) return 1 % modulus;
  long long half = modular_power(base, exponent / 2, modulus);
  long long result = half * half % modulus;
  if (exponent % 2 == 1) result = result * base % modulus;
  return result;
}

long long extended_gcd(long long a, long long b,
                       long long &x, long long &y) {
  if (b == 0) {
    x = 1;
    y = 0;
    return a;
  }
  long long nextX, nextY;
  long long result = extended_gcd(b, a % b, nextX, nextY);
  x = nextY;
  y = nextX - (a / b) * nextY;
  return result;
}

int main() {
  long long base, exponent, modulus;
  cout << "Enter base, exponent and modulus: ";
  if (!(cin >> base >> exponent >> modulus) || base < 0 ||
      exponent < 0 || modulus < 2 || modulus > 1000000000) {
    cout << "Use base >= 0, exponent >= 0 and modulus 2..1000000000.\n";
    return 1;
  }
  base %= modulus;
  cout << "GCD: " << gcd(base, modulus) << endl;
  cout << "Modular power: "
       << modular_power(base, exponent, modulus) << endl;
  long long x, y;
  if (extended_gcd(base, modulus, x, y) == 1) {
    cout << "Modular inverse: " << (x % modulus + modulus) % modulus
         << endl;
  } else {
    cout << "Modular inverse does not exist.\n";
  }
  return 0;
}
