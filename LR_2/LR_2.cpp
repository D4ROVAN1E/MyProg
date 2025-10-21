#include "binary_tree.h"
#include "stack.h"
#include <iostream>

using namespace std;

// Итеративный прямой обход (Pre-order) с использованием стека
// Порядок: Корень -> Левый -> Правый
template<typename T>
void DFS_Preorder(TreeNode<T>* root) {
    if (root == nullptr) {
        cout << "Дерево пустое." << endl;
        return;
    }

    Stack<TreeNode<T>*> st;
    SPUSH(st, root);

    cout << "Pre-order DFS (итеративный со стеком): ";

    while (st.size > 0) {
        TreeNode<T>* current = SPOP(st);
        cout << current->key << " ";

        // Важно: сначала помещаем правого потомка, потом левого
        // Так как стек работает по принципу LIFO, левый будет обработан первым
        if (current->right != nullptr) {
            SPUSH(st, current->right);
        }
        if (current->left != nullptr) {
            SPUSH(st, current->left);
        }
    }
    cout << endl;
}

// Итеративный центрированный обход (In-order) с использованием стека
// Порядок: Левый -> Корень -> Правый
template<typename T>
void DFS_Inorder(TreeNode<T>* root) {
    if (root == nullptr) {
        cout << "Дерево пустое." << endl;
        return;
    }

    Stack<TreeNode<T>*> st;
    TreeNode<T>* current = root;

    cout << "In-order DFS (итеративный со стеком): ";

    while (current != nullptr || st.size > 0) {
        // Идём максимально влево, добавляя узлы в стек
        while (current != nullptr) {
            SPUSH(st, current);
            current = current->left;
        }

        // Извлекаем узел из стека и обрабатываем
        current = SPOP(st);
        cout << current->key << " ";

        // Переходим к правому поддереву
        current = current->right;
    }
    cout << endl;
}

// Итеративный обратный обход (Post-order) с использованием стека
// Порядок: Левый -> Правый -> Корень
template<typename T>
void DFS_Postorder(TreeNode<T>* root) {
    if (root == nullptr) {
        cout << "Дерево пустое." << endl;
        return;
    }

    Stack<TreeNode<T>*> st1, st2;
    SPUSH(st1, root);

    cout << "Post-order DFS (итеративный со стеком): ";

    // Используем два стека для реализации post-order
    while (st1.size > 0) {
        TreeNode<T>* current = SPOP(st1);
        SPUSH(st2, current);

        // Добавляем сначала левого, потом правого потомка
        if (current->left != nullptr) {
            SPUSH(st1, current->left);
        }
        if (current->right != nullptr) {
            SPUSH(st1, current->right);
        }
    }

    // Извлекаем элементы из второго стека для получения правильного порядка
    while (st2.size > 0) {
        TreeNode<T>* node = SPOP(st2);
        cout << node->key << " ";
    }
    cout << endl;
}

// Универсальная функция DFS с выбором типа обхода
template<typename T>
void DFS_Traverse(FullBinaryTree<T>& tree, int traversal_type) {
    cout << "\n=== Обход дерева в глубину (DFS) ===" << endl;

    switch (traversal_type) {
    case 1:
        DFS_Preorder(tree.root);
        break;
    case 2:
        DFS_Inorder(tree.root);
        break;
    case 3:
        DFS_Postorder(tree.root);
        break;
    default:
        cout << "Неверный тип обхода!" << endl;
    }
}

void printMenu() {
    cout << "\n======================================" << endl;
    cout << "\tМЕНЮ РАБОТЫ С ДЕРЕВОМ И СТЕКОМ" << endl;
    cout << "======================================" << endl;
    cout << "1. Добавить элемент в дерево" << endl;
    cout << "2. Визуальный вывод дерева" << endl;
    cout << "3. DFS Pre-order (со стеком)" << endl;
    cout << "4. DFS In-order (со стеком)" << endl;
    cout << "5. DFS Post-order (со стеком)" << endl;
    cout << "6. Все типы обхода DFS" << endl;
    cout << "7. Сравнение с рекурсивными обходами" << endl;
    cout << "8. Проверка на полноту дерева" << endl;
    cout << "9. Сохранить дерево в файл" << endl;
    cout << "10. Загрузить дерево из файла" << endl;
    cout << "0. Выход" << endl;
    cout << "======================================" << endl;
    cout << "Выберите действие: ";
}

int main() {
	setlocale(LC_ALL, "Russian");
    FullBinaryTree<int> tree;
    int choice, value;
    string filename;

    // Создаём тестовое дерево для демонстрации
    cout << "Создание тестового дерева с элементами: 50, 30, 70, 20, 40, 60, 80" << endl;
    TINSERT(tree, 50);
    TINSERT(tree, 30);
    TINSERT(tree, 70);
    TINSERT(tree, 20);
    TINSERT(tree, 40);
    TINSERT(tree, 60);
    TINSERT(tree, 80);

    cout << "\nВизуализация дерева:" << endl;
    printTreeVisual(tree.root);

    while (true) {
        printMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Введите значение для добавления: ";
            cin >> value;
            TINSERT(tree, value);
            cout << "Элемент " << value << " добавлен." << endl;
            break;

        case 2:
            cout << "\nВизуализация дерева:" << endl;
            printTreeVisual(tree.root);
            break;

        case 3:
            DFS_Preorder(tree.root);
            break;

        case 4:
            DFS_Inorder(tree.root);
            break;

        case 5:
            DFS_Postorder(tree.root);
            break;

        case 6:
            cout << "\n=== Все типы DFS обхода ===" << endl;
            DFS_Preorder(tree.root);
            DFS_Inorder(tree.root);
            DFS_Postorder(tree.root);
            break;

        case 7:
            cout << "\n=== Сравнение итеративных (стек) и рекурсивных обходов ===" << endl;
            cout << "\nИтеративный Pre-order: ";
            DFS_Preorder(tree.root);
            cout << "Рекурсивный Pre-order: ";
            printPreorder(tree);

            cout << "\nИтеративный In-order: ";
            DFS_Inorder(tree.root);
            cout << "Рекурсивный In-order: ";
            printInorder(tree);

            cout << "\nИтеративный Post-order: ";
            DFS_Postorder(tree.root);
            cout << "Рекурсивный Post-order: ";
            printPostorder(tree);
            break;

        case 8:
            if (tree.root != nullptr) {
                bool isFull = TFULL(tree.root);
                cout << "Дерево " << (isFull ? "является" : "не является")
                    << " полным бинарным деревом." << endl;
            }
            else {
                cout << "Дерево пустое." << endl;
            }
            break;

        case 9:
            cout << "Введите имя файла для сохранения: ";
            cin >> filename;
            TSAVE(tree, filename);
            break;

        case 10:
            cout << "Введите имя файла для загрузки: ";
            cin >> filename;
            TLOAD(tree, filename);
            break;

        case 0:
            cout << "Выход из программы." << endl;
            return 0;

        default:
            cout << "Неверный выбор. Попробуйте снова." << endl;
        }
    }

    return 0;
}