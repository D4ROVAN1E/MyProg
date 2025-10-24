#include "binary_tree.hpp"
#include "stack.hpp"
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

    cout << "Pre-order DFS: ";

    while (st.size > 0) {
        TreeNode<T>* current = SPOP(st);
        cout << current->key << " ";

        //Сначала помещаем правого потомка, потом левого
        //Так как стек работает по принципу LIFO, левый будет обработан первым
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

    cout << "In-order DFS: ";

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

    cout << "Post-order DFS: ";

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
    cout << "3. DFS Pre-order" << endl;
    cout << "4. DFS In-order" << endl;
    cout << "5. DFS Post-order" << endl;
    cout << "6. Проверка на полноту дерева" << endl;
    cout << "0. Выход" << endl;
    cout << "======================================" << endl;
    cout << "Выберите действие: ";
}

int main() {
	setlocale(LC_ALL, "Russian");
    FullBinaryTree<int> tree;

    cout << "Создание тестового дерева с элементами: 5, 3, 7, 2, 4, 6, 8" << endl;
    TINSERT(tree, 5);
    TINSERT(tree, 3);
    TINSERT(tree, 7);
    TINSERT(tree, 2);
    TINSERT(tree, 4);
    TINSERT(tree, 6);
    TINSERT(tree, 8);

    cout << "\nВизуализация дерева:" << endl;
    printTreeVisual(tree.root);

    while (true) {
        
        int choice, value;
        string filename;

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
            if (tree.root != nullptr) {
                bool isFull = TFULL(tree.root);
                cout << "Дерево " << (isFull ? "является" : "не является")
                    << " полным бинарным деревом." << endl;
            }
            else {
                cout << "Дерево пустое." << endl;
            }
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