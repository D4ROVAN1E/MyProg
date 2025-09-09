import math

def gcd(a, b):
    """наибольший общий делитель двух чисел"""
    while b:
        a, b = a % b, a
    return a

def power(base, exp):
    """Вычисляет степень числа"""
    res = 1
    while exp > 0:
        if exp % 2 == 1:
            res *= base
        base *= base
        exp //= 2
    return res

def main():
    a, b = map(int, input().split())

    # Согласно условию, ряд расходится, если b=1 
    if b == 1:
        print("infinity")
        return

    # При целых a > 0 и b > 1 сумма всегда является рациональным числом.
    # Коэффициенты C[k][i] - это числа Эйлера A(k, i-1).
    # C[k][i] - это коэффициент при x^i в полиноме P_k(x).
    C = [[0 for _ in range(11)] for _ in range(11)]

    # Базовый случай для рекурренсии: a=1.
    # Сумма S_1 = sum(n * x^n) = x / (1-x)^2. Полином P_1(x) = x.
    C[1][1] = 1

    # Динамически вычисляем коэффициенты полиномов Эйлера до P_a(x)
    for k in range(2, a + 1):
        for i in range(1, k + 1):
            # Рекуррентное соотношение для коэффициентов:
            # C[k][i] = i * C[k-1][i] + (k - i + 1) * C[k-1][i-1]
            C[k][i] = i * C[k - 1][i] + (k - i + 1) * C[k - 1][i - 1]

    # Вычисляем итоговую дробь по формуле S = (b * P_a(1/b)) / (b-1)^(a+1)
    # Числитель дроби P_a(1/b) * b^a, чтобы избавиться от степеней 1/b
    numerator_poly_val = 0
    for i in range(1, a + 1):
        numerator_poly_val += C[a][i] * power(b, a - i)

    # Полный числитель итоговой дроби
    final_numerator = b * numerator_poly_val

    # Знаменатель итоговой дроби
    final_denominator = power(b - 1, a + 1)

    # Сокращаем дробь, находя НОД
    common_divisor = gcd(final_numerator, final_denominator)
    
    # Проверяем на случай, если НОД равен 0 (что не должно происходить)
    if common_divisor == 0:
        print(f"{final_numerator}/{final_denominator}")
        return

    # Выводим несократимую дробь 
    print(f"{final_numerator // common_divisor}/{final_denominator // common_divisor}")

if __name__ == "__main__":
    main()