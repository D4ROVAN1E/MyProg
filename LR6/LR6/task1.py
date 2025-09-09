import random
import os
import time
import collections

MAX_SIZE = 100
GRID_SIZE = 50

def clear_screen():
    """Функция для очистки экрана."""
    os.system('cls' if os.name == 'nt' else 'clear')

#ЗАДАЧА 1
def task1():
    """Задача 1: Квадратная матрица N×N, найти строку с наибольшим количеством положительных чисел."""
    print("=== Задача 1 ===")
    try:
        n = int(input("Введите порядок квадратной матрицы N: "))
        if not (0 < n <= MAX_SIZE):
            print("Неверный размер матрицы!")
            return
    except ValueError:
        print("Ошибка ввода! Введите целое число.")
        return

    matrix = [[random.randint(-100, 100) for _ in range(n)] for _ in range(n)]
    
    print("\nСгенерированная матрица:")
    for row in matrix:
        for val in row:
            print(f"{val}\t", end="")
        print()

    max_positive_count = -1
    best_row_index = -1

    for i in range(n):
        current_positive_count = sum(1 for x in matrix[i] if x > 0)
        if current_positive_count > max_positive_count:
            max_positive_count = current_positive_count
            best_row_index = i

    if best_row_index == -1:
        print("\nВ матрице нет положительных чисел.")
        return

    new_array = matrix[best_row_index]

    print(f"\nСтрока с наибольшим количеством положительных чисел (строка {best_row_index}"
          f", {max_positive_count} положительных): ", end="")
    print(*new_array, sep=" ") # вывод элементов списка через пробел
    print("\n")

#ЗАДАЧА 2
def task2():
    """Задача 2: Матрица MxN, найти число с максимальной частотой встречаемости."""
    print("=== Задача 2 ===")
    try:
        m = int(input("Введите количество строк M: "))
        n = int(input("Введите количество столбцов N: "))
        if not (0 < m <= MAX_SIZE and 0 < n <= MAX_SIZE):
            print("Неверные размеры матрицы!")
            return
    except ValueError:
        print("Ошибка ввода! Введите целые числа.")
        return

    matrix = [[random.randint(100, 150) for _ in range(n)] for _ in range(m)]

    print("\nСгенерированная матрица:")
    for row in matrix:
        for val in row:
            print(f"{val}\t", end="")
        print()
        
    counts = collections.Counter(num for row in matrix for num in row)
    
    if not counts:
        print("\nМатрица пуста.")
        return

    max_freq = 0
    most_frequent_num = -1
    has_repeated = False

    for num in range(100, 151):
        if counts[num] > max_freq:
            max_freq = counts[num]
            most_frequent_num = num
        if counts[num] > 1:
            has_repeated = True

    if not has_repeated:
        print("\nВ матрице нет чисел, которые встречаются более одного раза.")
    else:
        print(f"\nЧисло {most_frequent_num} встречается наибольшее количество раз: "
              f"{max_freq} раз(а)")
    print()

#ЗАДАЧА 3: Клеточный автомат
def count_neighbors(grid, row, col, toroidal=False):
    count = 0
    for i in range(-1, 2):
        for j in range(-1, 2):
            if i == 0 and j == 0:
                continue
            
            new_row, new_col = row + i, col + j

            if toroidal:
                new_row = (new_row + GRID_SIZE) % GRID_SIZE
                new_col = (new_col + GRID_SIZE) % GRID_SIZE
                if grid[new_row][new_col]:
                    count += 1
            else:
                if 0 <= new_row < GRID_SIZE and 0 <= new_col < GRID_SIZE:
                    if grid[new_row][new_col]:
                        count += 1
    return count

def update_grid(current, next_g, toroidal=False, custom_rules=False):
    """ИСПРАВЛЕНО: Логика правил точно соответствует C++ коду."""
    for i in range(GRID_SIZE):
        for j in range(GRID_SIZE):
            neighbors = count_neighbors(current, i, j, toroidal)
            is_alive = current[i][j]
            
            if custom_rules:
                if is_alive:
                    next_g[i][j] = (2 <= neighbors <= 4)
                else:
                    next_g[i][j] = (neighbors == 3 or neighbors == 4)
            else:
                if is_alive:
                    next_g[i][j] = (neighbors == 2 or neighbors == 3)
                else:
                    next_g[i][j] = (neighbors == 3)

def print_grid(grid):
    """ИСПРАВЛЕНО: Вывод '1' и '0' вместо символов."""
    for i in range(GRID_SIZE):
        for j in range(GRID_SIZE):
            print('1' if grid[i][j] else '0', end="")
        print()

def initialize_glider(grid, start_row, start_col):
    """Инициализация глайдера, как в C++."""
    for r in range(GRID_SIZE):
        for c in range(GRID_SIZE):
            grid[r][c] = False
            
    if start_row + 2 < GRID_SIZE and start_col + 2 < GRID_SIZE:
        grid[start_row][start_col + 1] = True
        grid[start_row + 1][start_col + 2] = True
        grid[start_row + 2][start_col] = True
        grid[start_row + 2][start_col + 1] = True
        grid[start_row + 2][start_col + 2] = True

def initialize_random_colony(grid):
    """Случайная инициализация, как в C++."""
    for i in range(GRID_SIZE):
        for j in range(GRID_SIZE):
            grid[i][j] = random.choice([True, False])

def task3():
    print("=== Задача 3: Клеточный автомат Конвея ===")

    current_grid = [[False] * GRID_SIZE for _ in range(GRID_SIZE)]
    next_grid = [[False] * GRID_SIZE for _ in range(GRID_SIZE)]

    try:
        print("Выберите демонстрацию:")
        print("1 - Глайдер (классические правила, замкнутая плоскость)")
        print("2 - Случайная колония (классические правила)")
        print("3 - Случайная колония (собственные правила)")
        choice = int(input("Ваш выбор: "))

        if choice == 1:
            print("\nДемонстрация глайдера на замкнутой плоскости:")
            initialize_glider(current_grid, 5, 5)
        elif choice in [2, 3]:
            demo_type = "классические" if choice == 2 else "собственные"
            print(f"\nДемонстрация случайной колонии ({demo_type} правила):")
            initialize_random_colony(current_grid)
        else:
            print("Неверный выбор!")
            return
            
    except ValueError:
        print("Ошибка ввода! Введите целое число.")
        return

    input("Нажмите Enter для начала симуляции...")

    for generation in range(100):
        time.sleep(0.1)
        clear_screen()
        print(f"Поколение: {generation}")
        print(f"Правила: {'Собственные' if choice == 3 else 'Классические Конвея'}")
        print(f"Плоскость: {'Замкнутая' if choice == 1 else 'Ограниченная'}")
        print()

        print_grid(current_grid)
        
        toroidal = (choice == 1)
        custom_rules = (choice == 3)
        update_grid(current_grid, next_grid, toroidal, custom_rules)
        
        current_grid, next_grid = next_grid, current_grid
        
        time.sleep(0.1)

    print("\nСимуляция завершена.\n")

#Главное меню
def main():
    print("Программа для варианта 12\n")
    while True:
        print("Выберите задачу:")
        print("1 - Задача 1 (Квадратная матрица, строка с максимумом положительных)")
        print("2 - Задача 2 (Матрица MxN, число с максимальной частотой)")
        print("3 - Задача 3 (Клеточный автомат Конвея)")
        print("0 - Выход")
        
        try:
            choice = int(input("Ваш выбор: "))
            
            if choice == 1:
                task1()
            elif choice == 2:
                task2()
            elif choice == 3:
                task3()
            elif choice == 0:
                print("До свидания!")
                break
            else:
                print("Неверный выбор!")

            if choice != 0:
                input("Нажмите Enter для продолжения...")

        except ValueError:
            print("Ошибка ввода! Пожалуйста, введите число.")
        except KeyboardInterrupt:
            print("\nДо свидания!")
            break

if __name__ == "__main__":
    main()