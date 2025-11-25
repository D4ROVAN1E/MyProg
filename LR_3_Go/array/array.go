package array

import (
	"encoding/binary"
	"fmt"
	"os"
)

// Array представляет собой динамический массив с дженериками
type Array[T any] struct {
	data []T
}

// NewArray создает пустой массив
func NewArray[T any]() *Array[T] {
	return &Array[T]{
		data: make([]T, 0, 1), // size 0, capacity 1
	}
}

// NewArrayWithCap создает массив с заданной емкостью
func NewArrayWithCap[T any](cap int) *Array[T] {
	if cap < 1 {
		cap = 1
	}
	// Создаем слайс длиной cap-1 и емкостью cap
	return &Array[T]{
		data: make([]T, cap-1, cap),
	}
}

// GetSize возвращает текущий размер
func (a *Array[T]) GetSize() int {
	return len(a.data)
}

// GetCapacity возвращает текущую емкость
func (a *Array[T]) GetCapacity() int {
	return cap(a.data)
}

// PushBack добавляет элемент в конец
func (a *Array[T]) PushBack(value T) {
	// Go append автоматически управляет capacity (удваивает при необходимости)
	a.data = append(a.data, value)
}

// Get возвращает элемент по индексу или ошибку
func (a *Array[T]) Get(index int) (T, error) {
	if index < 0 || index >= len(a.data) {
		var empty T
		return empty, fmt.Errorf("error: Index %d is out of bounds (size %d)", index, len(a.data))
	}
	return a.data[index], nil
}

// Set заменяет элемент по индексу
func (a *Array[T]) Set(index int, value T) error {
	if index < 0 || index >= len(a.data) {
		return fmt.Errorf("error: Index %d is out of bounds", index)
	}
	a.data[index] = value
	return nil
}

// InsertByInd вставляет элемент по индексу.
func (a *Array[T]) InsertByInd(index int, value T) error {
	if index < 0 || index > len(a.data) {
		return fmt.Errorf("error: Index %d is out of bounds for insertion", index)
	}
	// Трюк со слайсами для вставки элемента
	if index == len(a.data) {
		a.PushBack(value)
		return nil
	}
	a.data = append(a.data[:index+1], a.data[index:]...) // Раздвигаем
	a.data[index] = value
	return nil
}

// DeleteByInd удаляет элемент по индексу
func (a *Array[T]) DeleteByInd(index int) error {
	if index < 0 || index >= len(a.data) {
		return fmt.Errorf("error: Index %d is out of bounds for deletion", index)
	}
	a.data = append(a.data[:index], a.data[index+1:]...)
	return nil
}

// SwapByInd заменяет значение
func (a *Array[T]) SwapByInd(index int, value T) error {
	if index < 0 || index >= len(a.data) {
		return fmt.Errorf("error: Index %d is out of bounds for swap", index)
	}
	a.data[index] = value
	return nil
}

// Clone создает глубокую копию массива
func (a *Array[T]) Clone() *Array[T] {
	newData := make([]T, len(a.data), cap(a.data))
	copy(newData, a.data)
	return &Array[T]{data: newData}
}

// SetCapacity изменяет емкость массива вручную
func (a *Array[T]) SetCapacity(newCap int) error {
	if newCap < len(a.data) {
		return fmt.Errorf("error: New capacity cannot be smaller than current size")
	}
	// Создаем новый слайс и копируем данные
	newData := make([]T, len(a.data), newCap)
	copy(newData, a.data)
	a.data = newData
	return nil
}

// SetSize изменяет размер массива
func (a *Array[T]) SetSize(newSize int) error {
	if newSize > cap(a.data) {
		return fmt.Errorf("error: New size exceeds current capacity")
	}
	a.data = a.data[:newSize]
	return nil
}

// SaveText сохраняет массив в текстовый файл
func (a *Array[T]) SaveText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error: Unable to open file for writing: %v", err)
	}
	defer file.Close()

	fmt.Fprintln(file, len(a.data))
	for _, val := range a.data {
		// Fprint работает с любым типом, поддерживающим форматирование
		fmt.Fprintf(file, "%v ", val)
	}
	return nil
}

// LoadText загружает массив из текстового файла
func (a *Array[T]) LoadText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error: Unable to open file for reading: %v", err)
	}
	defer file.Close()

	var newSize int
	if _, err := fmt.Fscan(file, &newSize); err != nil {
		return fmt.Errorf("error: Failed to read size")
	}

	// Сброс массива
	a.data = make([]T, 0, newSize)

	for i := 0; i < newSize; i++ {
		var val T
		// Fscan требует, чтобы T был сканируемым (int, float, string и т.д.)
		if _, err := fmt.Fscan(file, &val); err != nil {
			// Если данные кончились раньше времени
			break
		}
		a.data = append(a.data, val)
	}

	if len(a.data) != newSize {
		return fmt.Errorf("error: File corrupted or incomplete data")
	}
	return nil
}

// SaveBinary сохраняет массив в бинарном формате.
func (a *Array[T]) SaveBinary(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error: Unable to open file: %v", err)
	}
	defer file.Close()

	size := int32(len(a.data)) // Используем int32 для совместимости
	if err := binary.Write(file, binary.LittleEndian, size); err != nil {
		return err
	}

	if size > 0 {
		if err := binary.Write(file, binary.LittleEndian, a.data); err != nil {
			return fmt.Errorf("error: Write operation failed (type might not be fixed-size): %v", err)
		}
	}
	return nil
}

// LoadBinary загружает массив из бинарного файла.
func (a *Array[T]) LoadBinary(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error: Unable to open file: %v", err)
	}
	defer file.Close()

	var newSize int32
	if err := binary.Read(file, binary.LittleEndian, &newSize); err != nil {
		return fmt.Errorf("error: Failed to read size")
	}

	a.data = make([]T, newSize)
	if newSize > 0 {
		if err := binary.Read(file, binary.LittleEndian, a.data); err != nil {
			return fmt.Errorf("error: Failed to read data (incomplete or type mismatch): %v", err)
		}
	}
	return nil
}
