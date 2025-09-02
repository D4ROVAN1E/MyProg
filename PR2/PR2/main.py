import math
import sys
import struct
from typing import List, Tuple

# ==============================================================================
# РАЗДЕЛ: МАТЕМАТИЧЕСКИЕ И ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
# ==============================================================================

def gcd(a: int, b: int) -> int:
    """Вычисляет наибольший общий делитель (НОД)."""
    return math.gcd(a, b)

def power(base: int, degree: int, mod: int) -> int:
    """
    Вычисляет (base^degree) % mod с помощью быстрого возведения в степень.
    Это заменяет DecompOfDegree и Ferm из C++ кода.
    """
    return pow(base, degree, mod)

def is_prime_wheel(n: int) -> bool:
    """
    Реализует оптимизированный "колесообразный" тест на простоту.
    Проверяет делимость на 2, 3, 5, а затем на числа вида 6k ± 1.
    Аналог WheelTest.
    """
    if n in {2, 3, 5}:
        return True
    if n % 2 == 0 or n % 3 == 0 or n % 5 == 0 or n <= 1:
        return False
    
    sqrt_n = int(math.sqrt(n))
    # Пропускаем множители 2, 3, 5
    for i in range(7, sqrt_n + 1, 30):
        if n % i == 0 or \
           n % (i + 4) == 0 or \
           n % (i + 6) == 0 or \
           n % (i + 10) == 0 or \
           n % (i + 12) == 0 or \
           n % (i + 16) == 0 or \
           n % (i + 22) == 0 or \
           n % (i + 24) == 0:
            return False
    return True

def extended_euclidean(a: int, m: int, output: bool = False) -> int:
    """
    Реализует расширенный алгоритм Евклида для нахождения модульного обратного.
    Возвращает x для уравнения a*x + m*y = gcd(a, m).
    """
    if gcd(a, m) != 1:
        raise ValueError("Вводимые числа не взаимно простые.")

    # В Python 3.8+ можно просто использовать: pow(a, -1, m)
    # Но для соответствия C++ коду реализуем алгоритм вручную.
    
    first_str = [m, 1, 0]
    second_str = [a, 0, 1]
    
    if output:
        print("r-------x-------y--------------------")
        print(f"{first_str[0]}\t{first_str[1]}\t{first_str[2]}")
        print(f"{second_str[0]}\t{second_str[1]}\t{second_str[2]}")

    while second_str[0] != 0:
        quotient = first_str[0] // second_str[0]
        third_str = [
            first_str[0] % second_str[0],
            first_str[1] - quotient * second_str[1],
            first_str[2] - quotient * second_str[2]
        ]
        if output:
            print(f"{third_str[0]}\t{third_str[1]}\t{third_str[2]}\tquotient = {quotient}")
        
        first_str = second_str
        second_str = third_str
        
    # first_str[1] - это x, а first_str[2] - это y
    # Нам нужен y, так как мы ищем обратное к a по модулю m
    # Результат нужно привести к положительному виду в кольце вычетов
    return (first_str[2] % m + m) % m


def solve_diophantine(a: int, b: int, c: int) -> Tuple[int, int]:
    """
    Находит решение диофантова уравнения ax + by = c.
    Аналог кода из Fraction.cpp.
    """
    common_divisor = gcd(a, b)
    if c % common_divisor != 0:
        raise ValueError("Решений в целых числах не существует, так как c не делится на НОД.")

    # Упрощаем уравнение
    a_prime = a // common_divisor
    b_prime = b // common_divisor
    c_prime = c // common_divisor

    # Находим решение для a_prime*x + b_prime*y = 1
    # Расширенный алгоритм Евклида делает это напрямую
    m_inv = extended_euclidean(a_prime, b_prime)
    y0_base = (m_inv * 1) % b_prime # y0 для c=1
    x0_base = (1 - b_prime * y0_base) // a_prime
    
    # Масштабируем до c_prime
    x0 = x0_base * c_prime
    y0 = y0_base * c_prime
    
    return x0, y0


# ==============================================================================
# РАЗДЕЛ: ШИФР ШАМИРА
# ==============================================================================

def _text_transform(data: List[int], key: int, p: int) -> List[int]:
    """Вспомогательная функция для поэлементного возведения в степень по модулю."""
    return [power(char_code, key, p) for char_code in data]

def shamir_encrypt(ca: int, cb: int, p: int, message: bytes) -> List[int]:
    """Функция для шифрования сообщения по схеме Шамира."""
    if gcd(ca, p - 1) != 1 or gcd(cb, p - 1) != 1:
        raise ValueError("Ключи должны быть взаимнопростыми с p-1.")
    if not (256 < p < 2**32):
        raise ValueError("Простое число p должно быть больше 256 и меньше 2^32.")

    # 1. Алиса шифрует сообщение своим ключом cA
    # M -> M^cA mod p
    encrypted_by_a = _text_transform(list(message), ca, p)

    # 2. Боб шифрует полученное сообщение своим ключом cB
    # (M^cA)^cB mod p -> M^(cA*cB) mod p
    encrypted_by_b = _text_transform(encrypted_by_a, cb, p)

    return encrypted_by_b

def shamir_decrypt(ca: int, cb: int, p: int, cyphertext: List[int]) -> bytes:
    """Функция для расшифровки сообщения по схеме Шамира."""
    if gcd(ca, p - 1) != 1 or gcd(cb, p - 1) != 1:
        raise ValueError("Ключи должны быть взаимнопростыми с p-1.")
    if not (256 < p < 2**32):
        raise ValueError("Простое число p должно быть больше 256 и меньше 2^32.")

    # Абонент A вычисляет секретное число dA, такое что (cA * dA) mod (p-1) = 1
    da = extended_euclidean(ca, p - 1)
    
    # Абонент B вычисляет секретное число dB, такое что (cB * dB) mod (p-1) = 1
    db = extended_euclidean(cb, p - 1)

    # 3. Алиса расшифровывает своим ключом dA
    # (M^(cA*cB))^dA mod p -> M^(cB) mod p
    decrypted_by_a = _text_transform(cyphertext, da, p)

    # 4. Боб расшифровывает своим ключом dB
    # (M^cB)^dB mod p -> M mod p
    decrypted_by_b = _text_transform(decrypted_by_a, db, p)

    return bytes(decrypted_by_b)

# ==============================================================================
# РАЗДЕЛ: АТАКА "ВСТРЕЧА ПОСЕРЕДИНЕ"
# ==============================================================================

def meet_in_the_middle_attack(p: int, M: int, C: int, key_limit: int):
    """Функция, реализующая атаку "встреча посередине"."""
    print("--- Запуск атаки 'Встреча посередине' ---")
    print("Атакуем первый символ сообщения...")
    print(f"Символ открытого текста (M): {M}")
    print(f"Символ шифротекста (C): {C}")
    print(f"Поиск ключей в диапазоне до: {key_limit}\n")
    
    intermediate_values = {}

    # Фаза 1: Сторона Алисы. Шифруем M всеми возможными ключами gA.
    # X = M^gA mod p
    print(f"Фаза 1: Вычисление и сохранение M^gA для gA от 2 до {key_limit}...")
    for ga in range(2, key_limit + 1):
        if gcd(ga, p - 1) == 1:
            intermediate = power(M, ga, p)
            intermediate_values[intermediate] = ga
    print(f"Фаза 1 завершена. Сохранено {len(intermediate_values)} промежуточных значений.\n")

    # Фаза 2: Сторона Боба. Расшифровываем C всеми возможными ключами gB.
    # X = C^d_gB mod p
    print("Фаза 2: Вычисление C^d_gB и поиск совпадения...")
    for gb in range(2, key_limit + 1):
        if gcd(gb, p - 1) != 1:
            continue
        
        # Находим d_gB, такое что gB * d_gB = 1 mod (p-1)
        d_gb = extended_euclidean(gb, p - 1)
        decrypted_intermediate = power(C, d_gb, p)

        # Проверяем, есть ли результат в сохраненных значениях
        if decrypted_intermediate in intermediate_values:
            found_ca = intermediate_values[decrypted_intermediate]
            found_cb = gb
            print("\n--- АТАКА УСПЕШНА! ---")
            print("Найдено совпадение!")
            print(f"Промежуточное значение: {decrypted_intermediate}")
            print("Найдены потенциальные ключи:")
            print(f"  cA = {found_ca}")
            print(f"  cB = {found_cb}")
            print("--------------------------")
            return

    print("\n--- АТАКА ПРОВАЛЕНА ---")
    print(f"Не удалось найти ключи в заданном диапазоне поиска ({key_limit}).")
    print("---------------------")

# ==============================================================================
# РАЗДЕЛ: ГЛАВНОЕ МЕНЮ И ЛОГИКА ПРИЛОЖЕНИЯ
# ==============================================================================

def display_menu():
    """Отображает главное меню."""
    print("\nВыберите действие:")
    print("1) Сравнение двух чисел с возведением в степень по модулю")
    print("2) Расширенный алгоритм Евклида для чисел c и d")
    print("3) Нахождение обратного числа по модулю")
    print("4) Шифрование и дешифрование текста алгоритмом Шамира")
    print("5) Решение уравнения a*275 + b*145 = 10")
    print("6) Демонстрация атаки 'Встреча посередине' на шифр Шамира")
    print("7) Зашифровать файл алгоритмом Шамира")
    print("8) Расшифровать файл алгоритмом Шамира") 
    print("0) Выход")
    
def get_user_choice() -> int:
    """Получает и проверяет выбор пользователя."""
    while True:
        try:
            choice = int(input("Ваш выбор: "))
            return choice
        except ValueError:
            print("Ошибка: Введите число.")

def handle_modulo_comparison():
    try:
        b1, d1 = map(int, input("Введите первое число и его степень через пробел: ").split())
        b2, d2 = map(int, input("Введите второе число и его степень через пробел: ").split())
        p = int(input("Введите модуль (простое число): "))
        
        if not is_prime_wheel(p):
            raise ValueError("Модуль не является простым.")
        if not (gcd(b1, p) == 1 and gcd(b2, p) == 1):
            raise ValueError("Основания должны быть взаимнопростыми с модулем.")

        res1 = power(b1, d1, p)
        res2 = power(b2, d2, p)
        print(f"{b1}^{d1} mod {p} = {res1}")
        print(f"{b2}^{d2} mod {p} = {res2}")
        relation = "равны" if res1 == res2 else "не равны"
        print(f"Результат: Числа {relation} по модулю.")

    except ValueError as e:
        print(f"Ошибка ввода: {e}")

def handle_extended_euclidean():
    try:
        c, d = map(int, input("Введите два числа c и d для расширенного алгоритма Евклида: ").split())
        print(f"Находим y для уравнения {c}x + {d}y = НОД({c}, {d})")
        # Ищем обратное к d по модулю c
        y = extended_euclidean(d, c, output=True)
        print(f"Один из коэффициентов (y) = {y}")
    except ValueError as e:
        print(f"Ошибка ввода: {e}")

def handle_modular_inverse():
    try:
        num, mod = map(int, input("Введите число и модуль для нахождения обратного элемента: ").split())
        d_inv = extended_euclidean(num, mod, output=True)
        print(f"\nОбратный элемент d = {d_inv}")
        print(f"{num}^(-1) mod {mod} = {d_inv}")
    except ValueError as e:
        print(f"Ошибка ввода: {e}")

def handle_shamir_protocol():
    p, ca, cb = 4091, 101, 103
    print("\n--- Легитимный протокол Шамира ---")
    msg_str = input("Введите сообщение для шифрования: ")
    msg_bytes = msg_str.encode('utf-8')
    
    print(f"Оригинальное сообщение: {msg_str}")
    print(f"Параметры: p={p}, cA={ca}, cB={cb}")

    cyphertext = shamir_encrypt(ca, cb, p, msg_bytes)
    print(f"Зашифрованное сообщение (шифротекст): {cyphertext}")

    decrypted_bytes = shamir_decrypt(ca, cb, p, cyphertext)
    decrypted_msg = decrypted_bytes.decode('utf-8')
    print(f"Расшифрованное сообщение: {decrypted_msg}")
    
def handle_diophantine_equation():
    a, b, c = 275, 145, 10
    print(f"Решение уравнения {a}*x + {b}*y = {c}")
    try:
        x, y = solve_diophantine(a, b, c)
        print(f"Одно из частных решений: x = {x}, y = {y}")
        print(f"Проверка: {a}*{x} + {b}*{y} = {a * x + b * y}")
    except ValueError as e:
        print(f"Ошибка: {e}")
        
def handle_meet_in_the_middle():
    p, ca, cb = 4091, 101, 103
    print("--- Легитимный протокол Шамира ---")
    msg_str = "This is a secret message."
    msg_bytes = msg_str.encode('utf-8')
    print(f"Оригинальное сообщение: {msg_str}")
    print(f"Параметры: p={p}, cA={ca}, cB={cb}\n")
    
    cyphertext = shamir_encrypt(ca, cb, p, msg_bytes)
    print(f"Зашифрованное сообщение (шифротекст): {cyphertext}\n")
    
    decrypted_bytes = shamir_decrypt(ca, cb, p, cyphertext)
    print(f"Расшифрованное сообщение: {decrypted_bytes.decode('utf-8')}\n")
    
    # Запускаем атаку, используя только первый символ
    meet_in_the_middle_attack(p, msg_bytes[0], cyphertext[0], 200)

def handle_encrypt_file():
    try:
        in_filename = input("Введите полный путь до файла для шифрования: ")
        out_filename = input("Введите полный путь для сохранения результата: ")
        
        with open(in_filename, 'rb') as f_in:
            file_data = f_in.read()
        
        p, ca, cb = 4091, 101, 103
        
        cypherdata = shamir_encrypt(ca, cb, p, file_data)
        
        with open(out_filename, 'wb') as f_out:
            for block in cypherdata:
                # Пакуем каждое число в 4 байта (uint32_t)
                f_out.write(struct.pack('<I', block))
                
        print(f"Файл '{in_filename}' успешно зашифрован в '{out_filename}'.")
    except FileNotFoundError:
        print("Ошибка: Файл не найден.")
    except Exception as e:
        print(f"Произошла ошибка: {e}")

def handle_decrypt_file():
    try:
        in_filename = input("Введите полный путь до зашифрованного файла: ")
        out_filename = input("Введите полный путь для сохранения результата: ")

        cypherdata = []
        with open(in_filename, 'rb') as f_in:
            while True:
                # Читаем по 4 байта
                chunk = f_in.read(4)
                if not chunk:
                    break
                # Распаковываем 4 байта в число (uint32_t)
                block = struct.unpack('<I', chunk)[0]
                cypherdata.append(block)

        p, ca, cb = 4091, 101, 103

        decrypted_data = shamir_decrypt(ca, cb, p, cypherdata)

        with open(out_filename, 'wb') as f_out:
            f_out.write(decrypted_data)

        print(f"Файл '{in_filename}' успешно расшифрован в '{out_filename}'.")
    except FileNotFoundError:
        print("Ошибка: Файл не найден.")
    except Exception as e:
        print(f"Произошла ошибка: {e}")

def main():
    """Главная функция, запускающая цикл меню."""
    menu_actions = {
        1: handle_modulo_comparison,
        2: handle_extended_euclidean,
        3: handle_modular_inverse,
        4: handle_shamir_protocol,
        5: handle_diophantine_equation,
        6: handle_meet_in_the_middle,
        7: handle_encrypt_file,
        8: handle_decrypt_file,
    }

    while True:
        display_menu()
        choice = get_user_choice()

        if choice == 0:
            print("Выход из программы.")
            break
        
        action = menu_actions.get(choice)
        if action:
            try:
                action()
            except Exception as e:
                print(f"Произошла непредвиденная ошибка: {e}", file=sys.stderr)
        else:
            print("Неверный выбор. Пожалуйста, попробуйте снова.")

if __name__ == "__main__":
    main()