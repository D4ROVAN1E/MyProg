#include <iostream>
#include <vector>
#include <memory>

using namespace std;

class Artifact {
public:
    string name;
    int power;


    Artifact(string n, int p) : name(n), power(p) {
        cout << "Artifact " << name << " maded!" << endl;
    }
    ~Artifact() {
        cout << "Artifact " << name << " was destroyed!" << endl;
    }

    void use() {
        cout << "Artifact " << name << " was used with power of " << power << endl;
    }
};

class Hero {
public:
    string HeroName;
    shared_ptr<Artifact> weapon = nullptr;


    Hero(string name, shared_ptr<Artifact> art) : HeroName(name), weapon(art) {};

    void Fight() {
        cout << HeroName << " fighting using " << weapon->name << endl;
        weapon->use();
    }
};

int main() {
    auto TruthSword = make_shared<Artifact>("Sword of thausand truths", 100);
    Hero Araghorn("Araghorn", TruthSword);
    {
        Hero Legolas("Legolas", TruthSword);

        Araghorn.Fight();
        Legolas.weapon->power = 999;
        Araghorn.Fight();
    }
    Araghorn.Fight();
    return 0;
}