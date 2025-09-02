import math

def check_positive(input_num):
    """
    Проверяет, является ли входное число положительным.
    Вызывает ValueError, если число отрицательное.
    """
    if input_num < 0:
        raise ValueError("Введённое число должно быть положительным")

def get_modulo_comparison_input():
    """Запрашивает у пользователя входные данные для сравнения по модулю."""
    print("Введите первое число и его степень через пробел")
    first_base_str, first_degree_str = input().split()
    test_input_first = int(first_base_str)
    test_input_second = int(first_degree_str)
    check_positive(test_input_first)
    check_positive(test_input_second)
    first_base = test_input_first
    first_degree = test_input_second

    print("Введите второе число и его степень через пробел")
    second_base_str, second_degree_str = input().split()
    test_input_first = int(second_base_str)
    test_input_second = int(second_degree_str)
    check_positive(test_input_first)
    check_positive(test_input_second)
    second_base = test_input_first
    second_degree = test_input_second

    print("Введите модуль (число должно быть простым >0)")
    prime_num_str = input()
    test_input_first = int(prime_num_str)
    check_positive(test_input_first)
    prime_num = test_input_first
    
    return first_base, second_base, prime_num, first_degree, second_degree

def wheel_test(prime_num):
    """
    Реализует оптимизированный "колесообразный" тест на простоту.
    Проверяет делимость числа на 2, 3, 5, а затем на числа вида 6k ± 1.
    """
    if prime_num in (2, 3, 5):
        return True
    if prime_num % 2 == 0 or prime_num % 3 == 0 or prime_num % 5 == 0:
        print("Число не простое, оно делится на 2, 3 или 5")
        return False

    square_of_num = int(math.sqrt(prime_num))
    divider = 7
    count_of_inc = 0
    div_incr = [4, 2, 4, 2, 4, 6, 2, 6]

    while divider <= square_of_num:
        if prime_num % divider == 0:
            print(f"Число делится на {divider}")
            return False
        if count_of_inc > 7:
            count_of_inc = 0
        divider += div_incr[count_of_inc]
        count_of_inc += 1
    return True

def ferm(base, degree, prime_num):
    """
    Вычисляет остаток от деления A^B mod M с использованием теоремы Ферма.
    """
    degree %= (prime_num - 1)
    result = 1
    base64 = base % prime_num

    for _ in range(degree):
        result = (result * base64) % prime_num
    
    return result

def gcd(first_num, second_num):
    """Вычисляет наибольший общий делитель."""
    while second_num != 0:
        remains = first_num % second_num
        first_num = second_num
        second_num = remains
    return first_num

def theorem_conditions(base, prime_num):
    """Проверяет, применимы ли условия теоремы Ферма."""
    return gcd(base, prime_num) == 1 and ferm(base, prime_num - 1, prime_num) == 1

def squares_of_base(base, powers_of_two, prime_num):
    """
    Вычисляет степени двойки от основания по модулю.
    Используется для быстрого возведения в степень.
    """
    base_squares = [base % prime_num]
    for i in range(1, powers_of_two + 1):
        base_squares.append((base_squares[i - 1] * base_squares[i - 1]) % prime_num)
    return base_squares

def degree_to_bin(degree):
    """Преобразует степень в двоичное представление (в виде списка)."""
    binary_degree = []
    while degree > 0:
        binary_degree.append(degree % 2)
        degree //= 2
    return binary_degree

def check_log(powers_of_two, degree):
    """
    Проверяет и корректирует количество степеней двойки.
    Возвращает скорректированное значение.
    """
    full_degree = 2 ** powers_of_two
    if full_degree > degree:
        return powers_of_two - 1
    return powers_of_two

def decomp_of_degree(base, degree, prime_num):
    """
    Вычисляет остаток от деления A^B mod M с помощью метода разложения степени.
    (Алгоритм "возведение в степень путём двоичного разложения показателя")
    """
    if degree == 0:
        return 1
    if degree == 1:
        return base % prime_num
        
    powers_of_two = math.floor(math.log2(degree))
    powers_of_two = check_log(powers_of_two, degree)

    base_squares = squares_of_base(base, powers_of_two, prime_num)
    binary_degree = degree_to_bin(degree)
    result = 1

    # Перемножаем нужные множители (те, где бит в двоичной степени равен 1)
    # Убедимся, что binary_degree имеет достаточную длину
    while len(binary_degree) <= powers_of_two:
        binary_degree.append(0)

    for i in range(powers_of_two + 1):
        if binary_degree[i] == 1:
            result = (result * base_squares[i]) % prime_num
            
    return result