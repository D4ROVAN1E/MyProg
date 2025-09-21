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
void createDL(DoublyList& dList, int keyBegin) {
    Node* newNode = new Node{ keyBegin, nullptr, nullptr };
    dList.head = newNode;
    dList.tail = newNode;
}

//Добавление элемента в НАЧАЛО списка
void push_front(DoublyList& dList, int key) {
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
void push_back(DoublyList& dList, int key) {
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
void pop_front(DoublyList& dList) {
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
void pop_back(DoublyList& dList) {
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
Node* find_by_value(const DoublyList& dList, int key) {
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
void add_before(DoublyList& dList, int targetKey, int newKey) {
    Node* targetNode = find_by_value(dList, targetKey);
    if (!targetNode) return; // Элемент, перед которым нужно вставить, не найден

    if (targetNode == dList.head) { // Если вставляем перед головой
        push_front(dList, newKey);
        return;
    }

    Node* newNode = new Node{ newKey, targetNode, targetNode->prev };
    targetNode->prev->next = newNode;
    targetNode->prev = newNode;
}

//Удаление узла по значению (первое вхождение)
void delete_by_value(DoublyList& dList, int key) {
    Node* targetNode = find_by_value(dList, key);
    if (!targetNode) return; // Узел для удаления не найден

    if (targetNode == dList.head) {
        pop_front(dList);
    }
    else if (targetNode == dList.tail) {
        pop_back(dList);
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
        cout << "List is empty." << endl;
        return;
    }
    Node* current = dList.head;
    cout << "Head -> ";
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
        cout << "List is empty." << endl;
        return;
    }
    Node* current = dList.tail;
    cout << "Tail -> ";
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
        pop_front(dList);
    }
}

int main() {
    DoublyList list;

    cout << "--- Initial list creation ---" << endl;
    createDL(list, 10);
    print_forward(list);

    cout << "\n--- Adding elements ---" << endl;
    push_back(list, 20);      // в конец
    push_back(list, 30);      // в конец
    push_front(list, 5);      // в начало
    add_before(list, 20, 15); // до 20
    add_before(list, 5, 2);   // до головы
    print_forward(list);
    cout << "Verification print backward:" << endl;
    print_backward(list);

    cout << "\n--- Deleting elements ---" << endl;
    pop_front(list); // удаляем 2
    cout << "After pop_front(): ";
    print_forward(list);

    pop_back(list);  // удаляем 30
    cout << "After pop_back(): ";
    print_forward(list);

    delete_by_value(list, 15); // удаляем 15 (из середины)
    cout << "After delete_by_value(15): ";
    print_forward(list);
    cout << "Verification print backward:" << endl;
    print_backward(list);

    cout << "\n--- Finding element by value ---" << endl;
    int value_to_find = 20;
    Node* found_node = find_by_value(list, value_to_find);
    if (found_node) {
        cout << "Node with value " << value_to_find << " found. Prev: "
            << (found_node->prev ? to_string(found_node->prev->key) : "null")
            << ", Next: " << (found_node->next ? to_string(found_node->next->key) : "null") << endl;
    }
    else {
        cout << "Node with value " << value_to_find << " not found." << endl;
    }

    cout << "\n--- Cleaning up list ---" << endl;
    clean(list);
    print_forward(list);

    return 0;
}