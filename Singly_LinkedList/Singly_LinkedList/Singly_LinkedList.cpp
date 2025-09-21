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
void add_forward(Node* ptr, int key) {
    if (!ptr) return; // Нельзя добавить после nullptr
    Node* newNode = new Node{ key, nullptr };
    newNode->next = ptr->next; // передаем указатель на следующий элемент для вставки
    ptr->next = newNode; // создаем связь с новым элементом
}

// Создает список с начальным элементом
void createFL(ForwardList& fList, int keyBegin) {
    fList.head = new Node{ keyBegin, nullptr };
}

// Удаляет узел ПОСЛЕ указанного узла ptr
void delete_forward(Node* ptr) {
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
        cout << "List is empty." << endl;
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
void push_front(ForwardList& fList, int key) {
    Node* newNode = new Node{ key, nullptr };
    newNode->next = fList.head;
    fList.head = newNode;
}

//Добавление элемента в КОНЕЦ списка
void push_back(ForwardList& fList, int key) {
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
void add_before(ForwardList& fList, int targetKey, int newKey) {
    if (!fList.head) return; // Список пуст

    // Если искомый элемент - голова списка, используем push_front
    if (fList.head->key == targetKey) {
        push_front(fList, newKey);
        return;
    }

    Node* current = fList.head;
    // Ищем узел, ПОСЛЕ которого нужно вставить новый (т.е. предшествующий target)
    while (current->next != nullptr && current->next->key != targetKey) {
        current = current->next;
    }

    if (current->next != nullptr) { // Если нашли узел с targetKey
        add_forward(current, newKey); // Используем уже существующую функцию добавления после
    }
}

//Удаление первого элемента (головы) списка
void pop_front(ForwardList& fList) {
    if (!fList.head) return; // Список уже пуст

    Node* temp = fList.head;
    fList.head = fList.head->next;
    delete temp;
}

//Удаление последнего элемента списка
void pop_back(ForwardList& fList) {
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
void delete_by_value(ForwardList& fList, int key) {
    if (!fList.head) return; // Список пуст

    // Если удаляемый элемент - голова
    if (fList.head->key == key) {
        pop_front(fList);
        return;
    }

    Node* current = fList.head;
    // Ищем узел, который стоит ПЕРЕД удаляемым
    while (current->next != nullptr && current->next->key != key) {
        current = current->next;
    }

    if (current->next != nullptr) { // Если нашли такой узел
        delete_forward(current); // Используем существующую функцию удаления после
    }
}

//Чтение (поиск) элемента по значению
Node* find_by_value(ForwardList& fList, int key) {
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
    ForwardList list;

    cout << "--- Initial list creation ---" << endl;
    createFL(list, 10);
    print(list);

    cout << "\n--- Adding elements ---" << endl;
    push_back(list, 20); // в конец
    push_back(list, 30); // в конец
    push_front(list, 5); // в начало
    add_before(list, 20, 15); // до 20
    add_before(list, 5, 2); // до головы
    print(list);

    cout << "\n--- Deleting elements ---" << endl;
    pop_front(list); // удаляем 2
    print(list);
    pop_back(list); // удаляем 30
    print(list);
    delete_by_value(list, 15); // удаляем 15
    print(list);

    cout << "\n--- Finding element by value ---" << endl;
    int value_to_find = 20;
    Node* found_node = find_by_value(list, value_to_find);
    if (found_node) {
        cout << "Node with value " << value_to_find << " found at address: " << found_node << endl;
    }
    else {
        cout << "Node with value " << value_to_find << " not found." << endl;
    }

    value_to_find = 99;
    found_node = find_by_value(list, value_to_find);
    if (found_node) {
        cout << "Node with value " << value_to_find << " found at address: " << found_node << endl;
    }
    else {
        cout << "Node with value " << value_to_find << " not found." << endl;
    }

    cout << "\n--- Cleaning up list ---" << endl;
    clean(list);
    print(list);

    return 0;
}