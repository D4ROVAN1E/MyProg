#ifndef BINARY_TREE_HPP
#define BINARY_TREE_HPP
#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>
#include "queue.hpp"

using namespace std;

// Константы для форматирования вывода
const char branch_right[] = "|--- ";   // Ветка к правому потомку
const char branch_left[] = "|___ ";    // Ветка к левому потомку

// Структура узла
template<typename T>
struct TreeNode {
    T key;
    TreeNode<T>* left;
    TreeNode<T>* right;

    // Конструктор для создания узла
    explicit TreeNode(T val) : key(val)
                    , left(nullptr)
                    , right(nullptr) {}
};

template<typename T>
class FullBinaryTree {
 private:
    TreeNode<T>* root;

    // Рекурсивная функция для глубокого копирования дерева
    auto copy_tree(const TreeNode<T>* other_node) const -> TreeNode<T>* {
        if (other_node == nullptr) {
            return nullptr;
        }
        TreeNode<T>* new_node = new TreeNode<T>(other_node->key);
        new_node->left = copy_tree(other_node->left);
        new_node->right = copy_tree(other_node->right);
        return new_node;
    }

    // Рекурсивная функция для удаления всех узлов
    void destroy_tree(TreeNode<T>* node) {
        if (node != nullptr) {
            destroy_tree(node->left);
            destroy_tree(node->right);
            delete node;
        }
    }

        // Вспомогательная рекурсивная функция для красивой печати
    void print_tree_recursive(TreeNode<T>* node,
                            const string& prefix,
                             bool isLeft) const {
        if (node != nullptr) {
            cout << prefix;
            // Используем разные символы для левой и правой ветки
            cout << (isLeft ? branch_left : branch_right);
            // Выводим значение узла
            cout << node->key << endl;

            // Рекурсивно вызываем для потомков, увеличивая отступ
            // Правый потомок идет первым, чтобы дерево "росло" слева направо
            print_tree_recursive(node->right,
                                prefix + (isLeft ? "|   " : "    "),
                                false);
            print_tree_recursive(node->left,
                                prefix + (isLeft ? "|   " : "    "),
                                true);
        }
    }

    // Главная функция для красивой печати дерева
    void printTreeVisual(TreeNode<T>* root) const {
        if (root == nullptr) {
            cout << "Дерево пустое." << endl;
            return;
        }
        // Начинаем печать с корневого узла
        cout << root->key << endl;
        // Вызываем рекурсивную функцию для потомков
        print_tree_recursive(root->right, "", false);
        print_tree_recursive(root->left, "", true);
    }

    // Рекурсивные функцияы для обходов
    void preorder_recursive(TreeNode<T>* node) const {
        if (node != nullptr) {
            cout << node->key << " ";
            preorder_recursive(node->left);
            preorder_recursive(node->right);
        }
    }

    void inorder_recursive(TreeNode<T>* node) const {
        if (node != nullptr) {
            inorder_recursive(node->left);
            cout << node->key << " ";
            inorder_recursive(node->right);
        }
    }

    void postorder_recursive(TreeNode<T>* node) const {
        if (node != nullptr) {
            postorder_recursive(node->left);
            postorder_recursive(node->right);
            cout << node->key << " ";
        }
    }

    auto isFullRecursive(TreeNode<T>* current) const -> bool {
        if (((current->left == nullptr) ^ (current->right == nullptr)) == 1) {
            return false;
        } else if (current->left != nullptr && current->right != nullptr) {
            return isFullRecursive(current->left)
                && isFullRecursive(current->right);
        }
        return true;
    }

 public:
    // Конструктор по умолчанию
    FullBinaryTree() : root(nullptr) {}

    // Деструктор
    ~FullBinaryTree() {
        destroy_tree(root);
    }

    // Копирующий конструктор
    FullBinaryTree(const FullBinaryTree& other) {
        root = copy_tree(other.root);
    }

    // Копирующий оператор присваивания
    auto operator=(const FullBinaryTree& other) -> FullBinaryTree& {
        if (this != &other) {   // Защита от самоприсваивания
            // Очищаем текущие ресурсы
            destroy_tree(root);
            // Копируем ресурсы из другого объекта
            root = copy_tree(other.root);
        }
        return *this;
    }

    // Вставка элемента по принципу бинарного дерева поиска
    void TINSERT(T value) {
        // Создаем новый узел с переданным значением
        TreeNode<T>* new_node = new TreeNode<T>(value);

        // Если дерево пустое, новый узел становится корнем
        if (root == nullptr) {
            root = new_node;
            return;
        }

        // Начинаем поиск места для вставки с корня
        TreeNode<T>* current = root;
        TreeNode<T>* parent = nullptr;

        // Ищем подходящее место для вставки, спускаясь по дереву
        while (current != nullptr) {
            parent = current;   // Запоминаем родителя
            if (value < current->key) {
                // Если значение меньше ключа текущего узла, идем влево
                current = current->left;
            } else {
                // Если значение больше или равно, идем вправо
                current = current->right;
            }
        }

        // Когда найдено пустое место (current == nullptr)
        // вставляем новый узел как левого или правого потомка родителя (parent)
        if (value < parent->key) {
            parent->left = new_node;
        } else {
            parent->right = new_node;
        }
    }

    // Функция проверки на full
    [[nodiscard]] auto TFULL() const -> bool {
        return isFullRecursive(root);
    }

    // Обход в ширину (Level-order / Breadth-first)
    void printBreadthFirst() const {
        if (root == nullptr) {
            return;
        }

        Queue<TreeNode<T>*> q;
        q.QPUSH(root);

        while (!q.empty()) {
            TreeNode<T>* current = q.QPOP();

            cout << current->key << " ";

            if (current->left != nullptr) {
                q.QPUSH(current->left);
            }
            if (current->right != nullptr) {
                q.QPUSH(current->right);
            }
        }
        cout << endl;
    }

    void PRINT(int choise) const {
        switch (choise) {
        case 1:
            printBreadthFirst();
            break;
        case 2:   // Прямой обход (Pre-order):
            preorder_recursive(root);   // Корень -> Лево -> Право
            cout << endl;
            break;
        case 3:   // Центрированный обход (In-order):
            inorder_recursive(root);   // Лево -> Корень -> Право
            cout << endl;
            break;
        case 4:   // Обратный обход (Post-order):
            postorder_recursive(root);    // Лево -> Право -> Корень
            cout << endl;
            break;
        case 5:
            printTreeVisual(root);
            break;
        default:
            break;
        }
    }

    // Сохранение дерева в файл (обход в ширину)
    void TSAVE(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }

        if (root == nullptr) {
            file.close();
            cout << "Дерево пустое, сохранён пустой файл: " << filename << endl;
            return;
        }

        // Используем очередь для обхода в ширину, чтобы сохранить
        // узлы в том же порядке, в котором они добавляются функцией TINSERT
        Queue<TreeNode<T>*> q;
        q.QPUSH(root);

        while (!q.empty()) {
            TreeNode<T>* current = q.QPOP();

            file << current->key << " ";   // Записываем ключ узла в файл

            if (current->left != nullptr) {
                q.QPUSH(current->left);
            }
            if (current->right != nullptr) {
                q.QPUSH(current->right);
            }
        }

        file.close();
        cout << "Полное бинарное дерево сохранено в файл: " << filename << endl;
    }

    // Загрузка дерева из файла
    void TLOAD(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        // Очищаем текущее дерево, чтобы избежать утечек памяти
        destroy_tree(root);
        root = nullptr;

        // Читаем значения из файла и вставляем их в дерево
        T value;
        while (file >> value) {
            // Используем существующую функцию TINSERT для построения дерева
            TINSERT(value);
        }

        file.close();
        cout << "Полное бинарное дерево загружено из файла: "
             << filename << endl;
    }

    auto GetRoot() const -> TreeNode<T>* {
        return root;
    }
};

#endif   // BINARY_TREE_HPP
