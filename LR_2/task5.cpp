#include <iostream>
#include <sstream>
#include "binary_tree.hpp"

using namespace std;

// Функция для вставки элемента с проверкой на дубликат
// Возвращает глубину вставленного элемента или -1, если элемент уже существует
template<typename T>
int insertAndGetDepth(FullBinaryTree<T>& tree, T value) {
    // Если дерево пустое, новый узел становится корнем с глубиной 1
    if (tree.root == nullptr) {
        TreeNode<T>* new_node = new TreeNode<T>(value);
        tree.root = new_node;
        return 1;
    }

    // Начинаем поиск места для вставки с корня
    TreeNode<T>* current = tree.root;
    TreeNode<T>* parent = nullptr;
    int depth = 1; // Глубина текущего узла (корень имеет глубину 1)

    // Ищем подходящее место для вставки, спускаясь по дереву
    while (current != nullptr) {
        parent = current;

        if (value == current->key) {
            // Элемент уже существует в дереве
            return -1;
        }
        else if (value < current->key) {
            // Идем влево
            current = current->left;
            depth++;
        }
        else {
            // Идем вправо
            current = current->right;
            depth++;
        }
    }

    // Создаем новый узел и вставляем его
    TreeNode<T>* new_node = new TreeNode<T>(value);

    if (value < parent->key) {
        parent->left = new_node;
    }
    else {
        parent->right = new_node;
    }

    return depth;
}

int main() {
    FullBinaryTree<int> tree;
    int value;
    bool first = true;
	string line;
	getline(cin, line);
    istringstream is(line);

    // Читаем значения и вставляем их в дерево
    while (is >> value) {
        int depth = insertAndGetDepth(tree, value);

        // Выводим глубину только если элемент был добавлен (не дубликат)
        if (depth != -1) {
            if (!first) {
                cout << " ";
            }
            cout << depth;
            first = false;
        }
    }

    if (!first) {
        cout << endl;
    }

    return 0;
}