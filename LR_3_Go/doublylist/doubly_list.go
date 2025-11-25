package doublylist

import (
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"os"
)

// Node представляет узел двусвязного списка
type Node[T comparable] struct {
	Key  T
	Next *Node[T]
	Prev *Node[T]
}

// DoublyList представляет двусвязный список
type DoublyList[T comparable] struct {
	Head *Node[T]
	Tail *Node[T]
}

// NewDoublyList создает новый пустой список
func NewDoublyList[T comparable]() *DoublyList[T] {
	return &DoublyList[T]{}
}

// LCreate инициализирует список первым элементом
func (l *DoublyList[T]) LCreate(keyBegin T) error {
	if l.Head != nil {
		return errors.New("logic_error: List already created. Use push methods")
	}
	newNode := &Node[T]{Key: keyBegin}
	l.Head = newNode
	l.Tail = newNode
	return nil
}

// LPushHead добавляет элемент в начало
func (l *DoublyList[T]) LPushHead(key T) {
	newNode := &Node[T]{Key: key, Next: l.Head, Prev: nil}
	if l.Head != nil {
		l.Head.Prev = newNode
	} else {
		l.Tail = newNode
	}
	l.Head = newNode
}

// LPushBack добавляет элемент в конец
func (l *DoublyList[T]) LPushBack(key T) {
	newNode := &Node[T]{Key: key, Next: nil, Prev: l.Tail}
	if l.Tail != nil {
		l.Tail.Next = newNode
	} else {
		l.Head = newNode
	}
	l.Tail = newNode
}

// LDelHead удаляет первый элемент
func (l *DoublyList[T]) LDelHead() error {
	if l.Head == nil {
		return errors.New("underflow_error: Attempt to delete head in an empty list")
	}
	l.Head = l.Head.Next
	if l.Head != nil {
		l.Head.Prev = nil
	} else {
		l.Tail = nil
	}
	return nil
}

// LDelBack удаляет последний элемент
func (l *DoublyList[T]) LDelBack() error {
	if l.Tail == nil {
		return errors.New("underflow_error: Attempt to delete tail in an empty list")
	}
	l.Tail = l.Tail.Prev
	if l.Tail != nil {
		l.Tail.Next = nil
	} else {
		l.Head = nil
	}
	return nil
}

// LGetByValue ищет узел по значению
func (l *DoublyList[T]) LGetByValue(key T) *Node[T] {
	current := l.Head
	for current != nil {
		if current.Key == key {
			return current
		}
		current = current.Next
	}
	return nil
}

// LPushBefore вставляет элемент перед указанным значением
func (l *DoublyList[T]) LPushBefore(targetKey T, newKey T) error {
	targetNode := l.LGetByValue(targetKey)
	if targetNode == nil {
		return errors.New("invalid_argument: Target element for insertion (BEFORE) not found")
	}

	if targetNode == l.Head {
		l.LPushHead(newKey)
		return nil
	}

	newNode := &Node[T]{Key: newKey, Next: targetNode, Prev: targetNode.Prev}
	targetNode.Prev.Next = newNode
	targetNode.Prev = newNode
	return nil
}

// LPushAfter вставляет элемент после указанного значения
func (l *DoublyList[T]) LPushAfter(targetKey T, newKey T) error {
	targetNode := l.LGetByValue(targetKey)
	if targetNode == nil {
		return errors.New("invalid_argument: Target element for insertion (AFTER) not found")
	}

	if targetNode == l.Tail {
		l.LPushBack(newKey)
		return nil
	}

	newNode := &Node[T]{Key: newKey, Next: targetNode.Next, Prev: targetNode}
	targetNode.Next.Prev = newNode
	targetNode.Next = newNode
	return nil
}

// LDelAfter удаляет элемент после указанного значения
func (l *DoublyList[T]) LDelAfter(targetKey T) error {
	targetNode := l.LGetByValue(targetKey)
	if targetNode == nil {
		return errors.New("invalid_argument: Target element for deletion (AFTER) not found")
	}
	if targetNode.Next == nil {
		return errors.New("logic_error: No elements after the specified node")
	}

	nodeToDelete := targetNode.Next
	if nodeToDelete == l.Tail {
		return l.LDelBack()
	}

	targetNode.Next = nodeToDelete.Next
	if nodeToDelete.Next != nil {
		nodeToDelete.Next.Prev = targetNode
	}
	// nodeToDelete будет удален GC
	return nil
}

// LDelBefore удаляет элемент перед указанным значением
func (l *DoublyList[T]) LDelBefore(targetKey T) error {
	targetNode := l.LGetByValue(targetKey)
	if targetNode == nil {
		return errors.New("invalid_argument: Target element for deletion (BEFORE) not found")
	}
	if targetNode.Prev == nil {
		return errors.New("logic_error: No elements before the specified node (it is the head)")
	}

	nodeToDelete := targetNode.Prev
	if nodeToDelete == l.Head {
		return l.LDelHead()
	}

	targetNode.Prev = nodeToDelete.Prev
	if nodeToDelete.Prev != nil {
		nodeToDelete.Prev.Next = targetNode
	}
	return nil
}

// LDelByValue удаляет первое вхождение элемента по значению
func (l *DoublyList[T]) LDelByValue(key T) error {
	targetNode := l.LGetByValue(key)
	if targetNode == nil {
		return errors.New("invalid_argument: Element with such value not found")
	}

	if targetNode == l.Head {
		return l.LDelHead()
	}
	if targetNode == l.Tail {
		return l.LDelBack()
	}

	targetNode.Prev.Next = targetNode.Next
	targetNode.Next.Prev = targetNode.Prev
	return nil
}

// Clone создает глубокую копию списка
func (l *DoublyList[T]) Clone() *DoublyList[T] {
	newList := NewDoublyList[T]()
	current := l.Head
	for current != nil {
		newList.LPushBack(current.Key)
		current = current.Next
	}
	return newList
}

// Print выводит список.
func (l *DoublyList[T]) Print(w io.Writer, choice int) error {
	switch choice {
	case 1: // Forward
		if l.Head == nil {
			fmt.Fprintln(w, "Список пуст.")
			return nil
		}
		current := l.Head
		fmt.Fprint(w, "Голова -> ")
		for current != nil {
			fmt.Fprintf(w, "%v <-> ", current.Key)
			current = current.Next
		}
		fmt.Fprintln(w, "nullptr")
	case 2: // Backward
		if l.Tail == nil {
			fmt.Fprintln(w, "Список пуст.")
			return nil
		}
		current := l.Tail
		fmt.Fprint(w, "Хвост -> ")
		for current != nil {
			fmt.Fprintf(w, "%v <-> ", current.Key)
			current = current.Prev
		}
		fmt.Fprintln(w, "nullptr")
	default:
		return errors.New("invalid_argument: Invalid print mode selection")
	}
	return nil
}

// LSave сохраняет список в текстовый файл
func (l *DoublyList[T]) LSave(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	current := l.Head
	for current != nil {
		if _, err := fmt.Fprintf(file, "%v ", current.Key); err != nil {
			return err
		}
		current = current.Next
	}
	fmt.Printf("Двусвязный список сохранён в файл: %s\n", filename)
	return nil
}

// LLoad загружает список из текстового файла
func (l *DoublyList[T]) LLoad(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	// Очистка текущего списка
	l.Head = nil
	l.Tail = nil

	var value T
	for {
		_, err := fmt.Fscan(file, &value)
		if err != nil {
			if err == io.EOF {
				break
			}
			return err
		}
		l.LPushBack(value)
	}
	fmt.Printf("Двусвязный список загружен из файла: %s\n", filename)
	return nil
}

// LSaveBin сохраняет список в бинарный файл
func (l *DoublyList[T]) LSaveBin(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	current := l.Head
	for current != nil {
		// binary.Write работает корректно для фиксированных типов (int32, float64 и т.д.)
		if err := binary.Write(file, binary.LittleEndian, current.Key); err != nil {
			return err
		}
		current = current.Next
	}
	fmt.Printf("Двусвязный список сохранён в бинарный файл: %s\n", filename)
	return nil
}

// LLoadBin загружает список из бинарного файла
func (l *DoublyList[T]) LLoadBin(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	l.Head = nil
	l.Tail = nil

	var value T
	for {
		err := binary.Read(file, binary.LittleEndian, &value)
		if err != nil {
			if err == io.EOF {
				break
			}
			return err
		}
		l.LPushBack(value)
	}
	fmt.Printf("Двусвязный список загружен из бинарного файла: %s\n", filename)
	return nil
}
