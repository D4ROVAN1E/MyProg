#include <iostream>
#include <vector>
#include <set>
#include <string>

using namespace std;

enum class Action {
    SERVER,
    CLIENT,
    DB,
    UI,
    ART,
    TEST
};

string Doing(Action act) {
    switch (act)
    {
    case Action::SERVER: return " doing server  "; break;
    case Action::CLIENT: return " doing client  "; break;
    case Action::DB: return " doing data base  "; break;
    case Action::UI: return " doing ui  "; break;
    case Action::ART: return " doing art  "; break;
    case Action::TEST: return " doing test  "; break;
    default: return " unknown action  "; break;
    }
} 

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(string message) {
        cout << "[LOG]: " << message << endl;
    }

    void error(string message) {
        cerr << "[ERR]: " << message << endl;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() {};
};

class Developer {
public:
    string name;
    set<Action> abilities;

    Developer(string n, set<Action> abs) : name(n), abilities(abs) {};
    virtual ~Developer() = default;

    virtual void Act() = 0;

    bool canAct(Action act) {
        return abilities.count(act) > 0;
    }

};

class Backend : public Developer {
public:
    Backend(string nm) : Developer(nm, {Action::SERVER, Action::DB}) {}
    void Act() override {
        cout << "Backend ";
    }
};

class Frontend : public Developer {
public:
    Frontend(string nm) : Developer(nm, {Action::CLIENT, Action::UI}) {}
    void Act() override {
        cout << "Frontend ";
    }
};

class Disign : public Developer {
public:
    Disign(string nm) : Developer(nm, {Action::ART, Action::UI}) {}
    void Act() override {
        cout << "Disign ";
    }
};

class Tester : public Developer {
public:
    Tester(string nm) : Developer(nm, {Action::TEST}) {}
    void Act() override {
        cout << "Tester ";
    }
};

class FullStack : public Developer {
public:
    FullStack(string nm) : Developer(nm, {Action::SERVER, Action::CLIENT, Action::DB}) {}
    void Act() override {
        cout << "FullStack ";
    }
};

class ProjectMaker {
private:
    vector<Developer*> team;
    Logger& logger;
public:
    ProjectMaker() : logger(Logger::getInstance()) {}

    void AddMember(Developer* member, Action act) {
        if (member->canAct(act)) {
            team.push_back(member);
            logger.log("Member " + member->name + " was added to team to " + Doing(act));
        } else {
            logger.error("Sorry, member " + member->name + " can't " + Doing(act));
        }
    }

    void FinelizeProject() {
        bool DidServer = false;
        bool DidUI = false;
        bool DidTests = false;

        for(auto member : team) {
            if (member->canAct(Action::SERVER)) DidServer = true;
            if (member->canAct(Action::UI)) DidUI = true;
            if (member->canAct(Action::TEST)) DidTests = true;
        }

        if (DidServer && DidTests && DidUI) {
            logger.log("Project Succsesfully finished!");
        } else {
            logger.error("Can't finishing project. Team capabilities isn't enough");
        }
    }
};

int main() {
    Backend LaryBack("Lary");
    Frontend MicleFront("Micle");
    Disign MayaDesign("Maya");
    Tester IvanTest("Ivan");
    ProjectMaker firstApp;

    firstApp.AddMember(&LaryBack, Action::SERVER);
    firstApp.AddMember(&MicleFront, Action::CLIENT);
    firstApp.AddMember(&MayaDesign, Action::TEST);
    firstApp.AddMember(&MayaDesign, Action::UI);

    firstApp.FinelizeProject();

    firstApp.AddMember(&IvanTest, Action::TEST);

    firstApp.FinelizeProject();
    return 0;
}