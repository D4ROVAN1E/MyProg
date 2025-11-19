#ifndef BINARY_TREE_HPP
#define BINARY_TREE_HPP

#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept> 
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

            // Рекурсивно вызываем для потомков
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

    // Рекурсивные функции для обходов
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

    // Вспомогательная функция для сохранения в бинарном формате (Pre-order)
    void serialize_recursive(TreeNode<T>* node, ofstream& out) const {
        // Маркер существования узла: true - есть узел, false - nullptr
        bool exists = (node != nullptr);
        out.write(reinterpret_cast<const char*>(&exists), sizeof(bool));
        
        // Если запись не удалась (например, диск переполнен)
        if (!out) {
            throw runtime_error("Error writing to a binary file (perhaps there is no disk space).");
        }

        if (exists) {
            // Сохраняем данные узла
            out.write(reinterpret_cast<const char*>(&node->key), sizeof(T));
            if (!out) {
                 throw runtime_error("Error writing node data to a binary file.");
            }
            serialize_recursive(node->left, out);
            // Рекурсивно сохраняем правое поддерево
            serialize_recursive(node->right, out);
        }
    }

    // Вспомогательная функция для загрузки из бинарного формата
    void deserialize_recursive(TreeNode<T>*& node, ifstream& in) {
        // Используем однобайтовый тип для чтения
        uint8_t marker = 0; 
        
        // Читаем маркер (1 байт)
        if (!in.read(reinterpret_cast<char*>(&marker), sizeof(uint8_t))) {
            if (in.fail() && !in.eof()) throw runtime_error("Error reading file structure.");
            return; 
        }

        // Если байт не 0 и не 1, значит файл поврежден или это не тот формат
        if (marker != 0 && marker != 1) {
            throw runtime_error("Error: Invalid file format. Expected binary marker (0 or 1), got: " + to_string((int)marker));
        }

        bool exists = (marker == 1);

        if (!exists) {
            node = nullptr;
        } else {
            T value;
            // Читаем данные. Если маркер сказал "узел есть", а данных нет - это ошибка.
            if (!in.read(reinterpret_cast<char*>(&value), sizeof(T))) {
                throw runtime_error("Unexpected end of file or node data reading error.");
            }
            
            node = new TreeNode<T>(value);
            deserialize_recursive(node->left, in);
            deserialize_recursive(node->right, in);
        }
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
        if (this != &other) {
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
            parent = current;
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
        if (root == nullptr) return true; // Пустое дерево считается полным
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
        case 2:
            preorder_recursive(root);
            cout << endl;
            break;
        case 3:
            inorder_recursive(root);
            cout << endl;
            break;
        case 4:
            postorder_recursive(root);
            cout << endl;
            break;
        case 5:
            printTreeVisual(root);
            break;
        default:
            throw invalid_argument("Invalid print operation code: " + to_string(choise));
        }
    }

    // Сохранение дерева в файл
    void TSAVE(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Couldn't open the file for writing: " + filename);
        }

        if (root == nullptr) {
            file.close();
            // Можно просто выйти, создав пустой файл
            return;
        }

        Queue<TreeNode<T>*> q;
        q.QPUSH(root);

        while (!q.empty()) {
            TreeNode<T>* current = q.QPOP();
            file << current->key << " "; 
            
            // Проверка на ошибки записи (например, место кончилось)
            if (file.fail()) {
                throw runtime_error("Error when writing data to a file: " + filename);
            }

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
            throw runtime_error("Couldn't open the file for reading: " + filename);
        }

        destroy_tree(root);
        root = nullptr;

        T value;
        while (file >> value) {
            TINSERT(value);
        }

        if (file.bad()) {
             throw runtime_error("Critical I/O error when reading a file: " + filename);
        }

        file.close();
        cout << "Полное бинарное дерево загружено из файла: "
             << filename << endl;
    }

    // Сохранение в бинарный файл
    void TSAVE_BINARY(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Binary file could not be opened for writing: " + filename);
        }

        serialize_recursive(root, file);
        
        file.close();
        cout << "Дерево сохранено в бинарный файл: " << filename << endl;
    }

    // Загрузка из бинарного файла
    void TLOAD_BINARY(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("The binary file could not be opened for reading: " + filename);
        }

        destroy_tree(root);
        root = nullptr;

        try {
            deserialize_recursive(root, file);
        } catch (...) {
            // В случае ошибки при загрузке очищаем частично загруженное дерево
            destroy_tree(root);
            root = nullptr;
            file.close();
            throw; // Пробрасываем ошибку дальше
        }

        file.close();
        cout << "Дерево загружено из бинарного файла: " << filename << endl;
    }

    auto GetRoot() const -> TreeNode<T>* {
        return root;
    }
};

#endif   // BINARY_TREE_HPP
