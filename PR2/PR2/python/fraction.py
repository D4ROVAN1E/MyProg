from modulo_comparison import gcd

def get_continued_fraction(a, b):
    """Раскладывает отношение a/b в цепную дробь."""
    coeffs = []
    while b > 0:
        quotient = a // b
        remainder = a % b
        coeffs.append(quotient)
        a = b
        b = remainder
    return coeffs

def calculate_convergents(coeffs):
    """Вычисляет подходящие дроби P_k/Q_k и возвращает их в виде двух списков."""
    P = []
    Q = []
    p_prev2, p_prev1 = 0, 1
    q_prev2, q_prev1 = 1, 0

    for ak in coeffs:
        pk = ak * p_prev1 + p_prev2
        qk = ak * q_prev1 + q_prev2

        P.append(pk)
        Q.append(qk)

        p_prev2, p_prev1 = p_prev1, pk
        q_prev2, q_prev1 = q_prev1, qk
        
    return P, Q

def solve_diophantine(orig_a, orig_b, c):
    """Находит решение уравнения ax + by = c."""
    # Проверка на разрешимость
    common_divisor = gcd(orig_a, orig_b)
    if c % common_divisor != 0:
        raise ValueError("Решений в целых числах не существует, так как c не делится на НОД")

    # Упрощение уравнения делением на НОД
    a = orig_a // common_divisor
    b = orig_b // common_divisor
    c_prime = c // common_divisor

    # Получаем коэффициенты цепной дроби
    coeffs = get_continued_fraction(a, b)

    # Вычисляем подходящие дроби
    P, Q = calculate_convergents(coeffs)

    # Нам нужны предпоследние значения P_{k-1} и Q_{k-1}
    p_penultimate = P[-2]
    q_penultimate = Q[-2]
    
    # Мы имеем решение для a*x + b*y = sign
    # sign = a * q_{k-1} - b * p_{k-1}
    
    # Умножаем x0 и y0 на c_prime и на знак, чтобы правая часть стала c_prime
    multiplier = c_prime * (a * q_penultimate - b * p_penultimate)

    a0 = q_penultimate * multiplier
    b0 = -p_penultimate * multiplier
    
    return a0, b0