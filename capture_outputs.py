from pathlib import Path
import errno
import json
import os
import pty
import select
import subprocess
import time

root = Path(__file__).resolve().parent
output = root / 'outputs'
output.mkdir(exist_ok=True)
samples = {
    'introduction': [('modulus: ', '7 128 26')],
    'caesar': [('message: ', 'Attack at Dawn!'), ('shift: ', '3')],
    'vigenere': [('message: ', 'ATTACK AT DAWN!'), ('key: ', 'LEMON')],
    'hill': [('message: ', 'HELP'), ('(row-wise): ', '3 3 2 5')],
    'one-time-pad': [('message: ', 'HELLO'), ('per byte): ', '1A2B3C4D5E')],
    'feistel': [('in hex: ', '12345678'), ('in hex: ', '1111 2222 3333 4444')],
    'des': [('digits): ', '0123456789ABCDEF'), ('digits): ', '133457799BBCDFF1')],
    'triple-des': [('digits): ', '0123456789ABCDEF'),
                   ('digits): ', '0123456789ABCDEF'),
                   ('digits): ', '23456789ABCDEF01'),
                   ('digits): ', '456789ABCDEF0123')],
    'aes': [('digits): ', '00112233445566778899AABBCCDDEEFF'),
            ('digits): ', '000102030405060708090A0B0C0D0E0F')],
    'miller-rabin': [('test for: ', '97'), ('perform: ', '8')],
    'rsa': [('1000000): ', '61 53'), ('e: ', '17'), ('3233): ', '65')]
}


def capture(name, steps):
    master, slave = pty.openpty()
    process = subprocess.Popen([str(root / 'build' / name)], stdin=slave,
                               stdout=slave, stderr=slave, close_fds=True)
    os.close(slave)
    transcript = b''
    pending = b''
    next_step = 0
    deadline = time.monotonic() + 10
    try:
        while time.monotonic() < deadline:
            ready, _, _ = select.select([master], [], [], 0.1)
            if not ready:
                if process.poll() is not None:
                    break
                continue
            try:
                chunk = os.read(master, 4096)
            except OSError as error:
                if error.errno == errno.EIO:
                    break
                raise
            if not chunk:
                break
            transcript += chunk
            pending += chunk
            if next_step < len(steps):
                prompt, value = steps[next_step]
                if prompt.encode() in pending:
                    os.write(master, (value + '\n').encode())
                    pending = b''
                    next_step += 1
        if process.poll() is None:
            process.wait(timeout=1)
        if process.returncode != 0 or next_step != len(steps):
            raise RuntimeError(f'{name}: incomplete sample run')
    finally:
        os.close(master)
        if process.poll() is None:
            process.kill()
    return '$ ./build/' + name + '\n' + transcript.decode().replace('\r\n', '\n')


for name, steps in samples.items():
    (output / (name + '.txt')).write_text(capture(name, steps))
extra = capture('miller-rabin', [('test for: ', '100')])
with (output / 'miller-rabin.txt').open('a') as stream:
    stream.write('\n' + extra)
(output / 'sample-inputs.json').write_text(json.dumps(
    {name: [value for _, value in steps] for name, steps in samples.items()},
    indent=2) + '\n')
print('Captured 12 real terminal runs for 11 programs.')
