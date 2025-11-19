#ifndef SINGLY_LIST_HPP
#define SINGLY_LIST_HPP
#include <iostream>
#include <utility>  // Для swap
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

template <typename T>
struct SNode {
    T key;
    SNode<T>* next;  // указатель на следующий элемент
};

template <typename T>
class ForwardList {
 private:
    SNode<T>* head;

 public:
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
        head = new SNode<T>{ other.head->key, nullptr };
        SNode<T>* current_this = head;
        SNode<T>* current_other = other.head->next;
        while (current_other) {
            current_this->next = new SNode<T>{ current_other->key, nullptr };
            current_this = current_this->next;
            current_other = current_other->next;
        }
    }

    // Копирующий оператор присваивания
    auto operator=(const ForwardList& other) -> ForwardList& {
        if (this != &other) {
            ForwardList temp(other);
            swap(head, temp.head);
        }
        return *this;
    }

    // Деструктор (очищает память)
    ~ForwardList() {
        while (head != nullptr) {
            SNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Добавляет узел ПОСЛЕ указанного узла ptr
    void FPUSH_FORWARD(SNode<T>* ptr, T key) {
        if (!ptr) return;
        SNode<T>* newSNode = new SNode<T>{ key, nullptr };
        newSNode->next = ptr->next;
        ptr->next = newSNode;
    }

    // Создает список с начальным элементом
    void FCREATE(T keyBegin) {
        // Очистим список, если он уже был не пуст
        while (head) {
            SNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
        head = new SNode<T>{ keyBegin, nullptr };
    }

    // Выводит список в консоль
    void PRINT() const {
        if (!head) {
            cout << "Список пуст" << endl;
            return;
        }
        SNode<T>* current = head;
        while (current != nullptr) {
            cout << current->key << " -> ";
            current = current->next;
        }
        cout << "nullptr" << endl;
    }

    // Добавление элемента в НАЧАЛО списка
    void FPUSH_HEAD(T key) {
        SNode<T>* newSNode = new SNode<T>{ key, nullptr };
        newSNode->next = head;
        head = newSNode;
    }

    // Добавление элемента в КОНЕЦ списка
    void FPUSH_BACK(T key) {
        SNode<T>* newSNode = new SNode<T>{ key, nullptr };

        if (head == nullptr) {  // Если список пуст
            head = newSNode;
            return;
        }

        SNode<T>* current = head;
        while (current->next != nullptr) {  // Идем до последнего элемента
            current = current->next;
        }
        // Последний элемент теперь указывает на новый узел
        current->next = newSNode;
    }

    // Добавление элемента ДО узла с заданным значением
    void FPUSH_BEFORE(T targetKey, T newKey) {
        if (!head) return;  // Список пуст

        if (head->key == targetKey) {
            FPUSH_HEAD(newKey);
            return;
        }

        SNode<T>* current = head;
        while (current->next != nullptr && current->next->key != targetKey) {
            current = current->next;
        }

        if (current->next != nullptr) {
            FPUSH_FORWARD(current, newKey);
        }
    }

    // Удаление первого элемента (головы) списка
    void FDEL_HEAD() {
        if (!head) return;  // Список уже пуст

        SNode<T>* temp = head;
        head = head->next;
        delete temp;
    }

    // Удаление последнего элемента списка
    void FDEL_BACK() {
        if (!head) return;

        if (head->next == nullptr) {
            delete head;
            head = nullptr;
            return;
        }

        SNode<T>* current = head;
        while (current->next->next != nullptr) {
            current = current->next;
        }

        delete current->next;
        current->next = nullptr;
    }

    // Удаление элемента ПОСЛЕ указанного узла
    void FDEL_AFTER(SNode<T>* ptr) {
        // Если указанный узел не существует или он последний, ничего не делаем
        if (!ptr || !ptr->next) {
            return;
        }

        SNode<T>* nodeToDelete = ptr->next;  // Узел, который нужно удалить
        ptr->next = nodeToDelete->next;  // "Перепрыгиваем" через удаляемый узел
        delete nodeToDelete;              // Освобождаем память
    }

    // Удаление элемента ДО узла с заданным значением
    void FDEL_BEFORE(T targetKey) {
        // Если список пуст, содержит один элемент
        // или искомый элемент - голова,
        // то удалять нечего.
        if (!head || !head->next || head->key == targetKey) {
            return;
        }

        // Нужно удалить голову списка (элемент перед вторым элементом)
        if (head->next->key == targetKey) {
            FDEL_HEAD();
            return;
        }

        // Ищем узел, который находится за два шага до целевого
        SNode<T>* current = head;
        while (current->next && current->next->next) {
            if (current->next->next->key == targetKey) {
                // Мы нашли узел (current),
                // после которого идет узел для удаления.
                // Используем уже существующую функцию FDEL_AFTER.
                FDEL_AFTER(current);
                return;
            }
            current = current->next;
        }
    }

    // Удаление узла по значению (первое вхождение)
    void FDEL_BY_VALUE(T key) {
        if (!head) return;

        if (head->key == key) {
            FDEL_HEAD();
            return;
        }

        SNode<T>* current = head;
        while (current->next != nullptr && current->next->key != key) {
            current = current->next;
        }

        if (current->next != nullptr) {
            SNode<T>* deleteSNode = current->next;
            current->next = deleteSNode->next;
            delete deleteSNode;
        }
    }


    // Чтение (поиск) элемента по значению
    auto FGET_BY_VALUE(T key) const -> SNode<T>* {
        SNode<T>* current = head;
        while (current != nullptr) {
            if (current->key == key) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Сохранение списка в файл
    void FSAVE(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }
        SNode<T>* current = head;
        while (current != nullptr) {
            file << current->key << " ";
            current = current->next;
        }
        file.close();
    }

    // Загрузка списка из файла

    void FLOAD(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }
        // Очищаем текущий список
        while (head) {
            FDEL_HEAD();
        }

        T value;
        bool first = true;
        while (file >> value) {
            if (first) {
                FCREATE(value);
                first = false;
            } else {
                FPUSH_BACK(value);
            }
        }
        file.close();
    }

    // Сериализация (сохранение в бинарный файл)
    void FSERIALIZE(const string& filename) const {
        // Открываем файл с флагом ios::binary
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }

        SNode<T>* current = head;
        while (current != nullptr) {
            // Записываем побитовое представление ключа (key)
            // reinterpret_cast преобразует указатель на данные в указатель на char*,
            // который требуется функции write.
            file.write(reinterpret_cast<const char*>(&current->key), sizeof(T));
            current = current->next;
        }
        file.close();
    }

    // Десериализация (загрузка из бинарного файла)
    void FDESERIALIZE(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для чтения!" << endl;
            return;
        }

        // Очищаем текущий список перед загрузкой
        while (head) {
            FDEL_HEAD();
        }

        T tempValue;
        SNode<T>* tail = nullptr; // Локальный указатель на конец, чтобы вставка была быстрой O(1)

        // Читаем из файла блоками размером sizeof(T)
        while (file.read(reinterpret_cast<char*>(&tempValue), sizeof(T))) {
            // Создаем новый узел
            SNode<T>* newNode = new SNode<T>{ tempValue, nullptr };

            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode; // Сдвигаем хвост
            }
        }
        file.close();
    }

    auto GetHead() const -> SNode<T>* {
        return head;
    }
};

#endif  // SINGLY_LIST_HPP
