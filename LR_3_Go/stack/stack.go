package stack

import (
	"encoding/gob"
	"errors"
	"fmt"
	"os"
)

// Stack реализует структуру данных стек
type Stack[T any] struct {
	data []T
}

// NewStack создает новый пустой стек.
func NewStack[T any]() *Stack[T] {
	return &Stack[T]{
		data: make([]T, 0, 1),
	}
}

// NewStackWithCapacity создает стек с заданной начальной емкостью.
func NewStackWithCapacity[T any](cap int) (*Stack[T], error) {
	if cap <= 0 {
		return nil, errors.New("initial capacity must be greater than 0")
	}
	return &Stack[T]{
		data: make([]T, 0, cap),
	}, nil
}

// Push добавляет элемент в стек.
func (s *Stack[T]) Push(value T) {
	s.data = append(s.data, value)
}

// Pop извлекает элемент с вершины стека.
func (s *Stack[T]) Pop() (T, error) {
	var zero T
	if len(s.data) == 0 {
		return zero, errors.New("stack underflow: cannot pop from an empty stack")
	}

	index := len(s.data) - 1
	val := s.data[index]

	// Удаляем элемент из слайса (срезаем)
	s.data = s.data[:index]

	return val, nil
}

// Size возвращает текущее количество элементов.
func (s *Stack[T]) Size() int {
	return len(s.data)
}

// Print выводит содержимое стека в консоль.
func (s *Stack[T]) Print() {
	for _, v := range s.data {
		fmt.Printf("%v ", v)
	}
	fmt.Println()
}

// Clone создает глубокую копию стека
func (s *Stack[T]) Clone() *Stack[T] {
	newData := make([]T, len(s.data), cap(s.data))
	copy(newData, s.data)
	return &Stack[T]{data: newData}
}

// SaveText сохраняет стек в текстовый файл
func (s *Stack[T]) SaveText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("could not open file for writing: %w", err)
	}
	defer file.Close()

	// Записываем размер
	if _, err := fmt.Fprintln(file, len(s.data)); err != nil {
		return err
	}

	// Записываем данные через пробел
	for _, v := range s.data {
		if _, err := fmt.Fprintf(file, "%v ", v); err != nil {
			return err
		}
	}
	fmt.Println("Стек сохранён в файл:", filename)
	return nil
}

// LoadText загружает стек из текстового файла
func (s *Stack[T]) LoadText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("could not open file for reading: %w", err)
	}
	defer file.Close()

	var size int
	if _, err := fmt.Fscan(file, &size); err != nil {
		return fmt.Errorf("failed to read stack size: %w", err)
	}

	// Очищаем текущий стек и выделяем память
	s.data = make([]T, 0, size)

	for i := 0; i < size; i++ {
		var val T
		// Fscan автоматически парсит типы, разделенные пробелами
		if _, err := fmt.Fscan(file, &val); err != nil {
			return fmt.Errorf("failed to read data at index %d: %w", i, err)
		}
		s.Push(val)
	}

	fmt.Println("Стек загружен из файла:", filename)
	return nil
}

// SaveBinary сохраняет стек в бинарном формате
func (s *Stack[T]) SaveBinary(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("could not open binary file for writing: %w", err)
	}
	defer file.Close()

	encoder := gob.NewEncoder(file)
	if err := encoder.Encode(s.data); err != nil {
		return fmt.Errorf("failed to encode data: %w", err)
	}

	fmt.Println("Стек сохранён (bin):", filename)
	return nil
}

// LoadBinary загружает стек из бинарного файла
func (s *Stack[T]) LoadBinary(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("could not open binary file for reading: %w", err)
	}
	defer file.Close()

	decoder := gob.NewDecoder(file)
	var newData []T
	if err := decoder.Decode(&newData); err != nil {
		return fmt.Errorf("failed to decode data: %w", err)
	}

	s.data = newData
	fmt.Println("Стек загружен (bin):", filename)
	return nil
}
