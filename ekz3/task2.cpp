#include <iostream>
#include <string>
#include <memory>

// --- 1. Интерфейсы и Абстракции (DIP & ISP) ---

// Интерфейс двигателя (DIP - зависим от абстракции)
class IEngine {
public:
    virtual void Work() = 0;
    virtual ~IEngine() = default;
};

// Конкретные двигатели
class V8Engine : public IEngine {
public:
    void Work() override { std::cout << "V8 Engine roars!\n"; }
};

class ElectricEngine : public IEngine {
public:
    void Work() override { std::cout << "Electric engine hums silently.\n"; }
};

class JetEngine : public IEngine {
public:
    void Work() override { std::cout << "Jet engine thrusting!\n"; }
};

// --- 2. Базовые классы транспорта ---

// Абстрактный транспорт
class Transport {
protected:
    std::string model;
    std::shared_ptr<IEngine> engine; // Агрегация (HAS-A)

public:
    Transport(std::string m, std::shared_ptr<IEngine> e) : model(m), engine(e) {}
    virtual ~Transport() = default;
    
    void StartEngine() {
        std::cout << model << ": ";
        engine->Work();
    }
    
    virtual void Move() = 0; // Чисто виртуальный метод
};

// --- 3. Виды транспорта и Наследники (LSP & OCP) ---

// === Группа 1: Наземный транспорт ===
class Car : public Transport {
public:
    Car(std::string m, std::shared_ptr<IEngine> e) : Transport(m, e) {}
    void Move() override {
        std::cout << model << " is driving on the road.\n";
    }
};

// Наследник Car
class SportCar : public Car {
public:
    SportCar(std::string m, std::shared_ptr<IEngine> e) : Car(m, e) {}
    void Move() override {
        std::cout << model << " is RACING very fast!\n"; // Расширение поведения
    }
};

// === Группа 2: Водный транспорт ===
class Boat : public Transport {
public:
    Boat(std::string m, std::shared_ptr<IEngine> e) : Transport(m, e) {}
    void Move() override {
        std::cout << model << " is sailing on water.\n";
    }
};

// Наследник Boat
class SpeedBoat : public Boat {
public:
    SpeedBoat(std::string m, std::shared_ptr<IEngine> e) : Boat(m, e) {}
    // Логика движения та же, но можно переопределить при желании
};

// === Группа 3: Воздушный транспорт ===
class Airplane : public Transport {
public:
    Airplane(std::string m, std::shared_ptr<IEngine> e) : Transport(m, e) {}
    void Move() override {
        std::cout << model << " is flying in the sky.\n";
    }
};

// Наследник Airplane
class CargoPlane : public Airplane {
public:
    CargoPlane(std::string m, std::shared_ptr<IEngine> e) : Airplane(m, e) {}
    void Move() override {
        std::cout << model << " is flying heavily with cargo.\n";
    }
};

// --- Main для демонстрации ---
int main() {
    // Создаем двигатели
    auto v8 = std::make_shared<V8Engine>();
    auto electric = std::make_shared<ElectricEngine>();
    auto jet = std::make_shared<JetEngine>();

    // 1. Машины
    Car sedan("Toyota Camry", electric);
    SportCar ferrari("Ferrari F40", v8);

    // 2. Лодки
    Boat fishingBoat("Fishing Boat", v8);
    SpeedBoat racer("Yamaha Racer", v8);

    // 3. Самолеты
    Airplane cessna("Cessna 172", v8); // Даже самолет может быть на V8 (теоретически)
    CargoPlane antonov("An-225", jet);

    // Демонстрация (Полиморфизм)
    Transport* vehicles[] = { &sedan, &ferrari, &fishingBoat, &racer, &cessna, &antonov };

    for (auto v : vehicles) {
        v->StartEngine();
        v->Move();
        std::cout << "-----------------\n";
    }

    return 0;
}