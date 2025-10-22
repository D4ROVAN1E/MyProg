#pragma once
#include <iostream>
#include <utility> // Для swap
#include <fstream>
#include <sstream>

using namespace std;

template <typename T>
struct SNode
{
    T key;
    SNode<T>* next; // указатель на следующий элемент
};

template <typename T>
struct ForwardList
{
    SNode<T>* head;

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
            SNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }
};


// Добавляет узел ПОСЛЕ указанного узла ptr
template <typename T>
void FPUSH_FORWARD(SNode<T>* ptr, T key) {
    if (!ptr) return;
    SNode<T>* newSNode = new SNode<T>{ key, nullptr };
    newSNode->next = ptr->next;
    ptr->next = newSNode;
}

// Создает список с начальным элементом
template <typename T>
void FCREATE(ForwardList<T>& fList, T keyBegin) {
    // Очистим список, если он уже был не пуст
    while (fList.head) {
        SNode<T>* temp = fList.head;
        fList.head = fList.head->next;
        delete temp;
    }
    fList.head = new SNode<T>{ keyBegin, nullptr };
}

// Выводит список в консоль
template <typename T>
void PRINT(const ForwardList<T>& fList) {
    if (!fList.head) {
        cout << "Список пуст" << endl;
        return;
    }
    SNode<T>* current = fList.head;
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
    SNode<T>* newSNode = new SNode<T>{ key, nullptr };
    newSNode->next = fList.head;
    fList.head = newSNode;
}

//Добавление элемента в КОНЕЦ списка
template <typename T>
void FPUSH_BACK(ForwardList<T>& fList, T key) {
    SNode<T>* newSNode = new SNode<T>{ key, nullptr };

    if (fList.head == nullptr) { // Если список пуст
        fList.head = newSNode;
        return;
    }

    SNode<T>* current = fList.head;
    while (current->next != nullptr) { // Идем до последнего элемента
        current = current->next;
    }
    current->next = newSNode; // Последний элемент теперь указывает на новый узел
}

//Добавление элемента ДО узла с заданным значением
template <typename T>
void FPUSH_BEFORE(ForwardList<T>& fList, T targetKey, T newKey) {
    if (!fList.head) return; // Список пуст

    if (fList.head->key == targetKey) {
        FPUSH_HEAD(fList, newKey);
        return;
    }

    SNode<T>* current = fList.head;
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

    SNode<T>* temp = fList.head;
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

    SNode<T>* current = fList.head;
    while (current->next->next != nullptr) {
        current = current->next;
    }

    delete current->next;
    current->next = nullptr;
}

// Удаление элемента ПОСЛЕ указанного узла
template <typename T>
void FDEL_AFTER(SNode<T>* ptr) {
    // Если указанный узел не существует или он последний, ничего не делаем
    if (!ptr || !ptr->next) {
        return;
    }

    SNode<T>* nodeToDelete = ptr->next; // Узел, который нужно удалить
    ptr->next = nodeToDelete->next;   // "Перепрыгиваем" через удаляемый узел
    delete nodeToDelete;              // Освобождаем память
}

// Удаление элемента ДО узла с заданным значением
template <typename T>
void FDEL_BEFORE(ForwardList<T>& fList, T targetKey) {
    // Если список пуст, содержит один элемент, или искомый элемент - голова,
    // то удалять нечего.
    if (!fList.head || !fList.head->next || fList.head->key == targetKey) {
        return;
    }

    // Нужно удалить голову списка (элемент перед вторым элементом)
    if (fList.head->next->key == targetKey) {
        FDEL_HEAD(fList);
        return;
    }

    // Ищем узел, который находится за два шага до целевого
    SNode<T>* current = fList.head;
    while (current->next && current->next->next) {
        if (current->next->next->key == targetKey) {
            // Мы нашли узел (current), после которого идет узел для удаления.
            // Используем уже существующую функцию FDEL_AFTER.
            FDEL_AFTER(current);
            return;
        }
        current = current->next;
    }
}

//Удаление узла по значению (первое вхождение)
template <typename T>
void FDEL_BY_VALUE(ForwardList<T>& fList, T key) {
    if (!fList.head) return;

    if (fList.head->key == key) {
        FDEL_HEAD(fList);
        return;
    }

    SNode<T>* current = fList.head;
    while (current->next != nullptr && current->next->key != key) {
        current = current->next;
    }

    if (current->next != nullptr) {
        SNode<T>* deleteSNode = current->next;
        current->next = deleteSNode->next;
        delete deleteSNode;
    }
}


//Чтение (поиск) элемента по значению
template <typename T>
SNode<T>* FGET_BY_VALUE(ForwardList<T>& fList, T key) {
    SNode<T>* current = fList.head;
    while (current != nullptr) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Сохранение списка в файл
template <typename T>
void FSAVE(const ForwardList<T>& fList, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }
    SNode<T>* current = fList.head;
    while (current != nullptr) {
        file << current->key << " ";
        current = current->next;
    }
    file.close();
    //cout << "Односвязный список сохранён в файл: " << filename << endl;
}

// Загрузка списка из файла
template <typename T>
void FLOAD(ForwardList<T>& fList, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    // Очищаем текущий список
    while (fList.head) {
        FDEL_HEAD(fList);
    }

    T value;
    bool first = true;
    while (file >> value) {
        if (first) {
            FCREATE(fList, value);
            first = false;
        }
        else {
            FPUSH_BACK(fList, value);
        }
    }
    file.close();
    //cout << "Односвязный список загружен из файла: " << filename << endl;
};