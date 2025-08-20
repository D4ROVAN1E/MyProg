from enum import Enum
from collections import defaultdict

class Command(Enum):
    ADD = "ADD"
    REMOVE = "REMOVE"
    INFO = "INFO"
    EXIT = "EXIT"
    UNKNOWN = "UNKNOWN"

def parse_command(cmd):
    """Преобразует строку в команду"""
    try:
        return Command(cmd)
    except ValueError:
        return Command.UNKNOWN

def is_valid_address(address):
    """Проверяет корректность адреса"""
    if len(address) != 4:
        return False
    
    if address[0] not in ['A', 'B']:
        return False
    
    try:
        shelf = int(address[1])
        section = int(address[2])
        level = int(address[3])
        
        return (1 <= shelf <= 6 and 
                1 <= section <= 4 and 
                1 <= level <= 4)
    except ValueError:
        return False

def get_fill_percentage(total, max_capacity):
    """Вычисляет процент заполнения"""
    return (total / max_capacity) * 100

def print_warehouse_info(warehouse, cell_totals, total_items):
    """Выводит информацию о складе"""
    # Общая информация
    print(f"Общая загрузка склада: {get_fill_percentage(total_items, 1920):.2f}%")
    
    # Информация по зонам
    for zone in ['A', 'B']:
        zone_items = sum(total for address, total in cell_totals.items() 
                        if address[0] == zone)
        print(f"Загрузка зоны {zone}: {get_fill_percentage(zone_items, 960):.2f}%")
    
    # Заполненные ячейки
    print("\nЗаполненные ячейки:")
    for address, products in warehouse.items():
        if cell_totals[address] > 0:
            print(f"Ячейка {address}: ", end="")
            for product, quantity in products.items():
                print(f"{product} ({quantity}), ", end="")
            print()
    
    # Пустые ячейки
    print("\nПустые ячейки:")
    for zone in ['A', 'B']:
        for shelf in range(1, 7):
            for section in range(1, 5):
                for level in range(1, 5):
                    address = f"{zone}{shelf}{section}{level}"
                    if cell_totals[address] == 0:
                        print(address, end=" ")
    print()

def main():
    # Хранение товаров в ячейках: адрес -> {товар: количество}
    warehouse = defaultdict(lambda: defaultdict(int))
    
    # Хранение общего количества товаров в ячейках: адрес -> количество
    cell_totals = defaultdict(int)
    
    # Общее количество товаров на складе
    total_items = 0
    
    while True:
        try:
            command_line = input("Введите команду (ADD/REMOVE/INFO/EXIT): ").strip()
            
            if not command_line:
                print("Ошибка: пустая команда")
                continue
            
            parts = command_line.split()
            cmd = parse_command(parts[0])
            
            if cmd == Command.EXIT:
                break
            
            elif cmd == Command.INFO:
                print_warehouse_info(warehouse, cell_totals, total_items)
            
            elif cmd in [Command.ADD, Command.REMOVE]:
                if len(parts) < 4:
                    print("Ошибка: недостаточно параметров")
                    continue
                
                # Собираем название товара (может содержать пробелы)
                product = " ".join(parts[1:-2])
                
                # Получаем количество и адрес
                try:
                    quantity = int(parts[-2])
                except ValueError:
                    print("Ошибка: некорректное количество")
                    continue
                
                address = parts[-1]
                
                if not is_valid_address(address):
                    print("Ошибка: некорректный адрес")
                    continue
                
                if cmd == Command.ADD:
                    # Проверка на превышение лимита ячейки (максимум 10 единиц)
                    if cell_totals[address] + quantity > 10:
                        print("Ошибка: превышен лимит ячейки (максимум 10 единиц)")
                        continue
                    
                    warehouse[address][product] += quantity
                    cell_totals[address] += quantity
                    total_items += quantity
                    print("Товар успешно добавлен")
                
                else:  # REMOVE
                    # Проверка наличия достаточного количества товара
                    if warehouse[address][product] < quantity:
                        print("Ошибка: недостаточно товара в ячейке")
                        continue
                    
                    warehouse[address][product] -= quantity
                    cell_totals[address] -= quantity
                    total_items -= quantity
                    
                    # Удаление записи о товаре, если его количество стало нулевым
                    if warehouse[address][product] == 0:
                        del warehouse[address][product]
                    
                    print("Товар успешно удален")
            
            else:
                print("Неизвестная команда")
        
        except KeyboardInterrupt:
            print("\nВыход из программы...")
            break
        except EOFError:
            print("\nВыход из программы...")
            break

if __name__ == "__main__":
    main()