#include <iostream>
#include <utility> // Для std::swap

using namespace std;

template <typename T>
struct Node
{
    T key;
    Node<T>* next; // указатель на следующий элемент
};

template <typename T>
struct ForwardList
{
    Node<T>* head;

    // Конструктор по умолчанию
    ForwardList() {
		head = nullptr;
    }

    // Копирующий конструктор (создает глубокую копию)
    ForwardList(const ForwardList& other) {
        head = nullptr;
        if (!other.head) {
            return;
        }
        head = new Node<T>{ other.head->key, nullptr };
        Node<T>* current_this = head;
        Node<T>* current_other = other.head->next;
        while (current_other) {
            current_this->next = new Node<T>{ current_other->key, nullptr };
            current_this = current_this->next;
            current_other = current_other->next;
        }
    }

    // Копирующий оператор присваивания 
    ForwardList& operator=(const ForwardList& other) {
        if (this != &other) {
            ForwardList temp(other);
            swap(head, temp.head);
        }
        return *this;
    }

    // Деструктор (очищает память)
    ~ForwardList() {
        while (head != nullptr) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }
};


// Добавляет узел ПОСЛЕ указанного узла ptr
template <typename T>
void FPUSH_FORWARD(Node<T>* ptr, T key) {
    if (!ptr) return;
    Node<T>* newNode = new Node<T>{ key, nullptr };
    newNode->next = ptr->next;
    ptr->next = newNode;
}

// Создает список с начальным элементом
template <typename T>
void FCREATE(ForwardList<T>& fList, T keyBegin) {
    // Очистим список, если он уже был не пуст
    while (fList.head) {
        Node<T>* temp = fList.head;
        fList.head = fList.head->next;
        delete temp;
    }
    fList.head = new Node<T>{ keyBegin, nullptr };
}


// Выводит список в консоль
template <typename T>
void print(const ForwardList<T>& fList) {
    if (!fList.head) {
        cout << "Список пуст" << endl;
        return;
    }
    Node<T>* current = fList.head;
    while (current != nullptr)
    {
        cout << current->key << " -> ";
        current = current->next;
    }
    cout << "nullptr" << endl;
}

//Добавление элемента в НАЧАЛО списка
template <typename T>
void FPUSH_HEAD(ForwardList<T>& fList, T key) {
    Node<T>* newNode = new Node<T>{ key, nullptr };
    newNode->next = fList.head;
    fList.head = newNode;
}

//Добавление элемента в КОНЕЦ списка
template <typename T>
void FPUSH_BACK(ForwardList<T>& fList, T key) {
    Node<T>* newNode = new Node<T>{ key, nullptr };

    if (fList.head == nullptr) { // Если список пуст
        fList.head = newNode;
        return;
    }

    Node<T>* current = fList.head;
    while (current->next != nullptr) { // Идем до последнего элемента
        current = current->next;
    }
    current->next = newNode; // Последний элемент теперь указывает на новый узел
}

//Добавление элемента ДО узла с заданным значением
template <typename T>
void FPUSH_BEFORE(ForwardList<T>& fList, T targetKey, T newKey) {
    if (!fList.head) return; // Список пуст

    if (fList.head->key == targetKey) {
        FPUSH_HEAD(fList, newKey);
        return;
    }

    Node<T>* current = fList.head;
    while (current->next != nullptr && current->next->key != targetKey) {
        current = current->next;
    }

    if (current->next != nullptr) {
        FPUSH_FORWARD(current, newKey);
    }
}

//Удаление первого элемента (головы) списка
template <typename T>
void FDEL_HEAD(ForwardList<T>& fList) {
    if (!fList.head) return; // Список уже пуст

    Node<T>* temp = fList.head;
    fList.head = fList.head->next;
    delete temp;
}

//Удаление последнего элемента списка
template <typename T>
void FDEL_BACK(ForwardList<T>& fList) {
    if (!fList.head) return;

    if (fList.head->next == nullptr) {
        delete fList.head;
        fList.head = nullptr;
        return;
    }

    Node<T>* current = fList.head;
    while (current->next->next != nullptr) {
        current = current->next;
    }

    delete current->next;
    current->next = nullptr;
}

//Удаление узла по значению (первое вхождение)
template <typename T>
void FDEL_BY_VALUE(ForwardList<T>& fList, T key) {
    if (!fList.head) return;

    if (fList.head->key == key) {
        FDEL_HEAD(fList);
        return;
    }

    Node<T>* current = fList.head;
    while (current->next != nullptr && current->next->key != key) {
        current = current->next;
    }

    if (current->next != nullptr) {
        Node<T>* deleteNode = current->next;
        current->next = deleteNode->next;
        delete deleteNode;
    }
}


//Чтение (поиск) элемента по значению
template <typename T>
Node<T>* FGET_BY_VALUE(ForwardList<T>& fList, T key) {
    Node<T>* current = fList.head;
    while (current != nullptr) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

int main() {
    setlocale(LC_ALL, "ru");
    ForwardList<int> list;

    cout << "Создание списка" << endl;
    FCREATE(list, 10);
    print(list);

    cout << "\nДобавление элементов" << endl;
    FPUSH_BACK(list, 20);
    FPUSH_BACK(list, 30);
    FPUSH_HEAD(list, 5);
    FPUSH_BEFORE(list, 20, 15);
    FPUSH_BEFORE(list, 5, 2);
    print(list);

    cout << "\nПроверка копирования:" << endl;
    ForwardList<int> list2 = list; // Вызов копирующего конструктора
    cout << "Оригинал: ";
    print(list);
    cout << "Копия:    ";
    print(list2);

    cout << "\nУдаляем узел из оригинала:" << endl;
    FDEL_BY_VALUE(list, 15);
    cout << "Оригинал: ";
    print(list);
    cout << "Копия:    ";
    print(list2); // Копия осталась неизменной!

    cout << "\nПроверка присваивания:" << endl;
    ForwardList<int> list3;
    FPUSH_BACK(list3, 99);
    FPUSH_BACK(list3, 100);
    cout << "List3 до присваивания: ";
    print(list3);
    list3 = list; // Вызов оператора присваивания
    cout << "List3 после присваивания: ";
    print(list3);

    cout << "\nВозвращаемся к основному списку и удаляем элементы" << endl;
    FDEL_HEAD(list); // удаляем 2
    print(list);
    FDEL_BACK(list); // удаляем 30
    print(list);
    FDEL_BY_VALUE(list, 20);
    print(list);

    cout << "\nКонец main. Объекты будут уничтожены автоматически." << endl;
    return 0;
}