import pygame
import math
import sys

class Point:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

class SierpinskiTriangle:
    def __init__(self, width=800, height=600, depth=5):
        pygame.init()
        self.width = width
        self.height = height
        self.screen = pygame.display.set_mode((width, height))
        pygame.display.set_caption("Треугольник Серпинского")
        self.clock = pygame.time.Clock()
        
        self.triangles = []
        self.max_depth = depth
        
        # Цвета
        self.BLACK = (0, 0, 0)
        self.WHITE = (255, 255, 255)
        
    def get_midpoint(self, a, b):
        """Функция для получения средней точки между двумя точками"""
        return Point((a.x + b.x) / 2.0, (a.y + b.y) / 2.0)
    
    def draw_sierpinski(self, a, b, c, depth):
        """Рекурсивная функция для создания треугольника Серпинского"""
        if depth == 0:
            # Добавляем треугольник в список для отрисовки
            triangle = [(a.x, a.y), (b.x, b.y), (c.x, c.y)]
            self.triangles.append(triangle)
        else:
            # Находим средние точки сторон
            ab = self.get_midpoint(a, b)
            bc = self.get_midpoint(b, c)
            ca = self.get_midpoint(c, a)
            
            # Рекурсивно рисуем три меньших треугольника
            self.draw_sierpinski(a, ab, ca, depth - 1)
            self.draw_sierpinski(ab, b, bc, depth - 1)
            self.draw_sierpinski(ca, bc, c, depth - 1)
    
    def generate_triangle(self):
        """Генерация треугольника Серпинского"""
        self.triangles.clear()
        
        # Создаем начальный равносторонний треугольник
        center_x = 400
        center_y = 300
        size = 250
        
        # Вершины равностороннего треугольника
        a = Point(center_x, center_y - size)
        b = Point(center_x - size * math.cos(math.pi / 6), 
                 center_y + size * math.sin(math.pi / 6))
        c = Point(center_x + size * math.cos(math.pi / 6), 
                 center_y + size * math.sin(math.pi / 6))
        
        self.draw_sierpinski(a, b, c, self.max_depth)
    
    def handle_events(self):
        """Обработка событий"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False
            
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP and self.max_depth < 8:
                    self.max_depth += 1
                    self.generate_triangle()
                    print(f"Глубина увеличена до: {self.max_depth}")
                elif event.key == pygame.K_DOWN and self.max_depth > 0:
                    self.max_depth -= 1
                    self.generate_triangle()
                    print(f"Глубина уменьшена до: {self.max_depth}")
                elif event.key == pygame.K_r:
                    self.generate_triangle()
                    print("Треугольник перерисован")
        
        return True
    
    def render(self):
        """Отрисовка"""
        self.screen.fill(self.BLACK)
        
        # Рисуем все треугольники
        for triangle in self.triangles:
            pygame.draw.polygon(self.screen, self.WHITE, triangle)
        
        pygame.display.flip()
    
    def run(self):
        """Главный цикл программы"""
        print("Треугольник Серпинского")
        print("Управление:")
        print("Стрелка вверх - увеличить глубину рекурсии")
        print("Стрелка вниз - уменьшить глубину рекурсии")
        print("R - перерисовать треугольник")
        print(f"Текущая глубина: {self.max_depth}")
        
        self.generate_triangle()
        
        running = True
        while running:
            running = self.handle_events()
            self.render()
            self.clock.tick(60)  # 60 FPS
        
        pygame.quit()
        sys.exit()

def main():
    try:
        sierpinski = SierpinskiTriangle(800, 600, 3)
        sierpinski.run()
    except Exception as e:
        print(f"Ошибка: {e}")
        return -1
    
    return 0

if __name__ == "__main__":
    main()