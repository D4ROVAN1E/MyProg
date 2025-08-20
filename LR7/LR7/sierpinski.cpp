#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace sf;

struct Point {
    float x, y;
    Point(float x = 0, float y = 0) : x(x), y(y) {}
};

class SierpinskiTriangle {
private:
    RenderWindow window;
    vector<VertexArray> triangles;
    int maxDepth;
    
    // Функция для получения средней точки между двумя точками
    Point getMidpoint(const Point& a, const Point& b) {
        return Point((a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f);
    }
    
    // Рекурсивная функция для создания треугольника Серпинского
    void drawSierpinski(Point a, Point b, Point c, int depth) {
        if (depth == 0) {
            // Рисуем треугольник
            VertexArray triangle(Triangles, 3);
            triangle[0].position = Vector2f(a.x, a.y);
            triangle[1].position = Vector2f(b.x, b.y);
            triangle[2].position = Vector2f(c.x, c.y);
            
            // Устанавливаем цвет в зависимости от глубины
            Color color = Color::White;
            triangle[0].color = color;
            triangle[1].color = color;
            triangle[2].color = color;
            
            triangles.push_back(triangle);
        } else {
            // Находим средние точки сторон
            Point ab = getMidpoint(a, b);
            Point bc = getMidpoint(b, c);
            Point ca = getMidpoint(c, a);
            
            // Рекурсивно рисуем три меньших треугольника
            drawSierpinski(a, ab, ca, depth - 1);
            drawSierpinski(ab, b, bc, depth - 1);
            drawSierpinski(ca, bc, c, depth - 1);
        }
    }
    
public:
    SierpinskiTriangle(int width = 800, int height = 600, int depth = 5) 
        : window(VideoMode(width, height), "Треугольник Серпинского"), maxDepth(depth) {
        window.setFramerateLimit(60);
    }
    
    void generateTriangle() {
        triangles.clear();
        
        // Создаем начальный равносторонний треугольник
        float centerX = 400;
        float centerY = 300;
        float size = 250;
        
        // Вершины равностороннего треугольника
        Point a(centerX, centerY - size);
        Point b(centerX - size * cos(M_PI / 6), centerY + size * sin(M_PI / 6));
        Point c(centerX + size * cos(M_PI / 6), centerY + size * sin(M_PI / 6));
        
        drawSierpinski(a, b, c, maxDepth);
    }
    
    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Up && maxDepth < 8) {
                    maxDepth++;
                    generateTriangle();
                    cout << "Глубина увеличена до: " << maxDepth << endl;
                }
                if (event.key.code == Keyboard::Down && maxDepth > 0) {
                    maxDepth--;
                    generateTriangle();
                    cout << "Глубина уменьшена до: " << maxDepth << endl;
                }
                if (event.key.code == Keyboard::R) {
                    generateTriangle();
                    cout << "Треугольник перерисован" << endl;
                }
            }
        }
    }
    
    void render() {
        window.clear(Color::Black);
        
        // Рисуем все треугольники
        for (const auto& triangle : triangles) {
            window.draw(triangle);
        }
        
        // Создаем текст с информацией
        Font font;
        window.display();
    }
    
    void run() {
        cout << "Треугольник Серпинского" << endl;
        cout << "Управление:" << endl;
        cout << "Стрелка вверх - увеличить глубину рекурсии" << endl;
        cout << "Стрелка вниз - уменьшить глубину рекурсии" << endl;
        cout << "R - перерисовать треугольник" << endl;
        cout << "Текущая глубина: " << maxDepth << endl;
        
        generateTriangle();
        
        while (window.isOpen()) {
            handleEvents();
            render();
        }
    }
};

int main() {
    try {
        SierpinskiTriangle sierpinski(800, 600, 3);
        sierpinski.run();
    }
    catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
        return -1;
    }
    
    return 0;
}