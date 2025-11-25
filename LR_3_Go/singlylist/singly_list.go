package singlylist

import (
	"bytes"
	"encoding/gob"
	"fmt"
	"io"
	"os"
)

// SNode представляет узел списка
type SNode[T any] struct {
	Key  T
	Next *SNode[T]
}

// ForwardList реализует односвязный список
type ForwardList[T comparable] struct {
	Head *SNode[T]
}

// NewForwardList создает новый пустой список
func NewForwardList[T comparable]() *ForwardList[T] {
	return &ForwardList[T]{Head: nil}
}

// DeepCopy создает глубокую копию списка
func (l *ForwardList[T]) DeepCopy() *ForwardList[T] {
	newList := NewForwardList[T]()
	if l.Head == nil {
		return newList
	}

	newList.Head = &SNode[T]{Key: l.Head.Key, Next: nil}
	currentThis := newList.Head
	currentOther := l.Head.Next

	for currentOther != nil {
		currentThis.Next = &SNode[T]{Key: currentOther.Key, Next: nil}
		currentThis = currentThis.Next
		currentOther = currentOther.Next
	}
	return newList
}

// PushForward добавляет узел ПОСЛЕ указанного узла ptr
func (l *ForwardList[T]) PushForward(ptr *SNode[T], key T) error {
	if ptr == nil {
		return fmt.Errorf("pointer cannot be null")
	}
	newNode := &SNode[T]{Key: key, Next: ptr.Next}
	ptr.Next = newNode
	return nil
}

// Create создает список с начальным элементом (сбрасывает текущий)
func (l *ForwardList[T]) Create(keyBegin T) {
	l.Head = &SNode[T]{Key: keyBegin, Next: nil}
}

// Print выводит список в консоль
func (l *ForwardList[T]) Print() {
	if l.Head == nil {
		fmt.Println("Список пуст")
		return
	}
	current := l.Head
	for current != nil {
		fmt.Printf("%v -> ", current.Key)
		current = current.Next
	}
	fmt.Println("nil")
}

// GetPrintString возвращает строковое представление (для тестов)
func (l *ForwardList[T]) GetPrintString() string {
	if l.Head == nil {
		return "Список пуст\n"
	}
	var buffer bytes.Buffer
	current := l.Head
	for current != nil {
		buffer.WriteString(fmt.Sprintf("%v -> ", current.Key))
		current = current.Next
	}
	buffer.WriteString("nil\n")
	return buffer.String()
}

// PushHead добавляет элемент в НАЧАЛО списка
func (l *ForwardList[T]) PushHead(key T) {
	newNode := &SNode[T]{Key: key, Next: l.Head}
	l.Head = newNode
}

// PushBack добавляет элемент в КОНЕЦ списка
func (l *ForwardList[T]) PushBack(key T) {
	newNode := &SNode[T]{Key: key, Next: nil}
	if l.Head == nil {
		l.Head = newNode
		return
	}
	current := l.Head
	for current.Next != nil {
		current = current.Next
	}
	current.Next = newNode
}

// PushBefore добавляет элемент ДО узла с заданным значением
func (l *ForwardList[T]) PushBefore(targetKey T, newKey T) error {
	if l.Head == nil {
		return fmt.Errorf("list is empty, cannot find element")
	}

	if l.Head.Key == targetKey {
		l.PushHead(newKey)
		return nil
	}

	current := l.Head
	for current.Next != nil && current.Next.Key != targetKey {
		current = current.Next
	}

	if current.Next != nil {
		return l.PushForward(current, newKey)
	}
	return fmt.Errorf("target key not found")
}

// DelHead удаляет первый элемент списка
func (l *ForwardList[T]) DelHead() error {
	if l.Head == nil {
		return fmt.Errorf("attempt to delete from an empty list")
	}
	l.Head = l.Head.Next
	return nil
}

// DelBack удаляет последний элемент списка
func (l *ForwardList[T]) DelBack() error {
	if l.Head == nil {
		return fmt.Errorf("attempt to delete from an empty list")
	}

	if l.Head.Next == nil {
		l.Head = nil
		return nil
	}

	current := l.Head
	for current.Next.Next != nil {
		current = current.Next
	}
	current.Next = nil
	return nil
}

// DelAfter удаляет элемент ПОСЛЕ указанного узла
func (l *ForwardList[T]) DelAfter(ptr *SNode[T]) error {
	if ptr == nil {
		return fmt.Errorf("pointer cannot be null")
	}
	if ptr.Next == nil {
		return fmt.Errorf("no element exists after the specified node")
	}
	ptr.Next = ptr.Next.Next
	return nil
}

// DelBefore удаляет элемент ДО узла с заданным значением
func (l *ForwardList[T]) DelBefore(targetKey T) error {
	if l.Head == nil {
		return fmt.Errorf("list is empty")
	}
	if l.Head.Key == targetKey {
		return fmt.Errorf("cannot delete element before the head of the list")
	}
	if l.Head.Next == nil {
		return fmt.Errorf("list is too short or element not found")
	}
	// Если целевой элемент - второй, нужно удалить голову
	if l.Head.Next.Key == targetKey {
		return l.DelHead()
	}

	current := l.Head
	found := false
	// Ищем узел за два шага до целевого
	for current.Next != nil && current.Next.Next != nil {
		if current.Next.Next.Key == targetKey {
			l.DelAfter(current)
			found = true
			return nil
		}
		current = current.Next
	}

	if !found {
		return fmt.Errorf("target element not found")
	}
	return nil
}

// DelByValue удаляет узел по значению (первое вхождение)
func (l *ForwardList[T]) DelByValue(key T) error {
	if l.Head == nil {
		return fmt.Errorf("list is empty")
	}

	if l.Head.Key == key {
		return l.DelHead()
	}

	current := l.Head
	for current.Next != nil && current.Next.Key != key {
		current = current.Next
	}

	if current.Next != nil {
		current.Next = current.Next.Next
		return nil
	}
	return fmt.Errorf("element with specified value not found")
}

// GetByValue ищет элемент по значению
func (l *ForwardList[T]) GetByValue(key T) *SNode[T] {
	current := l.Head
	for current != nil {
		if current.Key == key {
			return current
		}
		current = current.Next
	}
	return nil
}

// Save сохраняет список в текстовый файл
func (l *ForwardList[T]) Save(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error opening file for writing: %v", err)
	}
	defer file.Close()

	current := l.Head
	for current != nil {
		if _, err := fmt.Fprintf(file, "%v ", current.Key); err != nil {
			return err
		}
		current = current.Next
	}
	return nil
}

// Load загружает список из текстового файла
func (l *ForwardList[T]) Load(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error opening file for reading: %v", err)
	}
	defer file.Close()

	// Очищаем список
	l.Head = nil

	var value T
	first := true
	for {
		_, err := fmt.Fscan(file, &value)
		if err == io.EOF {
			break
		}
		if err != nil {
			return fmt.Errorf("invalid data format or read error")
		}

		if first {
			l.Create(value)
			first = false
		} else {
			l.PushBack(value)
		}
	}
	return nil
}

// Serialize сохраняет список в бинарный формат (gob)
func (l *ForwardList[T]) Serialize(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error opening file for writing: %v", err)
	}
	defer file.Close()

	var values []T
	current := l.Head
	for current != nil {
		values = append(values, current.Key)
		current = current.Next
	}

	encoder := gob.NewEncoder(file)
	if err := encoder.Encode(values); err != nil {
		return fmt.Errorf("error writing binary data: %v", err)
	}
	return nil
}

// Deserialize загружает список из бинарного формата (gob)
func (l *ForwardList[T]) Deserialize(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error opening file for reading: %v", err)
	}
	defer file.Close()

	l.Head = nil
	var values []T
	decoder := gob.NewDecoder(file)
	if err := decoder.Decode(&values); err != nil {
		// Если файл пуст или EOF, это нормально, просто список будет пуст
		if err == io.EOF {
			return nil
		}
		return fmt.Errorf("error reading binary data: %v", err)
	}

	for i, val := range values {
		if i == 0 {
			l.Create(val)
		} else {
			l.PushBack(val)
		}
	}
	return nil
}
