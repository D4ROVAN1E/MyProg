import random
import sys

class Visitor:
    """Класс для хранения информации о посетителе"""
    def __init__(self, ticket, duration):
        self.ticket = ticket
        self.duration = duration

class Window:
    """Класс для хранения информации об окне обслуживания"""
    def __init__(self, window_id):
        self.id = window_id
        self.total_time = 0
        self.tickets = []

def generate_ticket():
    """Функция для генерации случайного номера билета"""
    number = random.randint(0, 999)
    return f"T{number:03d}"

def make_windows(num_windows, windows):
    """Функция для создания окон обслуживания"""
    for i in range(1, num_windows + 1):
        windows.append(Window(i))

def enqueue(duration, visitors):
    """Функция для добавления нового посетителя в очередь"""
    ticket = generate_ticket()
    visitors.append(Visitor(ticket, duration))
    return ticket

def distribute(visitors, windows):
    """Функция для распределения посетителей по окнам"""
    # Создаем копию посетителей для сортировки
    sorted_visitors = sorted(visitors, key=lambda x: x.duration, reverse=True)
    
    # Распределяем посетителей по окнам
    for visitor in sorted_visitors:
        # Находим окно с минимальным временем
        min_window = min(windows, key=lambda x: x.total_time)
        
        min_window.tickets.append(visitor.ticket)
        min_window.total_time += visitor.duration
    
    # Выводим результат
    for window in windows:
        tickets_str = ", ".join(window.tickets)
        print(f">>> Окно {window.id} ({window.total_time} минут): {tickets_str}")

def is_valid_num(num_str):
    """Функция для валидации числа"""
    if not num_str:
        raise ValueError("Введенная строка пуста")
    
    if not num_str.isdigit():
        raise ValueError("Введенное значение не является числом")
    
    num = int(num_str)
    if num < 0:
        raise ValueError("Количество окон должно быть положительным")
    
    return num

def main():
    """Основная функция программы"""
    visitors = []  # Список для хранения посетителей
    windows = []   # Список для хранения окон
    
    try:
        print(">>> Введите кол-во окон")
        num_windows_str = input().strip()
        num_windows = is_valid_num(num_windows_str)
        make_windows(num_windows, windows)
        
        # Основной цикл обработки команд
        while True:
            try:
                command = input().strip()
                
                if command.startswith("ENQUEUE"):
                    parts = command.split()
                    if len(parts) != 2:
                        print("Неверный формат команды ENQUEUE. Используйте: ENQUEUE <продолжительность>")
                        continue
                    
                    try:
                        duration = int(parts[1])
                        ticket = enqueue(duration, visitors)
                        print(f">>> {ticket}")
                    except ValueError:
                        print("Продолжительность должна быть числом")
                        continue
                        
                elif command == "DISTRIBUTE":
                    distribute(visitors, windows)
                    break
                else:
                    print(f"Неизвестная команда: {command}")
                    
            except EOFError:
                break
                
    except ValueError as e:
        print(f"Ошибка: {e}", file=sys.stderr)
    except KeyboardInterrupt:
        print("\nПрограмма прервана пользователем")

if __name__ == "__main__":
    main()