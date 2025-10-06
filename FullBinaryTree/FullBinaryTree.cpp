#include <iostream>
#include <queue>
#include <utility>

using namespace std;

// Структура узла 
template<typename T>
struct TreeNode {
    T key;
    TreeNode<T>* left;
    TreeNode<T>* right;

    // Конструктор для создания узла
    TreeNode(T val) {
        key = val;
        left = nullptr;
        right = nullptr;
    }
};

// Рекурсивная функция для глубокого копирования дерева
template<typename T>
TreeNode<T>* copy_tree(const TreeNode<T>* other_node) {
    if (other_node == nullptr) {
        return nullptr;
    }
    TreeNode<T>* new_node = new TreeNode<T>(other_node->key);
    new_node->left = copy_tree(other_node->left);
    new_node->right = copy_tree(other_node->right);
    return new_node;
}

// Рекурсивная функция для удаления всех узлов
template<typename T>
void destroy_tree(TreeNode<T>* node) {
    if (node != nullptr) {
        destroy_tree(node->left);
        destroy_tree(node->right);
        delete node;
    }
}

template<typename T>
struct FullBinaryTree {
    TreeNode<T>* root;

    //Конструктор по умолчанию
    FullBinaryTree() {
        root = nullptr;
    }

    //Деструктор
    ~FullBinaryTree() {
        destroy_tree(root);
    }

    //Копирующий конструктор
    FullBinaryTree(const FullBinaryTree& other) {
        root = copy_tree(other.root);
    }

    //Копирующий оператор присваивания
    FullBinaryTree& operator=(const FullBinaryTree& other) {
        if (this != &other) { // Защита от самоприсваивания
            //Очищаем текущие ресурсы
            destroy_tree(root);
            //Копируем ресурсы из другого объекта
            root = copy_tree(other.root);
        }
        return *this;
    }
};

// Рекурсивные функцияы для обходов
template<typename T>
void preorder_recursive(TreeNode<T>* node) {
    if (node != nullptr) {
        cout << node->key << " ";
        preorder_recursive(node->left);
        preorder_recursive(node->right);
    }
}

template<typename T>
void inorder_recursive(TreeNode<T>* node) {
    if (node != nullptr) {
        inorder_recursive(node->left);
        cout << node->key << " ";
        inorder_recursive(node->right);
    }
}

template<typename T>
void postorder_recursive(TreeNode<T>* node) {
    if (node != nullptr) {
        postorder_recursive(node->left);
        postorder_recursive(node->right);
        cout << node->key << " ";
    }
}

// Вставка элемента
template<typename T>
void insert(FullBinaryTree<T>& tree, T value) {
    TreeNode<T>* new_node = new TreeNode<T>(value);
    if (tree.root == nullptr) {
        tree.root = new_node;
        return;
    }

    queue<TreeNode<T>*> q;
    q.push(tree.root);

    while (!q.empty()) {
        TreeNode<T>* current = q.front();
        q.pop();

        if (current->left == nullptr) {
            current->left = new_node;
            return;
        }
        else {
            q.push(current->left);
        }

        if (current->right == nullptr) {
            current->right = new_node;
            return;
        }
        else {
            q.push(current->right);
        }
    }
}

//Функция проверки на full
template<typename T>
bool isFull(TreeNode<T>* current) {
    if (((current->left == nullptr) ^ (current->right == nullptr)) == 1) {
        return false;
    }
    else if (current->left != nullptr && current->right != nullptr) {
        return isFull(current->left) && isFull(current->right);
    }
    return true;
}

// Прямой обход (Pre-order): Корень -> Лево -> Право
template<typename T>
void printPreorder(FullBinaryTree<T>& tree) {
    preorder_recursive(tree.root);
    cout << endl;
}

// Центрированный обход (In-order): Лево -> Корень -> Право
template<typename T>
void printInorder(FullBinaryTree<T>& tree) {
    inorder_recursive(tree.root);
    cout << endl;
}

// Обратный обход (Post-order): Лево -> Право -> Корень
template<typename T>
void printPostorder(FullBinaryTree<T>& tree) {
    postorder_recursive(tree.root);
    cout << endl;
}

// Обход в ширину (Level-order / Breadth-first)
template<typename T>
void printBreadthFirst(FullBinaryTree<T>& tree) {
    if (tree.root == nullptr) {
        return;
    }

    queue<TreeNode<T>*> q;
    q.push(tree.root);

    while (!q.empty()) {
        TreeNode<T>* current = q.front();
        q.pop();

        cout << current->key << " ";

        if (current->left != nullptr) {
            q.push(current->left);
        }
        if (current->right != nullptr) {
            q.push(current->right);
        }
    }
    cout << endl;
}


int main() {
    setlocale(LC_ALL, "ru");

    FullBinaryTree<int> tree;
    for (int i = 1; i <= 11; i++) {
        insert(tree, i);
    }

    cout << "Дерево является полным (Full)? " << (isFull(tree.root) ? "Да" : "Нет") << endl << endl;

    cout << "Обход в ширину (Level-order):" << endl;
    printBreadthFirst(tree);
    cout << "--------------------------------" << endl;

    cout << "Прямой обход (Pre-order):" << endl;
    printPreorder(tree);
    cout << "--------------------------------" << endl;

    cout << "Центрированный обход (In-order):" << endl;
    printInorder(tree);
    cout << "--------------------------------" << endl;

    cout << "Обратный обход (Post-order):" << endl;
    printPostorder(tree);
    cout << "--------------------------------" << endl;

    // Демонстрация работы копирующего конструктора и оператора присваивания
    cout << "Создаем копию дерева..." << endl;
    FullBinaryTree<int> tree_copy = tree;
    cout << "Обход копии в ширину:" << endl;
    printBreadthFirst(tree_copy);

    return 0;
}