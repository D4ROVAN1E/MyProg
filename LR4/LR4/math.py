import math # type: ignore
import locale

# Исходная функция f(x) = cos(x) - (x-1)²
def f(x):
    return math.cos(x) - (x - 1) ** 2

# Первая производная f'(x) = -sin(x) - 2(x-1)
def df(x):
    return -math.sin(x) - 2 * (x - 1)

# Вторая производная f''(x) = -cos(x) - 2
def d2f(x):
    return -math.cos(x) - 2

# Функция для графического отделения корней
def find_root_intervals(start, end, step):
    print("\n=== ГРАФИЧЕСКОЕ ОТДЕЛЕНИЕ КОРНЕЙ ===")
    print("Поиск интервалов, где f(x) меняет знак:")
    print(f"{'x':>8} {'f(x)':>15} {'Знак':>10}")
    print("-" * 33)
    
    intervals = []
    x = start
    prev_x = None
    prev_fx = None
    
    while x <= end:
        fx = f(x)
        sign = "+" if fx > 0 else ("-" if fx < 0 else "0")
        print(f"{x:>8.2f} {fx:>15.6f} {sign:>10}")
        
        if prev_x is not None and prev_fx * fx < 0:
            intervals.append((prev_x, x))
            print(f">>> НАЙДЕН КОРЕНЬ НА ИНТЕРВАЛЕ [{prev_x:.2f}, {x:.2f}]")
        
        prev_x = x
        prev_fx = fx
        x += step
    
    return intervals

# Метод половинного деления
def bisection_method(a, b, eps):
    print("\n=== МЕТОД ПОЛОВИННОГО ДЕЛЕНИЯ ===")
    print(f"Начальный интервал: [{a}, {b}]")
    print(f"Точность: {eps}\n")
    
    print(f"{'N':>5} {'a_n':>12} {'b_n':>12} {'c_n':>12} {'f(c_n)':>12} {'b_n - a_n':>12}")
    print("-" * 65)
    
    table = []
    n = 0
    
    while abs(b - a) > eps:
        c = (a + b) / 2.0
        fc = f(c)
        interval_length = b - a
        
        print(f"{n:>5} {a:>12.6f} {b:>12.6f} {c:>12.6f} {fc:>12.6f} {interval_length:>12.6f}")
        table.append([n, a, b, c, fc, interval_length])
        
        if f(a) * fc < 0:
            b = c
        else:
            a = c
        n += 1
    
    result = (a + b) / 2.0
    print(f"\nРезультат: x* = {result:.8f}")
    print(f"Проверка: f(x*) = {f(result):.8f}")
    print(f"Количество итераций: {n}")
    
    return result, table

# Метод Ньютона
def newton_method(x0, eps):
    print("\n=== МЕТОД НЬЮТОНА ===")
    print(f"Начальное приближение: x0 = {x0}")
    print(f"Точность: {eps}")
    
    # Проверка условия выбора начального приближения
    fx0 = f(x0)
    d2fx0 = d2f(x0)
    condition = fx0 * d2fx0
    condition_ok = "✓" if condition > 0 else "✗"
    print(f"Проверка условия f(x0)*f''(x0) > 0: {fx0:.6f} * {d2fx0:.6f} = {condition:.6f} {condition_ok}\n")
    
    print(f"{'N':>5} {'x_n':>15} {'f(x_n)':>15} {'f\'(x_n)':>15} {'x_{n+1}':>15} {'|x_{n+1} - x_n|':>15}")
    print("-" * 80)
    
    table = []
    x = x0
    n = 0
    
    while True:
        fx = f(x)
        dfx = df(x)
        
        if abs(dfx) < 1e-12:
            print("Производная близка к нулю! Метод может не сходиться.")
            break
        
        x_new = x - fx / dfx
        diff = abs(x_new - x)
        
        print(f"{n:>5} {x:>15.8f} {fx:>15.8f} {dfx:>15.8f} {x_new:>15.8f} {diff:>15.8f}")
        table.append([n, x, fx, dfx, x_new, diff])
        
        if diff < eps:
            print(f"\nРезультат: x* = {x_new:.8f}")
            print(f"Проверка: f(x*) = {f(x_new):.8f}")
            print(f"Количество итераций: {n + 1}")
            return x_new, table
        
        x = x_new
        n += 1
        
        if n > 100:
            print("Превышено максимальное количество итераций!")
            break
    
    return x, table

# Функция итерации (адаптивная)
def phi_adaptive(x):
    fx = f(x)
    dfx = df(x)
    
    # Адаптивный выбор коэффициента α
    if abs(dfx) > 1e-12:
        alpha = 1.0 / abs(dfx)
        alpha = min(alpha, 0.5)
        alpha = max(alpha, 0.1)
    else:
        alpha = 0.3
    
    return x - alpha * fx

# Метод простых итераций
def simple_iteration_method(x0, eps):
    print("\n=== МЕТОД ПРОСТЫХ ИТЕРАЦИЙ ===")
    print(f"Начальное приближение: x0 = {x0}")
    print("Функция итерации: φ(x) = x - α(x)·f(x)")
    print(f"Точность: {eps}\n")
    
    print(f"{'N':>5} {'x_n':>15} {'f(x_n)':>15} {'α_n':>15} {'x_{{n+1}}':>15} {'|x_{{n+1}} - x_n|':>15}")
    print("-" * 85)
    
    table = []
    x = x0
    n = 0
    
    while True:
        fx = f(x)
        dfx = df(x)
        
        # Вычисляем адаптивный коэффициент
        if abs(dfx) > 1e-12:
            alpha = 1.0 / abs(dfx)
            alpha = min(alpha, 0.5)
            alpha = max(alpha, 0.1)
        else:
            alpha = 0.3
        
        x_new = phi_adaptive(x)
        diff = abs(x_new - x)
        
        print(f"{n:>5} {x:>15.8f} {fx:>15.8f} {alpha:>15.8f} {x_new:>15.8f} {diff:>15.8f}")
        table.append([n, x, fx, alpha, x_new, diff])
        
        if diff < eps:
            print(f"\nРезультат: x* = {x_new:.8f}")
            print(f"Проверка: f(x*) = {f(x_new):.8f}")
            print(f"Количество итераций: {n + 1}")
            return x_new, table
        
        x = x_new
        n += 1
        
        if n > 100:
            print("\nПревышено максимальное количество итераций!")
            break
    
    return x, table

def main():
    try:
        locale.setlocale(locale.LC_ALL, 'ru_RU.UTF-8')
    except:
        pass  # Если русская локаль недоступна, продолжаем без неё
    
    print("Решение нелинейного уравнения: cos(x) - (x-1)² = 0")
    print("=" * 60)
    
    eps = 1e-4
    
    # Графическое отделение корней
    root_intervals = find_root_intervals(-1.0, 3.0, 0.1)
    
    print("\nНайденные интервалы с корнями:")
    for interval in root_intervals:
        print(f"[{interval[0]:.1f}, {interval[1]:.1f}]")
    
    # Обработка каждого найденного интервала
    all_roots = []
    
    for interval in root_intervals:
        a, b = interval
        
        print("\n" + "=" * 70)
        print(f"ОБРАБОТКА ИНТЕРВАЛА [{a:.1f}, {b:.1f}]")
        print("=" * 70)
        
        # Метод половинного деления
        root_bisection, bisection_table = bisection_method(a, b, eps)
        all_roots.append(root_bisection)
        
        # Выбор начального приближения для метода Ньютона
        x0_newton = a if f(a) * d2f(a) > 0 else b
        
        # Метод Ньютона
        root_newton, newton_table = newton_method(x0_newton, eps)
        
        # Метод простых итераций с лучшим начальным приближением
        x0_iteration = root_bisection  # Используем результат метода деления
        root_iteration, iteration_table = simple_iteration_method(x0_iteration, eps)
        
        # Сравнение результатов
        print("\n=== СРАВНЕНИЕ МЕТОДОВ ===")
        print(f"Метод половинного деления: x* = {root_bisection:.8f}, итераций: {len(bisection_table)}")
        print(f"Метод Ньютона:            x* = {root_newton:.8f}, итераций: {len(newton_table)}")
        print(f"Метод простых итераций:   x* = {root_iteration:.8f}, итераций: {len(iteration_table)}")
        
        print("\nАнализ точности:")
        print(f"f(x_bisection) = {f(root_bisection):.2e}")
        print(f"f(x_newton)    = {f(root_newton):.2e}")
        print(f"f(x_iteration) = {f(root_iteration):.2e}")
    
    # Итоговые результаты
    print("\n" + "=" * 50)
    print("ИТОГОВЫЕ РЕЗУЛЬТАТЫ")
    print("=" * 50)
    print("Все найденные корни уравнения cos(x) - (x-1)² = 0:")
    
    for i, root in enumerate(all_roots, 1):
        print(f"x{i} = {root:.8f} (проверка: f(x) = {f(root):.2e})")

if __name__ == "__main__":
    main() # type: ignore