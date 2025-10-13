#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>

using namespace std;

// Структура узла для двусвязного списка
template <typename T>
struct DNode
{
    T key;
    DNode<T>* next; // Указатель на следующий элемент
    DNode<T>* prev; // Указатель на предыдущий элемент
};

// Структура двусвязного списка
template <typename T>
struct DoublyList
{
    DNode<T>* head = nullptr;
    DNode<T>* tail = nullptr;

    DoublyList() {
        head = nullptr;
        tail = nullptr;
    }

    // Деструктор (очищает всю память)
    ~DoublyList() {
        while (head) {
            DNode<T>* temp = head;
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
        DNode<T>* current_other = other.head;
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
    DNode<T>* newDNode = new DNode<T>{ keyBegin, nullptr, nullptr };
    dList.head = newDNode;
    dList.tail = newDNode;
}

//Добавление элемента в НАЧАЛО списка
template <typename T>
void LPUSH_HEAD(DoublyList<T>& dList, T key) {
    DNode<T>* newDNode = new DNode<T>{ key, dList.head, nullptr };

    if (dList.head) { // Если список не пуст
        dList.head->prev = newDNode;
    }
    else { // Если список был пуст, новый узел также является хвостом
        dList.tail = newDNode;
    }
    dList.head = newDNode;
}

//Добавление элемента в КОНЕЦ списка
template <typename T>
void LPUSH_BACK(DoublyList<T>& dList, T key) {
    DNode<T>* newDNode = new DNode<T>{ key, nullptr, dList.tail };

    if (dList.tail) { // Если список не пуст
        dList.tail->next = newDNode;
    }
    else { // Если список был пуст, новый узел также является головой
        dList.head = newDNode;
    }
    dList.tail = newDNode;
}

//Удаление первого элемента (головы) списка
template <typename T>
void LDEL_HEAD(DoublyList<T>& dList) {
    if (!dList.head) return; // Список пуст

    DNode<T>* temp = dList.head;
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

    DNode<T>* temp = dList.tail;
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
DNode<T>* LGET_BY_VALUE(const DoublyList<T>& dList, T key) {
    DNode<T>* current = dList.head;
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
void LPUSH_BEFORE(DoublyList<T>& dList, T targetKey, T newKey) {
    DNode<T>* targetDNode = LGET_BY_VALUE(dList, targetKey);
    if (!targetDNode) return; // Элемент, перед которым нужно вставить, не найден

    if (targetDNode == dList.head) { // Если вставляем перед головой
        LPUSH_HEAD(dList, newKey);
        return;
    }

    DNode<T>* newDNode = new DNode<T>{ newKey, targetDNode, targetDNode->prev };
    targetDNode->prev->next = newDNode;
    targetDNode->prev = newDNode;
}

//Добавление элемента ПОСЛЕ узла с заданным значением
template <typename T>
void LPUSH_AFTER(DoublyList<T>& dList, T targetKey, T newKey) {
    DNode<T>* targetDNode = LGET_BY_VALUE(dList, targetKey);
    if (!targetDNode) return; // Элемент, после которого нужно вставить, не найден
    if (targetDNode == dList.tail) { // Если вставляем после хвоста
        LPUSH_BACK(dList, newKey);
        return;
    }
    DNode<T>* newDNode = new DNode<T>{ newKey, targetDNode->next, targetDNode };
    targetDNode->next->prev = newDNode;
    targetDNode->next = newDNode;
}

//Удаление узла по значению (первое вхождение)
template <typename T>
void LDEL_BY_VALUE(DoublyList<T>& dList, T key) {
    DNode<T>* targetDNode = LGET_BY_VALUE(dList, key);
    if (!targetDNode) return; // Узел для удаления не найден

    if (targetDNode == dList.head) {
        LDEL_HEAD(dList);
    }
    else if (targetDNode == dList.tail) {
        LDEL_BACK(dList);
    }
    else {
        // Связываем предыдущий и следующий узлы между собой
        targetDNode->prev->next = targetDNode->next;
        targetDNode->next->prev = targetDNode->prev;
        delete targetDNode;
    }
}

//Выводит список в консоль от начала до конца
template <typename T>
void print_forward(const DoublyList<T>& dList) {
    if (!dList.head) {
        cout << "Список пуст." << endl;
        return;
    }
    DNode<T>* current = dList.head;
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
    DNode<T>* current = dList.tail;
    cout << "Хвост -> ";
    while (current != nullptr)
    {
        cout << current->key << " <-> ";
        current = current->prev;
    }
    cout << "nullptr" << endl;
}

template <typename T>
void PRINT(const DoublyList<T>& dList, int choice) {
    switch (choice)
    {
    case 1:
        print_forward(dList);
        break;
    case 2:
        print_backward(dList);
        break;
    default:
        break;
    }
}

// Сохранение списка в файл
template <typename T>
void LSAVE(const DoublyList<T>& dList, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }
    DNode<T>* current = dList.head;
    while (current != nullptr) {
        file << current->key << " ";
        current = current->next;
    }
    file.close();
    cout << "Двусвязный список сохранён в файл: " << filename << endl;
}

// Загрузка списка из файла
template <typename T>
void LLOAD(DoublyList<T>& dList, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    // Очищаем текущий список
    while (dList.head) {
        LDEL_HEAD(dList);
    }

    T value;
    bool first = true;
    while (file >> value) {
        if (first) {
            LCREATE(dList, value);
            first = false;
        }
        else {
            LPUSH_BACK(dList, value);
        }
    }
    file.close();
    cout << "Двусвязный список загружен из файла: " << filename << endl;
}