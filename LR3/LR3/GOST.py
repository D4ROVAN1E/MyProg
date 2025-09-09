import math
import random
from typing import List, Tuple, Dict

# Глобальные переменные для хранения простых чисел
primes = []
sieve_results = {}

def sieve_of_eratosthenes(limit=500):
    """Решето Эратосфена для чисел меньше 500"""
    global primes, sieve_results
    
    is_prime = [True] * limit
    is_prime[0] = is_prime[1] = False
    
    i = 2
    while i * i < limit:
        if is_prime[i]:
            j = i * i
            while j < limit:
                is_prime[j] = False
                j += i
        i += 1
    
    primes.clear()
    sieve_results.clear()
    
    for i in range(2, limit):
        sieve_results[i] = is_prime[i]
        if is_prime[i]:
            primes.append(i)

def factorize(n):
    """Факторизация числа на простые множители"""
    factors = []
    
    for p in primes:
        if p * p > n:
            break
        
        count = 0
        while n % p == 0:
            n //= p
            count += 1
        
        if count > 0:
            factors.append((p, count))
    
    if n > 1:
        factors.append((n, 1))
    
    return factors

def mod_pow(base, exponent, modulus):
    """Быстрое возведение в степень по модулю"""
    if modulus == 1:
        return 0
    
    result = 1
    base %= modulus
    
    while exponent > 0:
        if exponent & 1:
            result = (result * base) % modulus
        base = (base * base) % modulus
        exponent >>= 1
    
    return result

def miller_test(n, t):
    """Тест Миллера для проверки простоты"""
    if n < 2:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    
    # Факторизуем n-1
    factors = factorize(n - 1)
    
    # Выполняем t проверок
    rand_num = []
    for test in range(t):
        a = random.randint(2, n - 1)
        rand_num.append(a)
    
    for a in rand_num:
        # Проверка малой теоремы Ферма
        fermat_result = mod_pow(a, n - 1, n)
        if fermat_result != 1:
            return False
    
    # Дополнительные проверки для каждого простого множителя
    for factor in factors:
        all_equal_one = True
        for a in rand_num:
            exp = (n - 1) // factor[0]
            result = mod_pow(a, exp, n)
            
            if result != 1:
                all_equal_one = False
                break
        
        if all_equal_one:
            return False
    
    return True

def miller_generate_prime(k, t):
    """Генерация простого числа методом Миллера"""
    n_min = 1 << (k - 2)
    n_max = (1 << (k - 1)) - 1
    n = 0
    m = 1
    
    while not miller_test(n, t):
        m = 1
        while not (n_min < m < n_max) and (m > 0):
            prime_it = random.randint(0, len(primes) - 1)
            prime_exp = random.randint(0, 2)
            m *= int(pow(primes[prime_it], prime_exp))
            
            while m >= n_max:
                m //= int(pow(primes[prime_it], prime_exp))
                if prime_it - 1 < 0:
                    break
                prime_it -= 1
                m *= int(pow(primes[prime_it], prime_exp))
        
        n = 2 * m + 1
    
    return n

def pocklington_test(n, F, t=10):
    """Тест Поклингтона для проверки простоты"""
    if n < 2:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    
    # Получаем факторизацию F
    F_factors = factorize(F)
    
    # Выполняем t проверок
    rand_num = []
    for test in range(t):
        a = random.randint(2, n - 1)
        rand_num.append(a)
    
    for a in rand_num:
        # Проверяем малую теорему Ферма
        if mod_pow(a, n - 1, n) != 1:
            return False  # n - составное
    
    # Проверяем, что для всех q результат не равен 1
    for a in rand_num:
        all_not_one = True
        for factor in F_factors:
            exp = (n - 1) // factor[0]
            result = mod_pow(a, exp, n)
            
            if result == 1:
                all_not_one = False
                break
        
        if all_not_one:
            return True
    
    return False

def pocklington_generate_prime(bits, t=10):
    """Генерация простого числа методом Поклингтона"""
    if bits < 4:
        return 0
    
    max_R = (1 << (bits // 2)) - 1
    min_R = 1 << (bits // 2 - 1)
    max_F = (1 << (bits // 2 + 1)) - 1
    min_F = 1 << (bits // 2)
    F = 1
    n = 1
    
    while not pocklington_test(n, F, t):
        # Строим F
        F = 1
        while not (min_F <= F < max_F) and (F > 0):
            prime_it = random.randint(0, len(primes) - 1)
            prime_exp = random.randint(0, 2)
            F *= int(pow(primes[prime_it], prime_exp))
            
            while F >= max_F:
                F //= int(pow(primes[prime_it], prime_exp))
                if prime_it - 1 < 0:
                    break
                prime_it -= 1
                F *= int(pow(primes[prime_it], prime_exp))
        
        R = random.randint(min_R // 2, max_R // 2) * 2  # Делаем четным
        n = R * F + 1
    
    return n

# Глобальная переменная для seed LCG
lcg_seed = None

def linear_congruential_generator():
    """Линейный конгруэнтный генератор для получения случайных чисел в (0,1)"""
    global lcg_seed
    
    if lcg_seed is None:
        lcg_seed = random.randint(0, 2**32 - 1)
    
    a = 1664525  # a = 1 mod 4
    c = 1013904223  # gcd(c,m) = 1
    m = 4294967296  # 2^32
    
    lcg_seed = (a * lcg_seed + c) % m
    return lcg_seed / m

def gost_generate_prime(t, q):
    """Генерация простого числа по алгоритму ГОСТ Р 34.10-94"""
    while True:
        get_num = True
        
        # Вычисляем N
        xi = linear_congruential_generator()
        
        # N = floor(2^(t-1) / q) + floor(xi * 2^(t-1) / q)
        power_t_minus_1 = 1 << (t - 1)
        N = power_t_minus_1 // q + int(xi * power_t_minus_1 // q)
        
        # Если N нечетное, то N = N + 1
        if N % 2 == 1:
            N = N + 1
        
        u = 0
        p = 1
        
        while True:
            # Вычисляем p = (N + u) * q + 1
            p = (N + u) * q + 1
            
            # Проверяем, что p <= 2^t
            power_t = 1 << t
            if p > power_t:
                get_num = False
                break
            
            if mod_pow(2, p - 1, p) == 1 and mod_pow(2, N + u, p) != 1:
                break
            
            u = u + 2
        
        if get_num:
            return p

def generate_prime_of_size(target_bits):
    """Генерация простого числа заданной битовой длины"""
    if target_bits <= 2:
        return 3  # Минимальное простое число
    
    min_primal_target = target_bits
    while min_primal_target > 9:
        min_primal_target = math.ceil(min_primal_target / 2)
    
    # Начинаем с малого простого числа
    current_prime = 3
    current_bits = 0
    min_prime_ind = 0
    
    while current_bits != min_primal_target:
        current_prime = primes[min_prime_ind]
        min_prime_ind += 1
        current_bits = math.ceil(math.log2(current_prime))
    
    # Пошагово увеличиваем размер простого числа
    while current_bits < target_bits:
        next_bits = current_bits * 2
        next_prime = gost_generate_prime(next_bits, current_prime)
        
        current_prime = next_prime
        current_bits = next_bits
    
    return current_prime

def main():
    sieve_of_eratosthenes()
    run = True
    
    while run:
        print("=== Выберите процедуру генерации простых чисел ===")
        print("1) Миллер")
        print("2) Поклингтон")
        print("3) ГОСТ")
        print("4) Выход")
        
        try:
            choice = int(input())
        except ValueError:
            print("Неверный ввод")
            continue
        
        if choice == 1:
            print("Введите размер простого числа в битах (5 - 32)")
            try:
                t_bits = int(input())
            except ValueError:
                print("Неверный ввод")
                continue
                
            if t_bits > 32 or t_bits < 5:
                print("Вы ввели некорректное кол-во бит")
                continue
            
            # Проверка отвергнутых решетом Эратосфена
            k = 0
            for num, is_prime in sieve_results.items():
                if not is_prime:
                    if miller_test(num, 20):
                        k += 1
            
            print(f"{'N':<3}{'P':<15}{'Проверка вероятностным тестом':<35}{'k':<6}")
            for i in range(1, 11):
                num = miller_generate_prime(t_bits, 20)
                test_res = '+' if miller_test(num, 20) else '-'
                print(f"{i:<3}{num:<15}{test_res:<35}{k:<6}")
        
        elif choice == 2:
            print("Введите размер простого числа в битах (5 - 32)")
            try:
                t_bits = int(input())
            except ValueError:
                print("Неверный ввод")
                continue
                
            if t_bits > 32 or t_bits < 5:
                print("Вы ввели некорректное кол-во бит")
                continue
            
            # Проверка отвергнутых решетом Эратосфена
            k = 0
            for num, is_prime in sieve_results.items():
                if not is_prime:
                    factor = factorize(num)
                    if pocklington_test(num, factor[-1][0] - 1, 20):
                        k += 1
            
            print(f"{'N':<3}{'P':<15}{'Проверка вероятностным тестом':<35}{'k':<6}")
            for i in range(1, 11):
                num = pocklington_generate_prime(t_bits, 20)
                factor = factorize(num)
                test_res = '+' if pocklington_test(num, factor[-1][0] - 1, 20) else '-'
                print(f"{i:<3}{num:<15}{test_res:<35}{k:<6}")
        
        elif choice == 3:
            print("Введите размер простого числа в битах (5 - 32)")
            try:
                t_bits = int(input())
            except ValueError:
                print("Неверный ввод")
                continue
                
            if t_bits > 32 or t_bits < 5:
                print("Вы ввели некорректное кол-во бит")
                continue
            
            # Проверка отвергнутых решетом Эратосфена
            k = 0
            for num, is_prime in sieve_results.items():
                if not is_prime:
                    if miller_test(num, 20):
                        k += 1
            
            print(f"{'N':<3}{'P':<15}{'Проверка вероятностным тестом':<35}{'k':<6}")
            for i in range(1, 11):
                num = generate_prime_of_size(t_bits)
                test_res = '+' if miller_test(num, 20) else '-'
                print(f"{i:<3}{num:<15}{test_res:<35}{k:<6}")
        
        elif choice == 4:
            run = False
        
        else:
            print("Неверный ввод")

if __name__ == "__main__":
    main()