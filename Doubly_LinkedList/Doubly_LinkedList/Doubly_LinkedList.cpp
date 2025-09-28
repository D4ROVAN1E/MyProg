#include <iostream>
#include<string>

using namespace std;

// Структура узла для двусвязного списка
struct Node
{
    int key;
    Node* next; // Указатель на следующий элемент
    Node* prev; // Указатель на предыдущий элемент
};

// Структура двусвязного списка
struct DoublyList
{
    Node* head = nullptr;
    Node* tail = nullptr;
};

//Создает список с начальным элементом
void LCREATE(DoublyList& dList, int keyBegin) {
    Node* newNode = new Node{ keyBegin, nullptr, nullptr };
    dList.head = newNode;
    dList.tail = newNode;
}

//Добавление элемента в НАЧАЛО списка
void LPUSH_HEAD(DoublyList& dList, int key) {
    Node* newNode = new Node{ key, dList.head, nullptr };

    if (dList.head) { // Если список не пуст
        dList.head->prev = newNode;
    }
    else { // Если список был пуст, новый узел также является хвостом
        dList.tail = newNode;
    }
    dList.head = newNode;
}

//Добавление элемента в КОНЕЦ списка
void LPUSH_BACK(DoublyList& dList, int key) {
    Node* newNode = new Node{ key, nullptr, dList.tail };

    if (dList.tail) { // Если список не пуст
        dList.tail->next = newNode;
    }
    else { // Если список был пуст, новый узел также является головой
        dList.head = newNode;
    }
    dList.tail = newNode;
}

//Удаление первого элемента (головы) списка
void LDEL_HEAD(DoublyList& dList) {
    if (!dList.head) return; // Список пуст

    Node* temp = dList.head;
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
void LDEL_BACK(DoublyList& dList) {
    if (!dList.tail) return; // Список пуст

    Node* temp = dList.tail;
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
Node* LGET_BY_VALUE(const DoublyList& dList, int key) {
    Node* current = dList.head;
    while (current != nullptr) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return nullptr; // Элемент не найден
}

//Добавление элемента ДО узла с заданным значением
void LPUSH_BEFORE (DoublyList& dList, int targetKey, int newKey) {
    Node* targetNode = LGET_BY_VALUE(dList, targetKey);
    if (!targetNode) return; // Элемент, перед которым нужно вставить, не найден

    if (targetNode == dList.head) { // Если вставляем перед головой
        LPUSH_HEAD(dList, newKey);
        return;
    }

    Node* newNode = new Node{ newKey, targetNode, targetNode->prev };
    targetNode->prev->next = newNode;
    targetNode->prev = newNode;
}

//Удаление узла по значению (первое вхождение)
void LDEL_BY_VALUE(DoublyList& dList, int key) {
    Node* targetNode = LGET_BY_VALUE(dList, key);
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
void print_forward(const DoublyList& dList) {
    if (!dList.head) {
        cout << "Список пуст." << endl;
        return;
    }
    Node* current = dList.head;
    cout << "Голова -> ";
    while (current != nullptr)
    {
        cout << current->key << " <-> ";
        current = current->next;
    }
    cout << "nullptr" << endl;
}

//Выводит список в консоль от конца до начала (для проверки)
void print_backward(const DoublyList& dList) {
    if (!dList.tail) {
        cout << "Список пуст." << endl;
        return;
    }
    Node* current = dList.tail;
    cout << "Хвост -> ";
    while (current != nullptr)
    {
        cout << current->key << " <-> ";
        current = current->prev;
    }
    cout << "nullptr" << endl;
}

//Очищает всю память, занятую списком
void clean(DoublyList& dList) {
    while (dList.head) {
        LDEL_HEAD(dList);
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    DoublyList list;

    cout << "Создание списка" << endl;
    LCREATE(list, 10);
    print_forward(list);

    cout << "\nДобавление элементов" << endl;
    LPUSH_BACK(list, 20);      // в конец
    LPUSH_BACK(list, 30);      // в конец
    LPUSH_HEAD(list, 5);      // в начало
    LPUSH_BEFORE(list, 20, 15); // до 20
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
    Node* found_node = LGET_BY_VALUE(list, value_to_find);
    if (found_node) {
        cout << "Узел со значением " << value_to_find << " найден. Предыдущий: "
            << (found_node->prev ? to_string(found_node->prev->key) : "null")
            << ", Следующий: " << (found_node->next ? to_string(found_node->next->key) : "null") << endl;
    }
    else {
        cout << "Узел со значением " << value_to_find << " не найден." << endl;
    }

    cout << "\nОчистка списка" << endl;
    clean(list);
    print_forward(list);

    return 0;
}