#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <memory>

// --- Типы действий (роли), которые могут выполнять музыканты ---
enum class Action {
    DRUMMING,
    RHYTHM,
    SOLO,
    VOCAL,
    BASS_PLAYING
};

// Функция для красивого вывода enum
std::string actionToString(Action a) {
    switch(a) {
        case Action::DRUMMING: return "играет на барабанах";
        case Action::RHYTHM: return "играет ритм-партию";
        case Action::SOLO: return "играет соло";
        case Action::VOCAL: return "поет";
        case Action::BASS_PLAYING: return "играет на басу";
        default: return "неизвестное действие";
    }
}

// --- 1. Класс Ошибок / Логгер (Singleton Scott Meyers) ---
class Logger {
public:
    // Единственный способ получить доступ к экземпляру
    static Logger& getInstance() {
        static Logger instance; // Инициализируется при первом вызове
        return instance;
    }

    void log(const std::string& msg) {
        std::cout << "[LOG]: " << msg << std::endl;
    }

    void error(const std::string& msg) {
        std::cerr << "[ERROR]: " << msg << std::endl;
    }

    // Удаляем методы копирования и присваивания
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() {} // Приватный конструктор
};

// --- 2. Базовый класс Музыканта ---
class Musician {
protected:
    std::string name;
    std::set<Action> capabilities; // Набор того, что музыкант умеет

public:
    Musician(std::string n) : name(n) {}
    virtual ~Musician() = default;

    std::string getName() const { return name; }

    // Проверка: умеет ли музыкант выполнять это действие?
    bool canPerform(Action action) const {
        return capabilities.count(action) > 0;
    }

    // Полиморфный метод (хотя в данной задаче логика ушла в canPerform)
    virtual std::string getType() const = 0;
};

// --- Реализация конкретных музыкантов ---

class Drummer : public Musician {
public:
    Drummer(std::string n) : Musician(n) {
        capabilities.insert(Action::DRUMMING);
    }
    std::string getType() const override { return "Барабанщик"; }
};

class Bassist : public Musician {
public:
    Bassist(std::string n) : Musician(n) {
        capabilities.insert(Action::BASS_PLAYING);
    }
    std::string getType() const override { return "Басист"; }
};

class RhythmGuitarist : public Musician {
public:
    RhythmGuitarist(std::string n) : Musician(n) {
        capabilities.insert(Action::RHYTHM);
        capabilities.insert(Action::SOLO);
    }
    std::string getType() const override { return "Ритм-гитарист"; }
};

class LeadGuitarist : public Musician { // Соло
public:
    LeadGuitarist(std::string n) : Musician(n) {
        capabilities.insert(Action::SOLO);
    }
    std::string getType() const override { return "Соло-гитарист"; }
};

class Vocalist : public Musician {
public:
    Vocalist(std::string n) : Musician(n) {
        capabilities.insert(Action::RHYTHM);
        capabilities.insert(Action::VOCAL);
    }
    std::string getType() const override { return "Вокалист"; }
};

// --- 3. Класс Компоновщика (Composer) ---
class Composer {
    // Структура для хранения участника записи: Музыкант + Роль, которую он исполняет
    struct SessionMember {
        Musician* musician;
        Action role;
    };

    std::vector<SessionMember> currentSession;
    Logger& logger;

public:
    Composer() : logger(Logger::getInstance()) {}

    void startNewTrack(const std::string& trackName) {
        currentSession.clear();
        logger.log("--- Начало создания трека: " + trackName + " ---");
    }

    // Добавление музыканта на конкретную роль
    void addMusician(Musician* m, Action role) {
        logger.log("Попытка добавить " + m->getType() + " (" + m->getName() + ") на роль: " + actionToString(role));
        
        // Проверка: может ли этот музыкант выполнять эту роль?
        // (Например, чтобы гитарист не барабанил)
        if (m->canPerform(role)) {
            currentSession.push_back({m, role});
            logger.log("Успех: " + m->getName() + " записан.");
        } else {
            logger.error("ОШИБКА: " + m->getType() + " не умеет выполнять действие: " + actionToString(role));
        }
    }

    // Финализация трека (проверка обязательных требований)
    bool finalizeTrack() {
        bool hasDrummer = false;
        bool hasRhythm = false;
        bool hasBass = false;

        for (const auto& member : currentSession) {
            if (member.role == Action::DRUMMING) hasDrummer = true;
            if (member.role == Action::RHYTHM) hasRhythm = true;
            if (member.role == Action::BASS_PLAYING) hasBass = true;
        }

        if (hasDrummer && hasRhythm && hasBass) {
            logger.log("Трек успешно сведен и готов к выпуску!");
            return true;
        } else {
            logger.error("ОШИБКА: Трек не готов. Обязательный состав (Барабаны + Ритм + Бас) не соблюден.");
            if (!hasDrummer) logger.error(" -> Нет барабанщика.");
            if (!hasRhythm) logger.error(" -> Нет ритм-секции.");
            if (!hasBass) logger.error(" -> Нет баса.");
            return false;
        }
    }
};

// --- MAIN ---
int main() {
    // Подготовка музыкантов
    Drummer dr("Lars");
    Bassist bs("Flea");
    RhythmGuitarist rg("James");
    LeadGuitarist lg("Kirk");
    Vocalist vo("Chester");

    Composer composer;

    // --- Трек 1: Идеальный состав ---
    composer.startNewTrack("Master of Puppets");
    composer.addMusician(&dr, Action::DRUMMING);
    composer.addMusician(&bs, Action::BASS_PLAYING);
    composer.addMusician(&rg, Action::RHYTHM);
    composer.addMusician(&lg, Action::SOLO);
    composer.finalizeTrack();
    std::cout << std::endl;

    // --- Трек 2: Ошибка - Гитарист пытается барабанить ---
    composer.startNewTrack("Weird Experiment");
    composer.addMusician(&rg, Action::DRUMMING); // Ошибка
    composer.addMusician(&bs, Action::BASS_PLAYING);
    composer.addMusician(&rg, Action::RHYTHM);
    composer.finalizeTrack(); // Ошибка: нет барабанщика
    std::cout << std::endl;

    // --- Трек 3: Минимальный состав (Барабан + Бас + Вокалист на ритме) ---
    // Вокалист умеет и петь, и играть ритм, используем это.
    composer.startNewTrack("Minimalistic Song");
    composer.addMusician(&dr, Action::DRUMMING);
    composer.addMusician(&bs, Action::BASS_PLAYING);
    composer.addMusician(&vo, Action::RHYTHM); // Вокалист закрывает требование по ритму
    composer.addMusician(&vo, Action::VOCAL);
    composer.finalizeTrack();
    std::cout << std::endl;

    // --- Трек 4: Ошибка - Нет баса ---
    composer.startNewTrack("No Bass Song");
    composer.addMusician(&dr, Action::DRUMMING);
    composer.addMusician(&rg, Action::RHYTHM);
    composer.finalizeTrack();
    std::cout << std::endl;

    // --- Трек 5: Все вместе ---
    composer.startNewTrack("Symphony");
    composer.addMusician(&dr, Action::DRUMMING);
    composer.addMusician(&bs, Action::BASS_PLAYING);
    composer.addMusician(&rg, Action::RHYTHM);
    composer.addMusician(&rg, Action::SOLO); // Ритм гитарист может играть соло
    composer.addMusician(&vo, Action::VOCAL);
    composer.finalizeTrack();

    return 0;
}