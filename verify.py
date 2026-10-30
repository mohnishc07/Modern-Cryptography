from pathlib import Path
import random
import shutil
import subprocess
import tempfile

root = Path(__file__).resolve().parent
build = root / 'build'
build.mkdir(exist_ok=True)
checks = 0


def check(condition, description):
    global checks
    if not condition:
        raise AssertionError(description)
    checks += 1


def run(name, data, valid=True):
    result = subprocess.run([str(build / name)], input=data, text=True,
                            capture_output=True, timeout=10)
    check(result.returncode == (0 if valid else 1), name + ': exit status')
    return result.stdout


def encrypted(output):
    return output.split('Encrypted: ')[1].splitlines()[0]


for source in sorted(root.glob('*.cpp')):
    subprocess.run(['g++', '-std=c++17', '-O2', '-Wall', '-Wextra',
                    str(source), '-o', str(build / source.stem)], check=True)
print('Compiled all 11 C++ programs.')

output = run('introduction', '7 128 26\n')
check('Modular power: 3' in output and 'Modular inverse: 15' in output,
      'modular arithmetic example')
check('does not exist' in run('introduction', '6 0 26\n'), 'noninvertible base')
run('introduction', '7 -1 26\n', False)
check('Encrypted: Dwwdfn dw Gdzq!' in run('caesar', 'Attack at Dawn!\n3\n'),
      'Caesar known answer')
check('Encrypted: Zab!' in run('caesar', 'Abc!\n-1\n'), 'negative Caesar shift')
check('Encrypted: LXFOPV EF RNHR!' in
      run('vigenere', 'ATTACK AT DAWN!\nLEMON\n'), 'Vigenere known answer')
run('vigenere', 'HELLO\n\n', False)
run('vigenere', 'HELLO\nK3Y\n', False)
check('Encrypted: HIAT' in run('hill', 'HELP\n3 3 2 5\n'), 'Hill known answer')
check('Decrypted: HELLO' in run('hill', 'Hello!\n3 3 2 5\n'), 'Hill padding')
check('Decrypted: HELX' in run('hill', 'HELX\n3 3 2 5\n'), 'preserve genuine X')
run('hill', 'HELP\n2 4 2 4\n', False)
check('Ciphertext (hex): 526E700111' in
      run('one-time-pad', 'HELLO\n1A2B3C4D5E\n'), 'OTP XOR known answer')
run('one-time-pad', 'HELLO\nABCD\n', False)
run('one-time-pad', 'A\nZZ\n', False)
check('Decrypted: 12345678' in
      run('feistel', '12345678\n1111 2222 3333 4444\n'), 'Feistel round trip')
run('feistel', '123456789\n', False)
run('feistel', '12345678\n10000\n', False)

check(encrypted(run('des', '0123456789ABCDEF\n133457799BBCDFF1\n')) ==
      '85E813540F0AB405', 'DES known answer')
check(encrypted(run('aes', '00112233445566778899AABBCCDDEEFF\n'
                          '000102030405060708090A0B0C0D0E0F\n')) ==
      '69C4E0D86A7B0430D8CDB78070B4C55A', 'AES FIPS 197 known answer')
check(encrypted(run('aes', '0' * 32 + '\n' + '0' * 32 + '\n')) ==
      '66E94BD4EF8A2C3B884CFA59CA342B2E', 'AES zero vector')
run('des', '1234\n', False)
run('aes', 'Z' * 32 + '\n', False)
triple = run('triple-des', '0123456789ABCDEF\n133457799BBCDFF1\n'
                         '133457799BBCDFF1\n133457799BBCDFF1\n')
check(encrypted(triple) == '85E813540F0AB405', '3DES reduces to DES for equal keys')

if not shutil.which('openssl'):
    raise RuntimeError('OpenSSL is required for independent cipher checks.')
rng = random.Random(20261030)
for name, cipher, size, keysize in [('des', 'des-ecb', 8, 8),
                                   ('triple-des', 'des-ede3', 8, 24),
                                   ('aes', 'aes-128-ecb', 16, 16)]:
    for _ in range(25):
        block = rng.randbytes(size).hex().upper()
        key = rng.randbytes(keysize).hex().upper()
        keys = [key[i:i + 16] for i in range(0, 48, 16)] if name == 'triple-des' else [key]
        output = run(name, block + '\n' + '\n'.join(keys) + '\n')
        reference = subprocess.run(
            ['openssl', 'enc', '-' + cipher, '-K', key, '-nopad', '-nosalt',
             '-provider', 'default', '-provider', 'legacy'],
            input=bytes.fromhex(block), capture_output=True, check=True).stdout
        check(encrypted(output) == reference.hex().upper(), name + ': OpenSSL match')
        check('Decrypted: ' + block in output, name + ': reverse operation')
print('DES, 3DES and AES: known vectors plus 75 independent OpenSSL comparisons passed.')

for value, expected in [(0, 'not prime'), (1, 'not prime'), (2, 'is prime'),
                        (3, 'is prime'), (100, 'composite (even)'),
                        (97, 'probably prime after 8 rounds')]:
    check(expected in run('miller-rabin', f'{value}\n8\n'), 'Miller-Rabin cases')
run('miller-rabin', '97\n0\n', False)

with tempfile.TemporaryDirectory() as temporary:
    temp = Path(temporary)
    harness = temp / 'miller-core.cpp'
    harness.write_text('#define main lab_main\n#include "' +
                       str(root / 'miller-rabin.cpp') + '"\n#undef main\n' + r'''
int main() {
  long long n, base, exponent;
  while (cin >> n >> base >> exponent) {
    cout << fermats_primality_test(exponent, base, n) << " ";
    cout << test_sequence(n, base) << "\n";
  }
}
''')
    executable = temp / 'miller-core'
    subprocess.run(['g++', '-std=c++17', '-O2', str(harness), '-o', str(executable)], check=True)
    cases = []
    for number in range(5, 3000, 2):
        for base in [2, 3, 5, 7, 11]:
            if base < number - 1:
                cases.append((number, base, rng.randrange(number)))
    for _ in range(100):
        number = rng.randrange(2 ** 62, 2 ** 63 - 1) | 1
        cases.append((number, rng.randrange(2, number - 1), rng.randrange(number)))
    data = ''.join(f'{n} {b} {e}\n' for n, b, e in cases)
    result = subprocess.run([str(executable)], input=data, text=True,
                            capture_output=True, check=True)
    classifications = {}
    for (n, b, e), line in zip(cases, result.stdout.splitlines(), strict=True):
        power, passed = map(int, line.split())
        check(power == pow(b, e, n), 'Miller-Rabin modular power vs Python')
        d, s = n - 1, 0
        while d % 2 == 0:
            d //= 2
            s += 1
        x = pow(b, d, n)
        expected = x in (1, n - 1)
        for _ in range(s - 1):
            x = x * x % n
            expected |= x == n - 1
        check(bool(passed) == expected, 'Miller-Rabin witness sequence vs Python')
        classifications[n] = classifications.get(n, True) and bool(passed)
    for n in range(5, 3000, 2):
        prime = all(n % divisor for divisor in range(2, int(n ** 0.5) + 1))
        check(classifications[n] == prime, 'Miller-Rabin vs trial division')
print('Miller-Rabin: witness sequences, 64-bit arithmetic and small primes checked.')

output = run('rsa', '61 53\n17\n65\n')
check('Private key (d, n): (2753, 3233)' in output, 'RSA private exponent')
check('Encrypted: 2790' in output and 'Decrypted: 65' in output, 'RSA known answer')
for message in [0, 1, 53, 61, 3232]:
    output = run('rsa', f'61 53\n17\n{message}\n')
    check(encrypted(output) == str(pow(message, 17, 3233)), 'RSA power reference')
    check('Decrypted: ' + str(message) in output, 'RSA message round trip')
run('rsa', '61 61\n', False)
run('rsa', '15 53\n', False)
run('rsa', '61 53\n12\n', False)
run('rsa', '61 53\n17\n3233\n', False)
print(f'PASS: {checks} checks.')
