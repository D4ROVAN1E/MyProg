import math

# Константа для размера матрицы
N = 4

# Функция для вывода вектора
def print_vector(vec):
    for i in range(N):
        print(f"x{i + 1} = {vec[i]:.8f}")

# Функция для выполнения LU-разложения матрицы A
# A = L * U, где L - нижняя треугольная, U - верхняя треугольная
def lu_decomposition(A, L, U):
    for i in range(N):
        # Вычисление верхней треугольной матрицы U
        for k in range(i, N):
            sum_val = 0
            for j in range(i):
                sum_val += L[i][j] * U[j][k]
            U[i][k] = A[i][k] - sum_val

        # Вычисление нижней треугольной матрицы L
        for k in range(i, N):
            if i == k:
                L[i][i] = 1  # Диагональные элементы L равны 1
            else:
                sum_val = 0
                for j in range(i):
                    sum_val += L[k][j] * U[j][i]
                # Проверка деления на ноль
                if abs(U[i][i]) < 1e-9:
                    print("Ошибка: Деление на ноль при LU-разложении. Метод не применим.")
                    return False
                L[k][i] = (A[k][i] - sum_val) / U[i][i]
    return True

# Функция для решения системы Ax = b, используя LU-разложение
def solve_with_lu(A, b):
    L = [[0.0 for _ in range(N)] for _ in range(N)]
    U = [[0.0 for _ in range(N)] for _ in range(N)]
    x = [0.0] * N
    y = [0.0] * N

    if not lu_decomposition(A, L, U):
        return []  # Возвращаем пустой список в случае ошибки

    # Решаем систему Ly = b (прямой ход)
    for i in range(N):
        sum_val = 0
        for j in range(i):
            sum_val += L[i][j] * y[j]
        y[i] = b[i] - sum_val

    # Решаем систему Ux = y (обратный ход)
    for i in range(N - 1, -1, -1):
        sum_val = 0
        for j in range(i + 1, N):
            sum_val += U[i][j] * x[j]
        if abs(U[i][i]) < 1e-9:
            print("Ошибка: Матрица является вырожденной.")
            return []
        x[i] = (y[i] - sum_val) / U[i][i]

    return x

# Функция для проверки условия сходимости (диагональное преобладание)
# Возвращает true, если условие выполняется, иначе false.
def check_convergence(A):
    is_convergent = True
    for i in range(N):
        diag_element = abs(A[i][i])
        sum_off_diag = 0
        for j in range(N):
            if i != j:
                sum_off_diag += abs(A[i][j])
        if diag_element < sum_off_diag:
            print(f"Предупреждение: Для строки {i + 1} не выполняется условие диагонального преобладания.")
            is_convergent = False
    return is_convergent

# Функция для решения системы методом Зейделя
def seidel_method(A, b, epsilon):
    x = [0.0] * N  # Начальное приближение - нулевой вектор
    x_prev = [0.0] * N
    iterations = 0
    max_iterations = 1000

    print("\n--- Процесс итераций (метод Зейделя) ---")
    print("----------------------------------------------------------------------------------------")
    print(f"{'N':>5}{'X1':>18}{'X2':>18}{'X3':>18}{'X4':>18}{'Epsilon_n':>18}")
    print("----------------------------------------------------------------------------------------")

    while True:
        x_prev = x[:]
        for i in range(N):
            sum1 = 0
            for j in range(i):
                sum1 += A[i][j] * x[j]
            sum2 = 0
            for j in range(i + 1, N):
                sum2 += A[i][j] * x_prev[j]
            if abs(A[i][i]) < 1e-9:
                print("Ошибка: Нулевой диагональный элемент. Метод Зейделя не применим.")
                return []
            x[i] = (b[i] - sum1 - sum2) / A[i][i]

        # Вычисляем норму разности векторов для проверки сходимости
        norm = 0
        for i in range(N):
            norm += (x[i] - x_prev[i]) * (x[i] - x_prev[i])
        current_epsilon = math.sqrt(norm)

        iterations += 1

        # Вывод текущей итерации в таблицу
        print(f"{iterations:>5}{x[0]:>18.8f}{x[1]:>18.8f}{x[2]:>18.8f}{x[3]:>18.8f}{current_epsilon:>18.8f}")

        if current_epsilon < epsilon:
            break

        if iterations >= max_iterations:
            break

    print("----------------------------------------------------------------------------------------")

    if iterations == max_iterations:
        print("\nПредупреждение: Достигнуто максимальное число итераций. Решение может быть неточным.")

    return x

def main():
    # Исходные данные для варианта 11 (M=0.89, N=0.08, P=-1.21)
    A = [
        [0.89, -0.04, 0.21, -18.00],
        [0.25, -1.23, 0.08, -0.09],
        [-0.21, 0.08, 0.8, -0.13],
        [0.15, -1.31, 0.06, -1.21]
    ]

    b = [-1.24, -1.21, 2.56, 0.89]

    epsilon = 1e-3

    # --- Решение прямым методом (LU-разложение) ---
    print("1. Прямой метод: LU-разложение")
    x_lu = solve_with_lu(A, b)
    if x_lu:
        print("\nНайденные корни системы:")
        print_vector(x_lu)

    print("\n=================================================================================\n")

    # --- Решение итерационным методом (метод Зейделя) ---
    print("2. Итерационный метод: Метод Зейделя")

    # Проверка сходимости для исходной матрицы
    if check_convergence(A):
        x_seidel = seidel_method(A, b, epsilon)
        if x_seidel:
            print(f"\nНайденные корни системы (с точностью Epsilon = {epsilon}):")
            print_vector(x_seidel)

if __name__ == "__main__":
    main()