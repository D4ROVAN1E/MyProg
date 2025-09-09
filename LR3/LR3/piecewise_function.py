import math

# Функция для вычисления значения кусочно-заданной функции
def calculate_function(x):
    if -10 <= x <= -8:
        return -3
    elif -8 < x <= -3:
        return (3.0 / 5.0) * (x + 3)
    elif -3 < x <= 3:
        # Для x > 3, (9 - x**2) становится отрицательным,
        # что приводит к ошибке при вычислении квадратного корня.
        # В данном случае, это не произойдет, так как
        # условие `x <= 3` это предотвращает.
        return -math.sqrt(9 - x**2)
    elif 3 < x <= 5:
        return x - 3
    elif 5 < x <= 8:
        return 3
    else:
        # Если x не входит в область определения
        return 0

def main():
    x_start = -10.0  # Начальное значение x
    x_end = 8.0      # Конечное значение x
    dx = 0.5         # Шаг изменения x

    print("Таблица значений кусочно-заданной функции:")
    print("==========================================")
    print(f"{'x':>8} {'y':>12} {'Ветвь функции':>20}")
    print("==========================================")

    x = x_start
    while x <= x_end:
        y = calculate_function(x)

        # Определение текущей ветви для вывода
        branch = ""
        if -10 <= x <= -8:
            branch = "y = -3"
        elif -8 < x <= -3:
            branch = "y = 3/5*(x+3)"
        elif -3 < x <= 3:
            branch = "y = -sqrt(9-x^2)"
        elif 3 < x <= 5:
            branch = "y = x-3"
        elif 5 < x <= 8:
            branch = "y = 3"
        else:
            branch = "Не определена"

        print(f"{x:8.1f} {y:12.4f} {branch:>20}")
        x += dx

    print("==========================================")

    total_points = int((x_end - x_start) / dx) + 1
    print(f"Всего вычислено точек: {total_points}")

if __name__ == "__main__":
    main()