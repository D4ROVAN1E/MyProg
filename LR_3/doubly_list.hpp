#ifndef DOUBLY_LIST_HPP
#define DOUBLY_LIST_HPP
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>

using namespace std;

// Структура узла для двусвязного списка
template <typename T>
struct DNode {
    T key;
    DNode<T>* next;  // Указатель на следующий элемент
    DNode<T>* prev;  // Указатель на предыдущий элемент
};

// Структура двусвязного списка
template <typename T>
class DoublyList {
 private:
    DNode<T>* head = nullptr;
    DNode<T>* tail = nullptr;

    // Выводит список в консоль от начала до конца
    void print_forward() const {
        if (!head) {
            cout << "Список пуст." << endl;
            return;
        }
        DNode<T>* current = head;
        cout << "Голова -> ";
        while (current != nullptr) {
            cout << current->key << " <-> ";
            current = current->next;
        }
        cout << "nullptr" << endl;
    }

    // Выводит список в консоль от конца до начала (для проверки)
    void print_backward() const {
        if (!tail) {
            cout << "Список пуст." << endl;
            return;
        }
        DNode<T>* current = tail;
        cout << "Хвост -> ";
        while (current != nullptr) {
            cout << current->key << " <-> ";
            current = current->prev;
        }
        cout << "nullptr" << endl;
    }

 public:
    DoublyList() : head(nullptr), tail(nullptr) {}

    // Деструктор (очищает всю память)
    ~DoublyList() {
        while (head) {
            DNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Копирующий конструктор (создает глубокую копию)
    DoublyList(const DoublyList& other) : head(nullptr), tail(nullptr) {
        if (!other.head) {
            return;
        }
        DNode<T>* current_other = other.head;
        while (current_other) {
            LPUSH_BACK(current_other->key);
            current_other = current_other->next;
        }
    }

    auto operator=(const DoublyList& other) -> DoublyList& {
        if (this != &other) {
            // Создание глубокой копии
            DoublyList temp(other);

            // Используем swap.
            swap(head, temp.head);
            swap(tail, temp.tail);
        }

        return *this;
    }

    // Создает список с начальным элементом
    void LCREATE(T keyBegin) {
        DNode<T>* newDNode = new DNode<T>{ keyBegin, nullptr, nullptr };
        head = newDNode;
        tail = newDNode;
    }

    // Добавление элемента в НАЧАЛО списка
    void LPUSH_HEAD(T key) {
        DNode<T>* newDNode = new DNode<T>{ key, head, nullptr };

        if (head) {   // Если список не пуст
            head->prev = newDNode;
        } else {  // Если список был пуст, новый узел также является хвостом
            tail = newDNode;
        }
        head = newDNode;
    }

    // Добавление элемента в КОНЕЦ списка
    void LPUSH_BACK(T key) {
        DNode<T>* newDNode = new DNode<T>{ key, nullptr, tail };

        if (tail) {  // Если список не пуст
            tail->next = newDNode;
        } else {  // Если список был пуст, новый узел также является головой
            head = newDNode;
        }
        tail = newDNode;
    }

    // Удаление первого элемента (головы) списка
    void LDEL_HEAD() {
        if (!head) return;  // Список пуст

        DNode<T>* temp = head;
        head = head->next;

        if (head) {  // Если в списке остались элементы
            head->prev = nullptr;
        } else {  // Если список стал пустым
            tail = nullptr;
        }
        delete temp;
    }

    // Удаление последнего элемента списка
    void LDEL_BACK() {
        if (!tail) return;  // Список пуст

        DNode<T>* temp = tail;
        tail = tail->prev;

        if (tail)  // Если в списке остались элементы
            tail->next = nullptr;
        else  // Если список стал пустым
            head = nullptr;
        delete temp;
    }

    // Чтение (поиск) элемента по значению
    auto LGET_BY_VALUE(T key) const -> DNode<T>* {
        DNode<T>* current = head;
        while (current != nullptr) {
            if (current->key == key) {
                return current;
            }
            current = current->next;
        }
        return nullptr;  // Элемент не найден
    }

    // Добавление элемента ДО узла с заданным значением
    void LPUSH_BEFORE(T targetKey, T newKey) {
        DNode<T>* targetDNode = LGET_BY_VALUE(targetKey);
        // Элемент, перед которым нужно вставить, не найден
        if (!targetDNode) return;

        if (targetDNode == head) {  // Если вставляем перед головой
            LPUSH_HEAD(newKey);
            return;
        }

        DNode<T>* newDNode = new DNode<T>{ newKey,
                                        targetDNode,
                                        targetDNode->prev };
        targetDNode->prev->next = newDNode;
        targetDNode->prev = newDNode;
    }

    // Добавление элемента ПОСЛЕ узла с заданным значением
    void LPUSH_AFTER(T targetKey, T newKey) {
        DNode<T>* targetDNode = LGET_BY_VALUE(targetKey);
        // Элемент, после которого нужно вставить, не найден
        if (!targetDNode) return;
        if (targetDNode == tail) {  // Если вставляем после хвоста
            LPUSH_BACK(newKey);
            return;
        }
        DNode<T>* newDNode = new DNode<T>{ newKey,
                                        targetDNode->next,
                                        targetDNode };
        targetDNode->next->prev = newDNode;
        targetDNode->next = newDNode;
    }

    // Удаление элемента ПОСЛЕ узла с заданным значением
    void LDEL_AFTER(T targetKey) {
        DNode<T>* targetNode = LGET_BY_VALUE(targetKey);

        // Если целевой узел не найден или он является хвостом,
        // то удалять нечего.
        if (!targetNode || !targetNode->next) {
            return;
        }

        DNode<T>* nodeToDelete = targetNode->next;

        // Если удаляемый узел - хвост, используем готовую функцию
        if (nodeToDelete == tail) {
            LDEL_BACK();
        } else {
            // Если это узел в середине списка
            targetNode->next = nodeToDelete->next;
            nodeToDelete->next->prev = targetNode;
            delete nodeToDelete;
        }
    }

    // Удаление элемента ДО узла с заданным значением
    void LDEL_BEFORE(T targetKey) {
        DNode<T>* targetNode = LGET_BY_VALUE(targetKey);

        // Если целевой узел не найден или он является головой,
        // то удалять нечего.
        if (!targetNode || !targetNode->prev) {
            return;
        }

        DNode<T>* nodeToDelete = targetNode->prev;

        // Если удаляемый узел - голова, используем готовую функцию
        if (nodeToDelete == head) {
            LDEL_HEAD();
        } else {
            // Если это узел в середине списка
            targetNode->prev = nodeToDelete->prev;
            nodeToDelete->prev->next = targetNode;
            delete nodeToDelete;
        }
    }

    // Удаление узла по значению (первое вхождение)
    void LDEL_BY_VALUE(T key) {
        DNode<T>* targetDNode = LGET_BY_VALUE(key);
        if (!targetDNode) return;  // Узел для удаления не найден

        if (targetDNode == head) {
            LDEL_HEAD();
        } else if (targetDNode == tail) {
            LDEL_BACK();
        } else {
            // Связываем предыдущий и следующий узлы между собой
            targetDNode->prev->next = targetDNode->next;
            targetDNode->next->prev = targetDNode->prev;
            delete targetDNode;
        }
    }

    void PRINT(int choice) const {
        switch (choice) {
        case 1:
            print_forward();
            break;
        case 2:
            print_backward();
            break;
        default:
            break;
        }
    }

    // Сохранение списка в файл
    void LSAVE(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }
        DNode<T>* current = head;
        while (current != nullptr) {
            file << current->key << " ";
            current = current->next;
        }
        file.close();
        cout << "Двусвязный список сохранён в файл: " << filename << endl;
    }

    // Загрузка списка из файла
    void LLOAD(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }
        // Очищаем текущий список
        while (head) {
            LDEL_HEAD();
        }

        T value;
        bool first = true;
        while (file >> value) {
            if (first) {
                LCREATE(value);
                first = false;
            } else {
                LPUSH_BACK(value);
            }
        }
        file.close();
        cout << "Двусвязный список загружен из файла: " << filename << endl;
    }

    auto GetHead() const -> DNode<T>* {
        return head;
    }

    auto GetTail() const -> DNode<T>* {
        return tail;
    }
};

#endif   // DOUBLY_LIST_HPP
