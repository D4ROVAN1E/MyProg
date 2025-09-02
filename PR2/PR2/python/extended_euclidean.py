from modulo_comparison import gcd

def extended_euclidean(first_num, second_num, output=False):
    """
    Реализует расширенный алгоритм Евклида для нахождения модульного обратного.
    Принимает два числа (firstNum, secondNum) и флаг для вывода промежуточных шагов.
    """
    if gcd(first_num, second_num) != 1:
        raise ValueError("Вводимые числа не взаимно простые")

    first_str = [first_num, 1, 0]
    second_str = [second_num, 0, 1]

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
        
    return first_str[1]