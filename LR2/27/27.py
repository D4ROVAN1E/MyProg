def is_valid_date(day, month):
    """
    Проверяет, является ли заданная комбинация дня и месяца корректной датой.
    """
    return day >= 1 and day <= 31 and month >= 1 and month <= 12

def check_date_format(number_str):
    """
    Проверяет, можно ли вставить символ '/' в строку так, чтобы получилась корректная дата.
    """
    if not number_str.isdigit() or len(number_str) < 2:
        return "NO"
        
    number = int(number_str)
    is_correct_date = False
    
    if 11 <= number < 100:
        day = number // 10
        month = number % 10
        is_correct_date = is_valid_date(day, month)
    elif 100 <= number < 1000:
        day = number // 10
        month = number % 10
        is_correct_date = is_valid_date(day, month)
        if not is_correct_date:
            day = number // 100
            month = number % 100
            is_correct_date = is_valid_date(day, month)
    elif 1000 <= number <= 3112:
        day = number // 100
        month = number % 100
        is_correct_date = is_valid_date(day, month)
        
    return "YES" if is_correct_date else "NO"

# Основной блок кода
input_numbers = input().split()  # Считываем последовательность чисел из ввода пользователя
n = int(input_numbers[0])  # Первое число - количество чисел для проверки
results = []

for i in range(1, n + 1):
    if i < len(input_numbers):
        results.append(check_date_format(input_numbers[i]))

print("".join(results))  