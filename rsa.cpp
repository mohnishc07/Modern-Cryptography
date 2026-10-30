#include <iostream>
#include <numeric>
using namespace std;

bool is_prime(long long number) {
  if (number < 2) return false;
  for (long long i = 2; i * i <= number; i++) {
    if (number % i == 0) return false;
  }
  return true;
}

long long modular_power(long long base, long long exponent,
                        long long modulus) {
  if (exponent == 0) return 1 % modulus;
  if (exponent % 2 == 0) {
    long long square = (unsigned __int128)base * base % modulus;
    return modular_power(square, exponent / 2, modulus);
  }
  return (unsigned __int128)base *
         modular_power(base, exponent - 1, modulus) % modulus;
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
  long long p, q, e, message;
  cout << "Enter two distinct primes p and q (2..1000000): ";
  if (!(cin >> p >> q) || p < 2 || q < 2 ||
      p > 1000000 || q > 1000000 || p == q ||
      !is_prime(p) || !is_prime(q)) {
    cout << "Invalid prime numbers.\n";
    return 1;
  }
  long long n = p * q;
  long long phi = (p - 1) * (q - 1);
  cout << "Enter the public exponent e: ";
  if (!(cin >> e) || e <= 1 || e >= phi || gcd(e, phi) != 1) {
    cout << "Use 1 < e < phi(n) with gcd(e, phi(n)) = 1.\n";
    return 1;
  }
  long long x, y;
  extended_gcd(e, phi, x, y);
  long long d = (x % phi + phi) % phi;
  cout << "Enter the integer message (0 <= m < " << n << "): ";
  if (!(cin >> message) || message < 0 || message >= n) {
    cout << "The message must belong to 0..n-1.\n";
    return 1;
  }
  long long encrypted = modular_power(message, e, n);
  cout << "n = " << n << ", phi(n) = " << phi << endl;
  cout << "Public key (e, n): (" << e << ", " << n << ")" << endl;
  cout << "Private key (d, n): (" << d << ", " << n << ")" << endl;
  cout << "Encrypted: " << encrypted << endl;
  cout << "Decrypted: " << modular_power(encrypted, d, n) << endl;
  return 0;
}
