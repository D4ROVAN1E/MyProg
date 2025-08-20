def count_points(rings):
    # Инициализируем список из 10 стержней, каждый представлен множеством для хранения цветов
    rods = [set() for _ in range(10)]
    n = len(rings)
    
    # Обрабатываем каждую пару символов (цвет и позиция)
    for i in range(0, n, 2):
        # Проверяем, что пара символов существует (не выходит за пределы строки)
        if i + 1 >= n:
            break
        
        color = rings[i]
        pos_char = rings[i + 1]
        
        # Пропускаем недопустимые цвета
        if color not in {'R', 'B', 'W'}:
            continue
        
        # Пропускаем недопустимые позиции (не цифра или не в диапазоне 0-9)
        if not pos_char.isdigit():
            continue
        
        pos = int(pos_char)
        if 0 <= pos <= 9:
            rods[pos].add(color)  # Добавляем цвет в соответствующий стержень
    
    # Считаем стержни с тремя разными цветами
    return sum(1 for rod in rods if len(rod) == 3)

def main():
    input_str = input().strip()
    print(count_points(input_str))

if __name__ == "__main__":
    main()