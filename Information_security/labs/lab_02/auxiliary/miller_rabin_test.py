import random

'''
Module is based on CLRS, 3ed editon, sec. 31.8 article
See more compact implementation:
https://rosettacode.org/wiki/Miller%E2%80%93Rabin_primality_test#Python
'''

def Miller_Rabin_test(n,s):
    t = 0
    u = n-1
    while u % 2 == 0:
        u >>= 1
        t += 1
    while s:
        a = random.randint(1, n-1)
        if witness(a, n, t, u):
            return False
        s -= 1
    return True

def witness(a, n, t, u):
    x = [0 for i in range(t+1)]
    x[0] = pow(a, u, n)
    for i in range(1, t+1):
        x[i] = pow(x[i-1], 2, n)
        if (x[i] == 1) and (x[i-1] != 1) and (x[i-1] != (n-1)):
            return True
    if x[t] != 1:
        return True
    return False

if __name__ == "__main__":
    print("miller_rabin_test.py module test run")
    s = 8
    count = 1000
    while count:      
        num = random.getrandbits(1024)
        if Miller_Rabin_test(num, s):
            print(f"programm has run {1000-(count-1)} times")
            print(f"The number found:\n{num}")
            print("The number is prime")
            break
        count -= 1
