#include <iostream>
#include <random>
using namespace std;

long long generate_random_test_number(long long limit) {
  long long result;

  random_device rd;
  mt19937_64 gen(rd());

  long long min = 2;

  uniform_int_distribution<long long> distribution(min, limit);

  result = distribution(gen);

  return result;
}

// computes base^exponent % prime (square-and-multiply, your recursion)
long long fermats_primality_test(long long exponent, long long base,
                                 long long prime) {
  if (exponent == 0) {
    return 1;
  }

  if (exponent % 2 == 0) {
    return fermats_primality_test(exponent / 2, (base * base) % prime, prime);
  }

  return (base * fermats_primality_test(exponent - 1, base, prime)) % prime;
}

// returns true if base says "probably prime", false if "definitely composite"
bool test_sequence(long long prime, long long base) {
  // write prime - 1 as 2^s * d with d odd
  long long d = prime - 1;
  int s = 0;
  while (d % 2 == 0) {
    d /= 2;
    s++;
  }

  // sequence: base^d, base^(2d), base^(4d), ... base^(2^s * d)
  long long previousResult = fermats_primality_test(d, base, prime);
  long long currentResult;

  // the very first term is already 1, so no 1 appears after a non-trivial
  // number
  if (previousResult == 1) {
    return true;
  }

  for (int i = 1; i <= s; i++) {
    currentResult = fermats_primality_test(d * (1LL << i), base, prime);

    // first 1 reached: the number before it must be n - 1
    if (currentResult == 1) {
      return previousResult == prime - 1;
    }
    previousResult = currentResult;
  }

  // never reached 1 at all, so it even fails Fermat's little theorem
  return false;
}

int main() {
  // steps,
  //  generate a random number list of 2 to n - 2
  //  do the fermat's little theorem and generate the sequence until 1's are out
  //  find if the number before 1 is n-1 (or the sequence starts at 1)
  //  if yes, try out with another number
  //  if no, output it

  cout << "Enter the prime number you want to do this test for: ";
  long long input;
  cin >> input;

  // small cases the random range [2, n-2] can't handle
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

  long long limit = input - 2;
  cout << "Enter the number of rounds you want to perform" << endl;
  int rounds;
  cin >> rounds;

  // loop through all those number and generate random numbers on each go
  for (int i = 0; i < rounds; i++) {
    long long test = generate_random_test_number(limit);

    bool isPrime = test_sequence(input, test);

    if (!isPrime) {
      cout << input << " is composite (witness: " << test << ", round " << i + 1
           << ")" << endl;
      return 0;
    }
  }

  cout << input << " is probably prime after " << rounds << " rounds" << endl;
  return 0;
}
