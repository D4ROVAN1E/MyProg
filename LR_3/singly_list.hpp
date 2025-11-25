#ifndef SINGLY_LIST_HPP
#define SINGLY_LIST_HPP

#include <iostream>
#include <utility>  // Для swap
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept> 

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
        if (!ptr) {
            throw invalid_argument("Pointer cannot be null.");
        }
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
        if (!head) {
            throw runtime_error("List is empty, cannot find element.");
        }

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
        } else {
            // Если мы дошли до конца и не нашли ключ
            throw runtime_error("Target key not found.");
        }
    }

    // Удаление первого элемента (головы) списка
    void FDEL_HEAD() {
        if (!head) {
            throw runtime_error("Attempt to delete from an empty list.");
        }

        SNode<T>* temp = head;
        head = head->next;
        delete temp;
    }

    // Удаление последнего элемента списка
    void FDEL_BACK() {
        if (!head) {
            throw runtime_error("Attempt to delete from an empty list.");
        }

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
        if (!ptr) {
            throw invalid_argument("Pointer cannot be null.");
        }
        
        if (!ptr->next) {
            throw runtime_error("No element exists after the specified node.");
        }

        SNode<T>* nodeToDelete = ptr->next;
        ptr->next = nodeToDelete->next;
        delete nodeToDelete;
    }

    // Удаление элемента ДО узла с заданным значением
    void FDEL_BEFORE(T targetKey) {
        if (!head) {
            throw runtime_error("List is empty.");
        }
        
        // Если искомый элемент - это голова, то перед ним ничего нет
        if (head->key == targetKey) {
            throw logic_error("Cannot delete element before the head of the list.");
        }

        // Если в списке всего один элемент, а мы ищем его (уже обработано выше) или другой
        if (!head->next) {
             throw runtime_error("List is too short or element not found.");
        }

        // Нужно удалить голову списка (элемент перед вторым элементом)
        if (head->next->key == targetKey) {
            FDEL_HEAD();
            return;
        }

        SNode<T>* current = head;
        bool found = false;
        // Ищем узел, который находится за два шага до целевого
        while (current->next && current->next->next) {
            if (current->next->next->key == targetKey) {
                FDEL_AFTER(current);
                found = true;
                return;
            }
            current = current->next;
        }

        if (!found) {
            throw runtime_error("Target element not found.");
        }
    }

    // Удаление узла по значению (первое вхождение)
    void FDEL_BY_VALUE(T key) {
        if (!head) {
            throw runtime_error("List is empty.");
        }

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
        } else {
            throw runtime_error("Element with specified value not found.");
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
            throw runtime_error("Error opening file for writing: " + filename);
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
             throw runtime_error("Error opening file for reading: " + filename);
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
        
        // Проверка на пустой файл или ошибки чтения
        if (first && file.eof()) {
             // Файл был пуст, список остался пустым.
        } else if (file.fail() && !file.eof()) {
             throw runtime_error("Invalid data format in file.");
        }
    }

    // Сериализация (сохранение в бинарный файл)
    void FSERIALIZE(const string& filename) const {
        // Открываем файл с флагом ios::binary
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Error opening file for writing: " + filename);
        }

        SNode<T>* current = head;
        while (current != nullptr) {
            // Записываем побитовое представление ключа (key)
            // reinterpret_cast преобразует указатель на данные в указатель на char*,
            // который требуется функции write.
            file.write(reinterpret_cast<const char*>(&current->key), sizeof(T));
            if (file.fail()) {
                 throw runtime_error("Error writing to file.");
            }
            current = current->next;
        }
        file.close();
    }

    // Десериализация (загрузка из бинарного файла)
    void FDESERIALIZE(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Error opening file for reading: " + filename);
        }

        // Очищаем текущий список перед загрузкой
        while (head) {
            FDEL_HEAD();
        }

        T tempValue;
        SNode<T>* tail = nullptr;

        // Читаем из файла блоками размером sizeof(T)
        while (file.read(reinterpret_cast<char*>(&tempValue), sizeof(T))) {
            // Создаем новый узел
            SNode<T>* newNode = new SNode<T>{ tempValue, nullptr };

            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        
        // Проверка ошибок чтения (кроме EOF)
        if (file.fail() && !file.eof()) {
             throw runtime_error("Error reading binary data.");
        }
        
        file.close();
    }

    auto GetHead() const -> SNode<T>* {
        return head;
    }
};

#endif  // SINGLY_LIST_HPP
