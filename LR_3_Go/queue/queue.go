package queue

import (
	"encoding/gob"
	"errors"
	"fmt"
	"io"
	"os"
)

// Ошибки, которые может вернуть очередь
var (
	ErrQueueEmpty  = errors.New("queue is empty")
	ErrFileCorrupt = errors.New("file corrupted or invalid data size")
)

// Queue реализует кольцевой буфер
type Queue[T any] struct {
	data     []T
	head     int
	tail     int
	count    int // Текущее количество элементов (size)
	capacity int // Вместимость
}

// NewQueue создает новую очередь с заданной начальной вместимостью
func NewQueue[T any](initialCap int) *Queue[T] {
	if initialCap < 1 {
		initialCap = 1
	}
	return &Queue[T]{
		data:     make([]T, initialCap),
		capacity: initialCap,
		head:     0,
		tail:     0,
		count:    0,
	}
}

// resize увеличивает размер внутреннего массива в 2 раза
func (q *Queue[T]) resize() {
	newCapacity := q.capacity * 2
	if newCapacity == 0 {
		newCapacity = 1
	}
	newData := make([]T, newCapacity)

	// Копируем элементы, "распрямляя" кольцо
	for i := 0; i < q.count; i++ {
		newData[i] = q.data[(q.head+i)%q.capacity]
	}

	q.data = newData
	q.capacity = newCapacity
	q.head = 0
	q.tail = q.count
}

// Push добавляет элемент в конец очереди
func (q *Queue[T]) Push(value T) {
	if q.count >= q.capacity {
		q.resize()
	}
	q.data[q.tail] = value
	q.tail = (q.tail + 1) % q.capacity
	q.count++
}

// Pop извлекает элемент из начала очереди
func (q *Queue[T]) Pop() (T, error) {
	var empty T
	if q.count == 0 {
		return empty, ErrQueueEmpty
	}

	value := q.data[q.head]
	var zero T
	q.data[q.head] = zero

	q.head = (q.head + 1) % q.capacity
	q.count--
	return value, nil
}

// Get возвращает первый элемент без удаления
func (q *Queue[T]) Get() (T, error) {
	if q.count == 0 {
		var empty T
		return empty, ErrQueueEmpty
	}
	return q.data[q.head], nil
}

// IsEmpty проверяет, пуста ли очередь
func (q *Queue[T]) IsEmpty() bool {
	return q.count == 0
}

// Size возвращает количество элементов
func (q *Queue[T]) Size() int {
	return q.count
}

// Clone создает глубокую копию очереди
func (q *Queue[T]) Clone() *Queue[T] {
	newQ := &Queue[T]{
		capacity: q.capacity,
		count:    q.count,
		head:     q.head,
		tail:     q.tail,
		data:     make([]T, q.capacity),
	}
	copy(newQ.data, q.data)
	return newQ
}

// Print выводит содержимое очереди
func (q *Queue[T]) Print(w io.Writer) {
	fmt.Fprint(w, "Содержимое очереди (от головы к хвосту): ")
	if q.count == 0 {
		fmt.Fprintln(w, "пусто")
		return
	}
	for i := 0; i < q.count; i++ {
		val := q.data[(q.head+i)%q.capacity]
		fmt.Fprintf(w, "%v ", val)
	}
	fmt.Fprintln(w)
}

// Сериализация (Text)

// SaveText сохраняет очередь в текстовый файл.
func (q *Queue[T]) SaveText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	// Записываем размер
	if _, err := fmt.Fprintln(file, q.count); err != nil {
		return err
	}

	// Записываем элементы
	for i := 0; i < q.count; i++ {
		val := q.data[(q.head+i)%q.capacity]
		if _, err := fmt.Fprintln(file, val); err != nil {
			return err
		}
	}
	return nil
}

// LoadText загружает очередь из текстового файла
func (q *Queue[T]) LoadText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	var newSize int
	if _, err := fmt.Fscan(file, &newSize); err != nil {
		return fmt.Errorf("error reading size: %w", err)
	}

	// Сбрасываем текущую очередь
	q.head = 0
	q.tail = 0
	q.count = 0
	// capacity оставляем или можем пересоздать slice, если нужно

	for i := 0; i < newSize; i++ {
		var val T
		// fmt.Fscan работает хорошо для базовых типов (int, string, float)
		// Для сложных структур потребуется кастомная логика.
		if _, err := fmt.Fscan(file, &val); err != nil {
			// Если достигли конца файла раньше времени или ошибка парсинга
			return ErrFileCorrupt
		}
		q.Push(val)
	}

	if q.count != newSize {
		return ErrFileCorrupt
	}

	return nil
}

// Сериализация (Binary)

// SaveBinary сохраняет данные
func (q *Queue[T]) SaveBinary(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	encoder := gob.NewEncoder(file)

	// Пишем размер
	if err := encoder.Encode(q.count); err != nil {
		return err
	}

	// Пишем элементы в логическом порядке
	for i := 0; i < q.count; i++ {
		val := q.data[(q.head+i)%q.capacity]
		if err := encoder.Encode(val); err != nil {
			return err
		}
	}
	return nil
}

// LoadBinary загружает данные используя encoding/gob
func (q *Queue[T]) LoadBinary(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	decoder := gob.NewDecoder(file)

	var newSize int
	if err := decoder.Decode(&newSize); err != nil {
		return fmt.Errorf("error reading header: %w", err)
	}

	// Сброс
	q.head = 0
	q.tail = 0
	q.count = 0

	for i := 0; i < newSize; i++ {
		var val T
		if err := decoder.Decode(&val); err != nil {
			if err == io.EOF {
				return ErrFileCorrupt
			}
			return err
		}
		q.Push(val)
	}

	return nil
}
