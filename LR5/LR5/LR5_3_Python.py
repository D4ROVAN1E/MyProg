from enum import Enum
from typing import List, Dict, Set
import sys


class Command(Enum):
    CREATE_PLANE = "CREATE_PLANE"
    PLANES_FOR_TOWN = "PLANES_FOR_TOWN"
    TOWNS_FOR_PLANE = "TOWNS_FOR_PLANE"
    PLANES = "PLANES"
    INVALID = "INVALID"


class Plane:
    def __init__(self, name: str, towns: List[str]):
        self.name = name
        self.towns = towns


class PlaneSystem:
    def __init__(self):
        self.planes: List[Plane] = []
        self.town_to_planes: Dict[str, Set[str]] = {}
        self.plane_names: Set[str] = set()
    
    def add_plane(self, plane_name: str, towns: List[str]) -> bool:
        # Проверяем уникальность имени самолета
        if plane_name in self.plane_names:
            print(f"Ошибка: Самолет с именем '{plane_name}' уже существует!")
            return False
        
        # Проверяем на дублирование городов в маршруте
        unique_towns = set()
        for town in towns:
            if town in unique_towns:
                print(f"Ошибка: Город '{town}' повторяется в маршруте!")
                return False
            unique_towns.add(town)
        
        # Добавляем самолет
        self.planes.append(Plane(plane_name, towns))
        self.plane_names.add(plane_name)
        
        # Обновляем индекс городов к самолетам
        for town in towns:
            if town not in self.town_to_planes:
                self.town_to_planes[town] = set()
            self.town_to_planes[town].add(plane_name)
        
        return True
    
    def show_planes_for_town(self, town: str):
        if town in self.town_to_planes:
            planes = ' '.join(self.town_to_planes[town])
            print(f"Самолеты, пролетающие через {town}: {planes}")
        else:
            print(f"Через город {town} не пролетает ни один самолет.")
    
    def show_towns_for_plane(self, plane_name: str):
        plane = None
        for p in self.planes:
            if p.name == plane_name:
                plane = p
                break
        
        if plane:
            print(f"Города для самолета {plane_name}:")
            for town in plane.towns:
                other_planes = []
                if town in self.town_to_planes:
                    for other_plane in self.town_to_planes[town]:
                        if other_plane != plane_name:
                            other_planes.append(other_plane)
                
                if other_planes:
                    other_planes_str = ', '.join(other_planes)
                else:
                    other_planes_str = "нет"
                
                print(f"  {town} (другие самолеты: {other_planes_str})")
        else:
            print(f"Самолет {plane_name} не найден.")
    
    def show_all_planes(self):
        print("Все самолеты:")
        for plane in self.planes:
            towns_str = ' '.join(plane.towns)
            print(f"{plane.name}: {towns_str}")


def parse_command(cmd: str) -> Command:
    try:
        return Command(cmd)
    except ValueError:
        return Command.INVALID


def split_string(input_str: str) -> List[str]:
    return input_str.split()


def process_command(system: PlaneSystem, input_str: str):
    tokens = split_string(input_str)
    
    if not tokens:
        print("Пустая команда.")
        return
    
    cmd = parse_command(tokens[0])
    
    if cmd == Command.CREATE_PLANE:
        if len(tokens) < 3:
            print("Неверный формат команды CREATE_PLANE. Использование: CREATE_PLANE plane_name town1 town2 ...")
            return
        
        plane_name = tokens[1]
        towns = tokens[2:]
        
        if system.add_plane(plane_name, towns):
            towns_str = ' '.join(towns)
            print(f"Самолет {plane_name} создан с маршрутом через {towns_str}")
    
    elif cmd == Command.PLANES_FOR_TOWN:
        if len(tokens) != 2:
            print("Неверный формат команды PLANES_FOR_TOWN. Использование: PLANES_FOR_TOWN town_name")
            return
        
        system.show_planes_for_town(tokens[1])
    
    elif cmd == Command.TOWNS_FOR_PLANE:
        if len(tokens) != 2:
            print("Неверный формат команды TOWNS_FOR_PLANE. Использование: TOWNS_FOR_PLANE plane_name")
            return
        
        system.show_towns_for_plane(tokens[1])
    
    elif cmd == Command.PLANES:
        system.show_all_planes()
    
    elif cmd == Command.INVALID:
        print(f"Неизвестная команда: {tokens[0]}")
        print("Доступные команды:")
        print("  CREATE_PLANE plane_name town1 town2 ...")
        print("  PLANES_FOR_TOWN town_name")
        print("  TOWNS_FOR_PLANE plane_name")
        print("  PLANES")


def main():
    system = PlaneSystem()
    
    print("=== Система управления графиком движения самолетов ===")
    print("Доступные команды:")
    print("  CREATE_PLANE plane_name town1 town2 ... - создать самолет с маршрутом")
    print("  PLANES_FOR_TOWN town_name - показать самолеты для города")
    print("  TOWNS_FOR_PLANE plane_name - показать города для самолета")
    print("  PLANES - показать все самолеты")
    print("  exit - выход из программы")
    print("========================================================")
    
    while True:
        try:
            input_str = input("\nВведите команду: ")
            
            if input_str.lower() in ["exit", "EXIT"]:
                print("Завершение работы программы.")
                break
            
            if not input_str.strip():
                continue
            
            process_command(system, input_str)
            
        except KeyboardInterrupt:
            print("\nЗавершение работы программы.")
            break
        except EOFError:
            print("\nЗавершение работы программы.")
            break


if __name__ == "__main__":
    main()