# Modern Cryptography

C++17 laboratory programs corresponding to the nine experiments in the record index.
Each program is standalone and uses only C++ headers. No cryptography library is
needed to build or run it.

The [PDF laboratory record](lab-record/Modern_Cryptography_Lab_Record.pdf) and
[standalone LaTeX source](lab-record/Modern_Cryptography_Lab_Record.tex) contain
the question, complete C++ program and captured terminal output for every entry.

To rebuild the PDF, run the following command twice from `lab-record/`:

```bash
pdflatex -interaction=nonstopmode -halt-on-error Modern_Cryptography_Lab_Record.tex
```

The `.tex` file embeds every program and transcript, so it compiles independently
of the repository's source files. It uses standard TeX Live packages.

| Experiment | Date | Program |
| --- | --- | --- |
| 1. Introduction: modular arithmetic | 13.07.2026 | `introduction.cpp` |
| 2(a). Caesar cipher | 20.07.2026 | `caesar.cpp` |
| 2(b). Vigenere cipher | 20.07.2026 | `vigenere.cpp` |
| 2(c). Hill cipher (2 x 2) | 20.07.2026 | `hill.cpp` |
| 3. One-Time Pad | 27.07.2026 | `one-time-pad.cpp` |
| 4. Feistel network | 03.08.2026 | `feistel.cpp` |
| 5. DES | 10.08.2026 | `des.cpp` |
| 6. Triple DES | 17.08.2026 | `triple-des.cpp` |
| 7. AES-128 | 24.08.2026 | `aes.cpp` |
| 8. Miller-Rabin primality test | 05.10.2026 | `miller-rabin.cpp` |
| 9. RSA | 12.10.2026 | `rsa.cpp` |

The photograph supplies experiment titles, not full question statements.
Implementation questions in the LaTeX record are formulated from those titles.
The introductory experiment demonstrates GCD, modular exponentiation and modular
inverse. The Hill cipher uses column vectors, uppercase letters and X padding;
spaces and punctuation are removed. Caesar and Vigenere preserve case and
punctuation. Vigenere advances the key only for letters.

## Build and run

```bash
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra aes.cpp -o build/aes
./build/aes
```

Substitute the appropriate source and executable name for another experiment.
GCC or Clang is required for the `unsigned __int128` arithmetic used by
Miller-Rabin and RSA. RSA accepts distinct primes at most 1,000,000 and an integer
message in 0..n-1. The introductory modular arithmetic program bounds the modulus
at 1,000,000,000 to keep its products within signed 64-bit arithmetic.

## Verification and captured outputs

```bash
python3 verify.py
python3 capture_outputs.py
```

Verification builds all 11 programs and checks known answers, invalid inputs,
reverse operations, 75 random comparisons against OpenSSL, Miller-Rabin witness
sequences against Python arbitrary-precision arithmetic and small primes against
trial division. It requires Python 3.10+, GCC and OpenSSL 3 with its default and
legacy providers. Capturing uses a real POSIX pseudo-terminal with input echo;
`outputs/*.txt` contains the actual runs used in the document.

These are educational demonstrations. DES and 3DES are legacy algorithms;
the block cipher programs process one block with no padding or authentication.
The four-round Feistel function is a teaching example. RSA is textbook RSA with
small parameters, not padded message encryption. A true OTP requires a uniformly
random secret key of the message length that is never reused; the fixed sample
key only demonstrates XOR. The Miller-Rabin generator samples test bases and is
not a secret-key generator.

## Algorithm references

- NIST, [FIPS 46-3: DES and TDEA](https://csrc.nist.gov/pubs/fips/46-3/final).
- NIST, [FIPS 197: AES](https://csrc.nist.gov/pubs/fips/197/final).
- Menezes, van Oorschot and Vanstone, [Handbook of Applied Cryptography,
  Chapter 4](https://cacr.uwaterloo.ca/hac/about/chap4.pdf), Algorithm 4.24.

The original Miller-Rabin function names and recursive square-and-multiply
structure are retained. Modular products now use a 128-bit intermediate,
the witness sequence squares incrementally, and nonpositive rounds are rejected.
