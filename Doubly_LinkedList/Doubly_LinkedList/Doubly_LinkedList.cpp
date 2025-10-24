#include <iostream>
#include<string>
#include <utility>

using namespace std;

// Структура узла для двусвязного списка
template <typename T>
struct Node
{
    T key;
    Node<T>* next; // Указатель на следующий элемент
    Node<T>* prev; // Указатель на предыдущий элемент
};

// Структура двусвязного списка
template <typename T>
struct DoublyList
{
    Node<T>* head = nullptr;
    Node<T>* tail = nullptr;

    DoublyList() {
		head = nullptr;
		tail = nullptr;
    }

    // Деструктор (очищает всю память)
    ~DoublyList() {
        while (head) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Копирующий конструктор (создает глубокую копию)
    DoublyList(const DoublyList& other) {
        head = nullptr;
		tail = nullptr;
        if (!other.head) {
            return;
        }
        Node<T>* current_other = other.head;
        while (current_other) {
            this->push_back(current_other->key);
            current_other = current_other->next;
        }
    }

    DoublyList& operator=(const DoublyList& other) {
        if (this != &other) {
            //Создание глубокой копии 
            DoublyList temp(other);

            // Используем swap.
            swap(head, temp.head);
            swap(tail, temp.tail);
        }

        return *this;
    }
};

//Создает список с начальным элементом
template <typename T>
void LCREATE(DoublyList<T>& dList, T keyBegin) {
    Node<T>* newNode = new Node<T>{ keyBegin, nullptr, nullptr };
    dList.head = newNode;
    dList.tail = newNode;
}

//Добавление элемента в НАЧАЛО списка
template <typename T>
void LPUSH_HEAD(DoublyList<T>& dList, T key) {
    Node<T>* newNode = new Node<T>{ key, dList.head, nullptr };

    if (dList.head) { // Если список не пуст
        dList.head->prev = newNode;
    }
    else { // Если список был пуст, новый узел также является хвостом
        dList.tail = newNode;
    }
    dList.head = newNode;
}

//Добавление элемента в КОНЕЦ списка
template <typename T>
void LPUSH_BACK(DoublyList<T>& dList, T key) {
    Node<T>* newNode = new Node<T>{ key, nullptr, dList.tail };

    if (dList.tail) { // Если список не пуст
        dList.tail->next = newNode;
    }
    else { // Если список был пуст, новый узел также является головой
        dList.head = newNode;
    }
    dList.tail = newNode;
}

//Удаление первого элемента (головы) списка
template <typename T>
void LDEL_HEAD(DoublyList<T>& dList) {
    if (!dList.head) return; // Список пуст

    Node<T>* temp = dList.head;
    dList.head = dList.head->next;

    if (dList.head) { // Если в списке остались элементы
        dList.head->prev = nullptr;
    }
    else { // Если список стал пустым
        dList.tail = nullptr;
    }
    delete temp;
}

//Удаление последнего элемента списка
template <typename T>
void LDEL_BACK(DoublyList<T>& dList) {
    if (!dList.tail) return; // Список пуст

    Node<T>* temp = dList.tail;
    dList.tail = dList.tail->prev;

    if (dList.tail) { // Если в списке остались элементы
        dList.tail->next = nullptr;
    }
    else { // Если список стал пустым
        dList.head = nullptr;
    }
    delete temp;
}

//Чтение (поиск) элемента по значению
template <typename T>
Node<T>* LGET_BY_VALUE(const DoublyList<T>& dList, T key) {
    Node<T>* current = dList.head;
    while (current != nullptr) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return nullptr; // Элемент не найден
}

//Добавление элемента ДО узла с заданным значением
template <typename T>
void LPUSH_BEFORE (DoublyList<T>& dList, T targetKey, T newKey) {
    Node<T>* targetNode = LGET_BY_VALUE(dList, targetKey);
    if (!targetNode) return; // Элемент, перед которым нужно вставить, не найден

    if (targetNode == dList.head) { // Если вставляем перед головой
        LPUSH_HEAD(dList, newKey);
        return;
    }

    Node<T>* newNode = new Node<T>{ newKey, targetNode, targetNode->prev };
    targetNode->prev->next = newNode;
    targetNode->prev = newNode;
}

//Добавление элемента ПОСЛЕ узла с заданным значением
template <typename T>
void LPUSH_AFTER(DoublyList<T>& dList, T targetKey, T newKey) {
    Node<T>* targetNode = LGET_BY_VALUE(dList, targetKey);
    if (!targetNode) return; // Элемент, после которого нужно вставить, не найден
    if (targetNode == dList.tail) { // Если вставляем после хвоста
        LPUSH_BACK(dList, newKey);
        return;
    }
    Node<T>* newNode = new Node<T>{ newKey, targetNode->next, targetNode };
    targetNode->next->prev = newNode;
    targetNode->next = newNode;
}

//Удаление узла по значению (первое вхождение)
template <typename T>
void LDEL_BY_VALUE(DoublyList<T>& dList, T key) {
    Node<T>* targetNode = LGET_BY_VALUE(dList, key);
    if (!targetNode) return; // Узел для удаления не найден

    if (targetNode == dList.head) {
        LDEL_HEAD(dList);
    }
    else if (targetNode == dList.tail) {
        LDEL_BACK(dList);
    }
    else {
        // Связываем предыдущий и следующий узлы между собой
        targetNode->prev->next = targetNode->next;
        targetNode->next->prev = targetNode->prev;
        delete targetNode;
    }
}

//Выводит список в консоль от начала до конца
template <typename T>
void print_forward(const DoublyList<T>& dList) {
    if (!dList.head) {
        cout << "Список пуст." << endl;
        return;
    }
    Node<T>* current = dList.head;
    cout << "Голова -> ";
    while (current != nullptr)
    {
        cout << current->key << " <-> ";
        current = current->next;
    }
    cout << "nullptr" << endl;
}

//Выводит список в консоль от конца до начала (для проверки)
template <typename T>
void print_backward(const DoublyList<T>& dList) {
    if (!dList.tail) {
        cout << "Список пуст." << endl;
        return;
    }
    Node<T>* current = dList.tail;
    cout << "Хвост -> ";
    while (current != nullptr)
    {
        cout << current->key << " <-> ";
        current = current->prev;
    }
    cout << "nullptr" << endl;
}

//Очищает всю память, занятую списком
template <typename T>
void clean(DoublyList<T>& dList) {
    while (dList.head) {
        LDEL_HEAD(dList);
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    DoublyList<int> list;

    cout << "Создание списка" << endl;
    LCREATE(list, 10);
    print_forward(list);

    cout << "\nДобавление элементов" << endl;
    LPUSH_BACK(list, 20);      // в конец
    LPUSH_BACK(list, 30);      // в конец
    LPUSH_HEAD(list, 5);      // в начало
    LPUSH_AFTER(list, 20, 15); // до 20
    LPUSH_BEFORE(list, 5, 2);   // до головы
    print_forward(list);
    cout << "Проверка вывода в обратном порядке:" << endl;
    print_backward(list);

    cout << "\nУдаление элементов" << endl;
    LDEL_HEAD(list); // удаляем 2
    cout << "После LDEL_HEAD(): ";
    print_forward(list);

    LDEL_BACK(list);  // удаляем 30
    cout << "После LDEL_BACK(): ";
    print_forward(list);

    LDEL_BY_VALUE(list, 15); // удаляем 15 (из середины)
    cout << "После LDEL_BY_VALUE(15): ";
    print_forward(list);
    cout << "Проверка вывода в обратном порядке:" << endl;
    print_backward(list);

    cout << "\nНахождение элемента по значению" << endl;
    int value_to_find = 20;
    Node<int>* found_node = LGET_BY_VALUE(list, value_to_find);
    if (found_node) {
        cout << "Узел со значением " << value_to_find << " найден. Предыдущий: "
            << (found_node->prev ? to_string(found_node->prev->key) : "null")
            << ", Следующий: " << (found_node->next ? to_string(found_node->next->key) : "null") << endl;
    }
    else {
        cout << "Узел со значением " << value_to_find << " не найден." << endl;
    }

    cout << "\nОчистка списка" << endl;
    print_forward(list);

    return 0;
}