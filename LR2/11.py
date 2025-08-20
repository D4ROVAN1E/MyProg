try:
    # Чтение и разбиение входных данных на части
    parts = input().split()
    if len(parts) != 3:
        print("Invalid input")
        exit()
    N_str, M_str, S = parts
    N = int(N_str)
    M = int(M_str)
except ValueError:
    # Обработка ошибок преобразования строк в числа
    print("Invalid input")
    exit()

# Проверка корректности размеров доски
if N < 1 or M < 1:
    print("Invalid input")
    exit()

# Проверка допустимости символов в строке команд
for c in S:
    if c not in {'L', 'R', 'U', 'D'}:
        print("Invalid input")
        exit()

# Инициализация минимальных координат и счетчиков команд
min_x = 1
min_y = 1
L = 0
R = 0
U = 0
D = 0

# Обработка каждой команды с динамическим обновлением координат
for c in S:
    if c == 'L':
        L += 1
        if min_x - L < 1:
            min_x += 1
        if R > 0:
            R -= 1
    elif c == 'R':
        R += 1
        if L > 0:
            L -= 1
    elif c == 'U':
        U += 1
        if min_y - U < 1:
            min_y += 1
        if D > 0:
            D -= 1
    elif c == 'D':
        D += 1
        if U > 0:
            U -= 1

# Проверка существования допустимой начальной позиции
if min_x > M or min_y > N:
    print("No")
else:
    print(f"({min_x},{min_y})")