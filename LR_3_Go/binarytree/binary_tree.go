package binarytree

import (
	"cmp"
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"os"
)

// Константы для визуализации
const (
	branchRight = "|--- "
	branchLeft  = "|___ "
)

// TreeNode представляет узел дерева
type TreeNode[T cmp.Ordered] struct {
	Key   T
	Left  *TreeNode[T]
	Right *TreeNode[T]
}

// FullBinaryTree представляет обертку над деревом
type FullBinaryTree[T cmp.Ordered] struct {
	root *TreeNode[T]
}

// NewFullBinaryTree создает новое пустое дерево
func NewFullBinaryTree[T cmp.Ordered]() *FullBinaryTree[T] {
	return &FullBinaryTree[T]{}
}

// GetRoot возвращает корень
func (t *FullBinaryTree[T]) GetRoot() *TreeNode[T] {
	return t.root
}

// Insert вставка элемента по принципу BST
func (t *FullBinaryTree[T]) Insert(value T) {
	newNode := &TreeNode[T]{Key: value}

	if t.root == nil {
		t.root = newNode
		return
	}

	current := t.root
	var parent *TreeNode[T]

	for current != nil {
		parent = current
		if value < current.Key {
			current = current.Left
		} else {
			current = current.Right
		}
	}

	if value < parent.Key {
		parent.Left = newNode
	} else {
		parent.Right = newNode
	}
}

// IsFull проверяет, является ли дерево полным
func (t *FullBinaryTree[T]) IsFull() bool {
	if t.root == nil {
		return true
	}
	return isFullRecursive(t.root)
}

func isFullRecursive[T cmp.Ordered](node *TreeNode[T]) bool {
	// Если ((left == nil) XOR (right == nil)) == true, то это неполное дерево
	hasLeft := node.Left != nil
	hasRight := node.Right != nil

	if hasLeft != hasRight {
		return false
	}

	if hasLeft && hasRight {
		return isFullRecursive(node.Left) && isFullRecursive(node.Right)
	}

	return true
}

// Clone создает глубокую копию дерева
func (t *FullBinaryTree[T]) Clone() *FullBinaryTree[T] {
	newTree := NewFullBinaryTree[T]()
	newTree.root = copyTreeRecursive(t.root)
	return newTree
}

func copyTreeRecursive[T cmp.Ordered](node *TreeNode[T]) *TreeNode[T] {
	if node == nil {
		return nil
	}
	newNode := &TreeNode[T]{Key: node.Key}
	newNode.Left = copyTreeRecursive(node.Left)
	newNode.Right = copyTreeRecursive(node.Right)
	return newNode
}

// Print - единый метод вывода.
// w - куда писать (os.Stdout для консоли или bytes.Buffer для тестов)
func (t *FullBinaryTree[T]) Print(choice int, w io.Writer) error {
	switch choice {
	case 1:
		t.printBreadthFirst(w)
	case 2:
		t.preOrderRecursive(t.root, w)
		fmt.Fprintln(w)
	case 3:
		t.inOrderRecursive(t.root, w)
		fmt.Fprintln(w)
	case 4:
		t.postOrderRecursive(t.root, w)
		fmt.Fprintln(w)
	case 5:
		t.printTreeVisual(t.root, w)
	default:
		return fmt.Errorf("invalid print operation code: %d", choice)
	}
	return nil
}

// Методы обхода

func (t *FullBinaryTree[T]) printBreadthFirst(w io.Writer) {
	if t.root == nil {
		return
	}
	// Простая реализация очереди на слайсе
	queue := []*TreeNode[T]{t.root}

	for len(queue) > 0 {
		current := queue[0]
		queue = queue[1:] // pop

		fmt.Fprintf(w, "%v ", current.Key)

		if current.Left != nil {
			queue = append(queue, current.Left)
		}
		if current.Right != nil {
			queue = append(queue, current.Right)
		}
	}
	fmt.Fprintln(w)
}

func (t *FullBinaryTree[T]) preOrderRecursive(node *TreeNode[T], w io.Writer) {
	if node != nil {
		fmt.Fprintf(w, "%v ", node.Key)
		t.preOrderRecursive(node.Left, w)
		t.preOrderRecursive(node.Right, w)
	}
}

func (t *FullBinaryTree[T]) inOrderRecursive(node *TreeNode[T], w io.Writer) {
	if node != nil {
		t.inOrderRecursive(node.Left, w)
		fmt.Fprintf(w, "%v ", node.Key)
		t.inOrderRecursive(node.Right, w)
	}
}

func (t *FullBinaryTree[T]) postOrderRecursive(node *TreeNode[T], w io.Writer) {
	if node != nil {
		t.postOrderRecursive(node.Left, w)
		t.postOrderRecursive(node.Right, w)
		fmt.Fprintf(w, "%v ", node.Key)
	}
}

// Визуализация

func (t *FullBinaryTree[T]) printTreeVisual(node *TreeNode[T], w io.Writer) {
	if node == nil {
		fmt.Fprintln(w, "Дерево пустое.")
		return
	}
	fmt.Fprintln(w, node.Key)
	printTreeVisualRecursive(node.Right, "", false, w)
	printTreeVisualRecursive(node.Left, "", true, w)
}

func printTreeVisualRecursive[T cmp.Ordered](node *TreeNode[T], prefix string, isLeft bool, w io.Writer) {
	if node != nil {
		fmt.Fprint(w, prefix)
		if isLeft {
			fmt.Fprint(w, branchLeft)
		} else {
			fmt.Fprint(w, branchRight)
		}
		fmt.Fprintln(w, node.Key)

		newPrefix := prefix
		if isLeft {
			newPrefix += "|   "
		} else {
			newPrefix += "    "
		}

		printTreeVisualRecursive(node.Right, newPrefix, false, w)
		printTreeVisualRecursive(node.Left, newPrefix, true, w)
	}
}

// Файловый ввод-вывод (Текстовый)

func (t *FullBinaryTree[T]) SaveText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("couldn't open the file for writing: %s (%w)", filename, err)
	}
	defer file.Close()

	if t.root == nil {
		return nil
	}

	queue := []*TreeNode[T]{t.root}
	for len(queue) > 0 {
		current := queue[0]
		queue = queue[1:]

		if _, err := fmt.Fprintf(file, "%v ", current.Key); err != nil {
			return fmt.Errorf("error writing data to file: %w", err)
		}

		if current.Left != nil {
			queue = append(queue, current.Left)
		}
		if current.Right != nil {
			queue = append(queue, current.Right)
		}
	}
	return nil
}

func (t *FullBinaryTree[T]) LoadText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("couldn't open the file for reading: %s (%w)", filename, err)
	}
	defer file.Close()

	// Очистка текущего дерева
	t.root = nil

	var value T
	for {
		// Fscan автоматически пропускает пробелы
		_, err := fmt.Fscan(file, &value)
		if err == io.EOF {
			break
		}
		if err != nil {
			return fmt.Errorf("critical I/O error when reading file: %w", err)
		}
		t.Insert(value)
	}
	return nil
}

// Файловый ввод-вывод (Бинарный)

func (t *FullBinaryTree[T]) SaveBinary(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("binary file could not be opened: %w", err)
	}
	defer file.Close()

	return serializeRecursive(t.root, file)
}

func serializeRecursive[T cmp.Ordered](node *TreeNode[T], w io.Writer) error {
	exists := node != nil
	// Пишем маркер (bool как int8/byte для простоты переносимости)
	var marker int8
	if exists {
		marker = 1
	}
	if err := binary.Write(w, binary.LittleEndian, marker); err != nil {
		return err
	}

	if exists {
		if err := binary.Write(w, binary.LittleEndian, node.Key); err != nil {
			return err
		}
		if err := serializeRecursive(node.Left, w); err != nil {
			return err
		}
		if err := serializeRecursive(node.Right, w); err != nil {
			return err
		}
	}
	return nil
}

func (t *FullBinaryTree[T]) LoadBinary(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("the binary file could not be opened: %w", err)
	}
	defer file.Close()

	t.root = nil
	var root *TreeNode[T]

	err = deserializeRecursive(&root, file)
	if err != nil {
		// В случае ошибки дерево остается пустым или частично заполненным
		t.root = nil
		return err
	}
	t.root = root
	return nil
}

func deserializeRecursive[T cmp.Ordered](node **TreeNode[T], r io.Reader) error {
	var marker int8
	if err := binary.Read(r, binary.LittleEndian, &marker); err != nil {
		if err == io.EOF {
			return nil // Нормальный конец рекурсии, если структура соблюдена
		}
		return err
	}

	if marker != 0 && marker != 1 {
		return fmt.Errorf("invalid file format: expected marker 0 or 1, got %d", marker)
	}

	if marker == 1 {
		var val T
		if err := binary.Read(r, binary.LittleEndian, &val); err != nil {
			return errors.New("unexpected end of file or node data reading error")
		}
		newNode := &TreeNode[T]{Key: val}
		*node = newNode
		if err := deserializeRecursive(&newNode.Left, r); err != nil {
			return err
		}
		if err := deserializeRecursive(&newNode.Right, r); err != nil {
			return err
		}
	} else {
		*node = nil
	}
	return nil
}
