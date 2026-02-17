#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

// --- 1. Enums для идентификации типов (чтобы формировать запрос) ---
enum class MatType { BRICK, CONCRETE, WIRE, WOOD };
enum class SpecType { MASON, ELECTRICIAN, CARPENTER };

// --- 2. Классы Материалов ---
class Material {
public:
    virtual std::string getName() const = 0;
    virtual MatType getType() const = 0;
    virtual ~Material() = default;
};

class Brick : public Material {
    std::string getName() const override { return "Кирпич"; }
    MatType getType() const override { return MatType::BRICK; }
};

class Concrete : public Material {
    std::string getName() const override { return "Бетон"; }
    MatType getType() const override { return MatType::CONCRETE; }
};

class Wire : public Material {
    std::string getName() const override { return "Провода"; }
    MatType getType() const override { return MatType::WIRE; }
};

class Wood : public Material {
    std::string getName() const override { return "Дерево"; }
    MatType getType() const override { return MatType::WOOD; }
};

// --- 3. Классы Специалистов ---
class Specialist {
public:
    virtual std::string getJobName() const = 0;
    virtual SpecType getType() const = 0;
    virtual ~Specialist() = default;
};

class Mason : public Specialist { // Каменщик
    std::string getJobName() const override { return "Каменщик"; }
    SpecType getType() const override { return SpecType::MASON; }
};

class Electrician : public Specialist { // Электрик
    std::string getJobName() const override { return "Электрик"; }
    SpecType getType() const override { return SpecType::ELECTRICIAN; }
};

class Carpenter : public Specialist { // Плотник
    std::string getJobName() const override { return "Плотник"; }
    SpecType getType() const override { return SpecType::CARPENTER; }
};

// --- 4. Класс Проверки Исключений (SafetyInspector) ---
class SafetyInspector {
public:
    // Метод проверки совместимости. Если что-то не так — кидает ошибку.
    static void checkSafety(Specialist* worker, Material* mat) {
        SpecType sType = worker->getType();
        MatType mType = mat->getType();

        // Логика: кто с чем может работать
        bool isSafe = false;

        if (sType == SpecType::MASON) {
            if (mType == MatType::BRICK || mType == MatType::CONCRETE) isSafe = true;
        }
        else if (sType == SpecType::ELECTRICIAN) {
            if (mType == MatType::WIRE) isSafe = true;
        }
        else if (sType == SpecType::CARPENTER) {
            if (mType == MatType::WOOD) isSafe = true;
        }

        if (!isSafe) {
            throw std::runtime_error("НАРУШЕНИЕ! " + worker->getJobName() + 
                                     " пытается использовать " + mat->getName());
        }
    }
};

// --- 5. Класс Запроса и Здания ---

// Структура одной задачи в запросе
struct Task {
    SpecType workerType;
    MatType materialType;
};

class ConstructionRequest {
public:
    std::vector<Task> tasks;
    void addTask(SpecType s, MatType m) {
        tasks.push_back({s, m});
    }
};

class Building {
    std::vector<std::string> log;
    bool isCompleted = false;
public:
    void addLog(const std::string& msg) { log.push_back(msg); }
    void setComplete(bool val) { isCompleted = val; }
    
    void printReport(int id) {
        std::cout << "\n--- Отчет по Зданию #" << id << " ---" << std::endl;
        for(const auto& line : log) {
            std::cout << line << std::endl;
        }
        if (isCompleted) std::cout << "СТАТУС: ПОСТРОЕНО УСПЕШНО" << std::endl;
        else std::cout << "СТАТУС: СТРОЙКА ОСТАНОВЛЕНА ИЗ-ЗА ОШИБОК" << std::endl;
        std::cout << "---------------------------" << std::endl;
    }
};

// --- 6. Фабрика (BuildingFactory) ---
class BuildingFactory {
public:
    // Factory Method для материалов
    static std::shared_ptr<Material> createMaterial(MatType type) {
        switch(type) {
            case MatType::BRICK: return std::make_shared<Brick>();
            case MatType::CONCRETE: return std::make_shared<Concrete>();
            case MatType::WIRE: return std::make_shared<Wire>();
            case MatType::WOOD: return std::make_shared<Wood>();
            default: return nullptr;
        }
    }

    // Factory Method для специалистов
    static std::shared_ptr<Specialist> createSpecialist(SpecType type) {
        switch(type) {
            case SpecType::MASON: return std::make_shared<Mason>();
            case SpecType::ELECTRICIAN: return std::make_shared<Electrician>();
            case SpecType::CARPENTER: return std::make_shared<Carpenter>();
            default: return nullptr;
        }
    }

    // Главный метод сборки
    static Building build(const ConstructionRequest& req) {
        Building building;
        
        try {
            for (const auto& task : req.tasks) {
                // 1. Создаем ресурсы через фабричные методы
                auto worker = createSpecialist(task.workerType);
                auto material = createMaterial(task.materialType);

                // 2. Инспектор проверяет безопасность
                SafetyInspector::checkSafety(worker.get(), material.get());

                // 3. Если ошибки не было, "строим"
                building.addLog(worker->getJobName() + " успешно установил " + material->getName());
            }
            // Если дошли до конца без catch
            building.setComplete(true);
        }
        catch (const std::exception& e) {
            // Ловим ошибку от Инспектора
            building.addLog(std::string("ОШИБКА СТРОЙКИ: ") + e.what());
            building.setComplete(false);
        }

        return building;
    }
};

// --- MAIN ---
int main() {
    // Здание 1: Обычный дом (Кирпич + Бетон + Проводка)
    ConstructionRequest req1;
    req1.addTask(SpecType::MASON, MatType::CONCRETE); // Фундамент
    req1.addTask(SpecType::MASON, MatType::BRICK);    // Стены
    req1.addTask(SpecType::ELECTRICIAN, MatType::WIRE); // Свет
    Building b1 = BuildingFactory::build(req1);
    b1.printReport(1);

    // Здание 2: Деревянный домик
    ConstructionRequest req2;
    req2.addTask(SpecType::CARPENTER, MatType::WOOD);
    req2.addTask(SpecType::ELECTRICIAN, MatType::WIRE);
    Building b2 = BuildingFactory::build(req2);
    b2.printReport(2);

    // Здание 3: ОШИБКА - Электрик пытается класть кирпичи
    ConstructionRequest req3;
    req3.addTask(SpecType::MASON, MatType::CONCRETE);
    req3.addTask(SpecType::ELECTRICIAN, MatType::BRICK); // <-- Ошибка здесь
    req3.addTask(SpecType::CARPENTER, MatType::WOOD);    // Сюда даже не дойдет
    Building b3 = BuildingFactory::build(req3);
    b3.printReport(3);

    // Здание 4: ОШИБКА - Плотник работает с бетоном
    ConstructionRequest req4;
    req4.addTask(SpecType::CARPENTER, MatType::CONCRETE); // <-- Ошибка сразу
    Building b4 = BuildingFactory::build(req4);
    b4.printReport(4);

    // Здание 5: Сложное здание (смешанное), все верно
    ConstructionRequest req5;
    req5.addTask(SpecType::MASON, MatType::CONCRETE);
    req5.addTask(SpecType::MASON, MatType::BRICK);
    req5.addTask(SpecType::CARPENTER, MatType::WOOD); // Крыша
    req5.addTask(SpecType::ELECTRICIAN, MatType::WIRE);
    Building b5 = BuildingFactory::build(req5);
    b5.printReport(5);

    return 0;
}