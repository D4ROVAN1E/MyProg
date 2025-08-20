import random
import string

def print_container(prefix, container):
    """Функция для вывода контейнера с префиксом"""
    print(prefix, end="")
    for elem in container:
        print(elem, end=" ")
    print()

def is_prime(n):
    """Проверка, является ли число простым"""
    if n <= 1:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

def has_repeated_digits(n):
    """Проверка, есть ли в числе повторяющиеся цифры"""
    if n == 0:
        return False
    
    seen = set()
    temp = n
    while temp > 0:
        digit = temp % 10
        if digit in seen:
            return True  # Цифра уже встречалась
        seen.add(digit)
        temp //= 10
    return False

def main():
    print("--- Задания 1, 2, 3 ---")
    
    # Инициализация генератора случайных чисел
    random.seed()
    
    n = 20
    arr1 = [random.randint(10, 100) for _ in range(n)]
    arr2 = [random.randint(10, 100) for _ in range(n)]
    
    print_container("Исходный массив arr1:", arr1)
    print_container("Исходный массив arr2:", arr2)
    print()
    
    # Поиск простых чисел в диапазоне [0, 10]
    print("Простые числа из диапазона [0, 10] в arr1: ", end="")
    primes_arr1 = [val for val in arr1 if 0 <= val <= 10 and is_prime(val)]
    for val in primes_arr1:
        print(val, end=" ")
    print()
    
    print("Простые числа из диапазона [0, 10] в arr2: ", end="")
    primes_arr2 = [val for val in arr2 if 0 <= val <= 10 and is_prime(val)]
    for val in primes_arr2:
        print(val, end=" ")
    print()
    print("(Числа не найдены, так как диапазон инициализации [10, 100] не содержит простых чисел из диапазона [0, 10])")
    print()
    
    # Сортировка первой половины arr1 по возрастанию
    arr1[:n//2] = sorted(arr1[:n//2])
    
    # Сортировка второй половины arr2 по убыванию
    arr2[n//2:] = sorted(arr2[n//2:], reverse=True)
    
    print_container("arr1 (первая половина отсортирована):", arr1)
    print_container("arr2 (вторая половина отсортирована):", arr2)
    print()
    
    # Обмен отсортированной части arr1 с несортированной частью arr2
    arr1[:n//2], arr2[:n//2] = arr2[:n//2], arr1[:n//2]
    
    print("Модификация согласно заданию:")
    print_container("Финальный arr1 (несортированные части):", arr1)
    print_container("Финальный arr2 (отсортированные части):", arr2)
    print("------------------------------------------")
    print()
    
    print("--- Задание 4 ---")
    char_array_size = 25
    alphabet = string.ascii_letters  # 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
    
    char_arr = [random.choice(alphabet) for _ in range(char_array_size)]
    
    print_container("Исходный массив символов:", char_arr)
    
    # Замена регистра
    for i in range(len(char_arr)):
        if char_arr[i].islower():
            char_arr[i] = char_arr[i].upper()
        elif char_arr[i].isupper():
            char_arr[i] = char_arr[i].lower()
    
    print_container("Массив с измененным регистром:", char_arr)
    print("------------------------------------------")
    print()
    
    print("--- Задание 5 ---")
    int_array_size = 15
    
    source_arr = [random.randint(1000, 2000) for _ in range(int_array_size)]
    print_container("Исходный массив [1000, 2000]:", source_arr)
    
    # Сортировка по убыванию
    source_arr.sort(reverse=True)
    print_container("Отсортированный массив:", source_arr)
    
    # Создание нового массива с числами, где есть повторяющиеся цифры
    filtered_arr = [val for val in source_arr if has_repeated_digits(val)]
    
    print_container("Массив с числами, содержащими повторяющиеся цифры:", filtered_arr)
    print("------------------------------------------")

if __name__ == "__main__":
    main()