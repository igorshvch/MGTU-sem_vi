import random

import auxiliary as aux

def tobits(s):
    '''
    Conver text string to list of bits
    https://stackoverflow.com/questions/10237926/convert-string-to-list-of-bits-and-viceversa/10238140
    '''
    result = []
    for c in s:
        bits = bin(ord(c))[2:]
        bits = '0000000000000000'[len(bits):] + bits
        result.extend([int(b) for b in bits])
    return result

def frombits(bits):
    '''
    Convert string of bits to text
    https://stackoverflow.com/questions/10237926/convert-string-to-list-of-bits-and-viceversa/10238140
    '''
    chars = []
    for b in range(int(len(bits) / 16)):
        byte = bits[b*16:(b+1)*16]
        chars.append(chr(int(''.join((str(bit) for bit in byte)), 2)))
    return ''.join(chars)

@aux.eval_time
def gcd(a, b):
    '''
    Equivalent to math.gcd
    '''
    while b:
        a, b = b, a%b
    return a

@aux.eval_time
def extended_gcd(a, b):
    '''
    Calculate GCD and Bézout's coefficients
    https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
    https://medium.com/geekculture/euclidean-algorithm-using-python-dc7785bb674a
    https://stackoverflow.com/questions/16044553/solving-a-modular-equation-python
    '''
    (old_r, r) = (a, b)
    (old_s, s) = (1, 0)
    (old_t, t) = (0, 1)
    while r != 0:
        quotient = old_r // r
        (old_r, r) = (r, (old_r - quotient * r))
        (old_s, s) = (s, (old_s - quotient * s))
        (old_t, t) = (t, (old_t - quotient * t))
    return old_r, old_s, old_t #GCD, x, y

@aux.eval_time
def modulo(bits_num=1024):
    '''
    Calculeate "modulo" of two random prime numbers
    '''
    p, q = 0, 0
    test_num = 8
    flags = {
        'p_found': False,
        'q_found': False
    }
    count1 = 0
    count2 = 0
    while True:
        if not flags['p_found']:
            p = random.getrandbits(bits_num)
            if p&1 and aux.Miller_Rabin_test(p, test_num):
                flags['p_found'] = True
                count1+=1
                continue
            else:
                count1+=1
                continue
        if not flags['q_found']:
            q = random.getrandbits(bits_num)
            if q&1 and aux.Miller_Rabin_test(q, test_num):
                flags['q_found'] = True
                count2+=1
                continue
            else:
                count2+=1
                continue
        if p == q:
            flags = {
                'p_found': False,
                'q_found': False
            }
            continue
        break
    print(f" /// {modulo.__name__}: p was found on {count1} run, q - on {count2} run")
    return p*q, p, q

@aux.eval_time
def calc_phi(p, q):
    '''
    Calculate Euler's totient function
    https://en.wikipedia.org/wiki/Euler%27s_totient_function
    '''
    return (p-1)*(q-1)

@aux.eval_time
def choose_open_exp(phi):
    es = [65537, 257, 17, 5] #prime Fermat numbers exept 3
    for e in es:
        if (gcd(phi, e) == 1):
            return e
    raise BaseException("Open exponent is not coprime to phi!")

@aux.eval_time
def calc_secret_expo(e, phi):
    '''
    https://stackoverflow.com/questions/16044553/solving-a-modular-equation-python
    '''
    g, x, _ = extended_gcd(e, phi)
    if g != 1:
        return None
    else:
        return x % phi

def generate_keys(bits_num=1024):
    n, p, q = modulo(bits_num=bits_num)
    phi = calc_phi(p, q)
    e = choose_open_exp(phi)
    d = calc_secret_expo(e, phi)
    print(f"Открытая экспонента:\n{e}")
    print(f"Секретная экспонента:\n{d}")
    print(f"Модуль n:\n{n}")
    return n, e, d

@aux.eval_time
def encode(string, e, n):
    split_str = string.split()
    encoded = []
    for word in split_str:
        bits = tobits(word)
        num = int(''.join((str(b) for b in bits)), 2)
        encoded.append(pow(num, e, n))
    return encoded

@aux.eval_time
def decode(encoded_list, d, n):
    decoded = ""
    for word in encoded_list:
        num = pow(word, d, n)
        bits = bin(num)[2:]
        decoded = decoded + (frombits(("0"*(16-len(bits)%16))+bits) + " ") #align bits string to 16 bits per char format
    return decoded

if __name__ == "__main__":
    options = {
        1: 64,
        2: 128,
        3: 256,
        4: 512,
        5: 1024,
        6: 2048,
    }
    while True:
        bits_num = int(
            input(
                "\nВыберите количество бит [1({})/2({})/3({})/4({})/5({})/6({})] >>> ".format(
                    options[1], options[2], options[3], options[4], options[5], options[6] 
                )
            )
        )
        if bits_num > 4:
            print("\n\t ==== Внимание! Генерация ключа может занять несколько десятков секунд! ====\n")
        n, e, d = generate_keys(options[bits_num])
        while True:
            string = input("\nВведите строку: >>>\n")
            print()
            encoded = encode(string, e, n)
            print(f"\n===> Это массив закодированных слов:\n\t{encoded}\n")
            decoded = decode(encoded, d, n)
            print(f"\n===? Это дешифрованная строка:\n\t{decoded}\n")
            breaker = int(input("\nПродолжить шифрование или изменить разрядность (выйти) [1(Продолжить)/2(Сменить разрядность|выйти)]? >>> "))
            if breaker == 1:
                continue
            elif breaker == 2:
                break
            else:
                raise ValueError()
        breaker = int(input("\nСменить разрядность ключа или выйти [1(Сменить разрядность)/2(Выйти)]? >>> "))
        if breaker == 1:
            continue
        elif breaker == 2:
            break
        else:
            raise ValueError()