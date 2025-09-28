#include <iostream>

using namespace std;

struct Node
{
    int key;
    Node* next; // указатель на следующий элемент
};

struct ForwardList
{
    Node* head = nullptr;
};

// Добавляет узел ПОСЛЕ указанного узла ptr
void FPUSH_FORWARD(Node* ptr, int key) {
    if (!ptr) return; // Нельзя добавить после nullptr
    Node* newNode = new Node{ key, nullptr };
    newNode->next = ptr->next; // передаем указатель на следующий элемент для вставки
    ptr->next = newNode; // создаем связь с новым элементом
}

// Создает список с начальным элементом
void FCREATE(ForwardList& fList, int keyBegin) {
    fList.head = new Node{ keyBegin, nullptr };
}

// Удаляет узел ПОСЛЕ указанного узла ptr
void FDEL_FORWARD(Node* ptr) {
    if (!ptr || !ptr->next)
    {
        return;
    }

    Node* deleteNode = ptr->next;
    ptr->next = deleteNode->next;

    delete deleteNode;
}

// Выводит список в консоль
void print(const ForwardList& fList) {
    if (!fList.head) {
        cout << "Список пуст" << endl;
        return;
    }
    Node* current = fList.head;
    while (current != nullptr)
    {
        cout << current->key << " -> ";
        current = current->next;
    }
    cout << "nullptr" << endl;
}

// Очищает всю память, занятую списком
void clean(ForwardList& fList) {
    while (fList.head)
    {
        Node* temp = fList.head;
        fList.head = fList.head->next;
        delete temp;
    }
}

//Добавление элемента в НАЧАЛО списка
void FPUSH_HEAD(ForwardList& fList, int key) {
    Node* newNode = new Node{ key, nullptr };
    newNode->next = fList.head;
    fList.head = newNode;
}

//Добавление элемента в КОНЕЦ списка
void FPUSH_BACK(ForwardList& fList, int key) {
    Node* newNode = new Node{ key, nullptr };

    if (fList.head == nullptr) { // Если список пуст
        fList.head = newNode;
        return;
    }

    Node* current = fList.head;
    while (current->next != nullptr) { // Идем до последнего элемента
        current = current->next;
    }
    current->next = newNode; // Последний элемент теперь указывает на новый узел
}

//Добавление элемента ДО узла с заданным значением
void FPUSH_BEFORE(ForwardList& fList, int targetKey, int newKey) {
    if (!fList.head) return; // Список пуст

    // Если искомый элемент - голова списка, используем FPUSH_HEAD
    if (fList.head->key == targetKey) {
        FPUSH_HEAD(fList, newKey);
        return;
    }

    Node* current = fList.head;
    // Ищем узел, ПОСЛЕ которого нужно вставить новый (т.е. предшествующий target)
    while (current->next != nullptr && current->next->key != targetKey) {
        current = current->next;
    }

    if (current->next != nullptr) { // Если нашли узел с targetKey
        FPUSH_FORWARD(current, newKey); // Используем уже существующую функцию добавления после
    }
}

//Удаление первого элемента (головы) списка
void FDEL_HEAD(ForwardList& fList) {
    if (!fList.head) return; // Список уже пуст

    Node* temp = fList.head;
    fList.head = fList.head->next;
    delete temp;
}

//Удаление последнего элемента списка
void FDEL_BACK(ForwardList& fList) {
    if (!fList.head) return; // Список пуст

    if (fList.head->next == nullptr) { // В списке только один элемент
        delete fList.head;
        fList.head = nullptr;
        return;
    }

    Node* current = fList.head;
    // Идем до предпоследнего элемента
    while (current->next->next != nullptr) {
        current = current->next;
    }

    delete current->next; // Удаляем последний элемент
    current->next = nullptr; // Предыдущий теперь последний
}

//Удаление узла по значению (первое вхождение)
void FDEL_BY_VALUE(ForwardList& fList, int key) {
    if (!fList.head) return; // Список пуст

    // Если удаляемый элемент - голова
    if (fList.head->key == key) {
        FDEL_HEAD(fList);
        return;
    }

    Node* current = fList.head;
    // Ищем узел, который стоит ПЕРЕД удаляемым
    while (current->next != nullptr && current->next->key != key) {
        current = current->next;
    }

    if (current->next != nullptr) { // Если нашли такой узел
        FDEL_FORWARD(current); // Используем существующую функцию удаления после
    }
}

//Чтение (поиск) элемента по значению
Node* FGET_BY_VALUE(ForwardList& fList, int key) {
    Node* current = fList.head;
    while (current != nullptr) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return nullptr; // Элемент не найден
}


int main() {
    setlocale(LC_ALL, "ru");
    ForwardList list;

    cout << "Создание списка" << endl;
    FCREATE(list, 10);
    print(list);

    cout << "\nДобавление элементов" << endl;
    FPUSH_BACK(list, 20); // в конец
    FPUSH_BACK(list, 30); // в конец
    FPUSH_HEAD(list, 5); // в начало
    FPUSH_BEFORE(list, 20, 15); // до 20
    FPUSH_BEFORE(list, 5, 2); // до головы
    print(list);

    cout << "\nУдаление элементов" << endl;
    FDEL_HEAD(list); // удаляем 2
    print(list);
    FDEL_BACK(list); // удаляем 30
    print(list);
    FDEL_BY_VALUE(list, 15); // удаляем 15
    print(list);

    cout << "\nНахождение по значению" << endl;
    int value_to_find = 20;
    Node* found_node = FGET_BY_VALUE(list, value_to_find);
    if (found_node) {
        cout << "Узел со значением " << value_to_find << " найден по адресу: " << found_node << endl;
    }
    else {
        cout << "Узел со значением " << value_to_find << " не найден." << endl;
    }

    value_to_find = 99;
    found_node = FGET_BY_VALUE(list, value_to_find);
    if (found_node) {
        cout << "Узел со значением " << value_to_find << " найден по адресу: " << found_node << endl;
    }
    else {
        cout << "Узел со значением " << value_to_find << " не найден." << endl;
    }

    cout << "\nОчистка списка" << endl;
    clean(list);
    print(list);

    return 0;
}