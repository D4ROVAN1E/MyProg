package dhash

import (
	"encoding/binary"
	"fmt"
	"io"
	"math"
	"os"
)

// HashNode представляет узел хеш-таблицы
type HashNode[T any] struct {
	Key        string
	Value      T
	IsOccupied bool
}

// DoubleHash реализует хеш-таблицу с двойным хешированием
type DoubleHash[T any] struct {
	table         []HashNode[T]
	tableSize     uint32
	elementsCount uint32
}

// NewDoubleHash создает новую таблицу заданного размера
func NewDoubleHash[T any](size uint32) (*DoubleHash[T], error) {
	if size == 0 {
		return nil, fmt.Errorf("table size cannot be zero")
	}
	return &DoubleHash[T]{
		table:         make([]HashNode[T], size+1),
		tableSize:     size,
		elementsCount: 0,
	}, nil
}

// hash1 реализует метод умножения (золотое сечение)
func (dh *DoubleHash[T]) hash1(key string) uint32 {
	var numKey uint64 = 0
	for _, c := range []byte(key) {
		numKey = numKey*31 + uint64(c)
	}

	const A = (2.2360679775 - 1.0) / 2.0 // (sqrt(5) - 1) / 2
	temp := float64(numKey) * A
	temp = temp - math.Floor(temp) // Дробная часть

	return uint32(math.Floor(float64(dh.tableSize) * temp))
}

// hash2 реализует метод свертки
func (dh *DoubleHash[T]) hash2(key string) uint32 {
	var sum uint32 = 0
	for _, c := range []byte(key) {
		sum += uint32(c)
	}

	result := (sum % (dh.tableSize - 1)) + 1

	// Делаем результат нечетным, если размер таблицы четный
	if dh.tableSize%2 == 0 && result%2 == 0 {
		result++
	}
	return result
}

// needResize проверяет load factor > 0.7
func (dh *DoubleHash[T]) needResize() bool {
	if dh.tableSize == 0 {
		return true
	}
	return (float64(dh.elementsCount) / float64(dh.tableSize)) > 0.7
}

// resize увеличивает таблицу и перехеширует элементы
func (dh *DoubleHash[T]) resize() {
	oldTable := dh.table
	oldSize := dh.tableSize

	dh.tableSize = dh.tableSize*2 + 1
	dh.table = make([]HashNode[T], dh.tableSize+1)
	dh.elementsCount = 0

	for i := uint32(0); i < oldSize; i++ {
		if oldTable[i].IsOccupied {
			_ = dh.Insert(oldTable[i].Key, oldTable[i].Value)
		}
	}
}

// Insert вставляет элемент или обновляет значение
func (dh *DoubleHash[T]) Insert(key string, value T) error {
	if dh.needResize() {
		dh.resize()
	}

	h1 := dh.hash1(key)
	h2 := dh.hash2(key)
	var i uint32 = 0

	for i < dh.tableSize {
		index := (h1 + i*h2) % dh.tableSize

		// Если ячейка свободна
		if !dh.table[index].IsOccupied {
			dh.table[index] = HashNode[T]{Key: key, Value: value, IsOccupied: true}
			dh.elementsCount++
			return nil
		}

		// Если ключ совпадает, обновляем значение
		if dh.table[index].Key == key {
			dh.table[index].Value = value
			return nil
		}

		i++
	}

	return fmt.Errorf("error: Hash table is full, cannot insert key")
}

// Find ищет элемент по ключу. Возвращает указатель на значение или nil
func (dh *DoubleHash[T]) Find(key string) *T {
	if dh.elementsCount == 0 {
		return nil
	}

	h1 := dh.hash1(key)
	h2 := dh.hash2(key)
	var i uint32 = 0

	for i < dh.tableSize {
		index := (h1 + i*h2) % dh.tableSize

		if !dh.table[index].IsOccupied {
			return nil
		}

		if dh.table[index].Key == key {
			return &dh.table[index].Value
		}
		i++
	}
	return nil
}

// Remove удаляет элемент по ключу
func (dh *DoubleHash[T]) Remove(key string) bool {
	if dh.elementsCount == 0 {
		return false
	}

	h1 := dh.hash1(key)
	h2 := dh.hash2(key)
	var i uint32 = 0

	for i < dh.tableSize {
		index := (h1 + i*h2) % dh.tableSize

		if !dh.table[index].IsOccupied {
			return false
		}

		if dh.table[index].Key == key {
			dh.table[index].IsOccupied = false
			// В Go нужно занулить значения, чтобы сборщик мусора мог очистить память
			var empty T
			dh.table[index].Value = empty
			dh.table[index].Key = ""
			dh.elementsCount--
			return true
		}
		i++
	}
	return false
}

// Size возвращает количество элементов
func (dh *DoubleHash[T]) Size() uint32 {
	return dh.elementsCount
}

// Empty проверяет, пуста ли таблица
func (dh *DoubleHash[T]) Empty() bool {
	return dh.elementsCount == 0
}

// Clear очищает таблицу
func (dh *DoubleHash[T]) Clear() {
	dh.table = make([]HashNode[T], dh.tableSize+1)
	dh.elementsCount = 0
}

// Print выводит таблицу в stdout
func (dh *DoubleHash[T]) Print() {
	fmt.Println("=== Хэш-таблица ===")
	fmt.Printf("Размер: %d, Элементов: %d\n", dh.tableSize, dh.elementsCount)
	for i := uint32(0); i < dh.tableSize; i++ {
		if dh.table[i].IsOccupied {
			fmt.Printf("[%d] %s => %v\n", i, dh.table[i].Key, dh.table[i].Value)
		}
	}
	fmt.Println("===================")
}

// SerializeText сохраняет таблицу в текстовый файл
func (dh *DoubleHash[T]) SerializeText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open file for writing: %w", err)
	}
	defer file.Close()

	if _, err := fmt.Fprintf(file, "%d %d\n", dh.tableSize, dh.elementsCount); err != nil {
		return err
	}

	for i := uint32(0); i < dh.tableSize; i++ {
		if dh.table[i].IsOccupied {
			// Внимание: T должен поддерживать стандартное форматирование %v
			if _, err := fmt.Fprintf(file, "%d %s %v\n", i, dh.table[i].Key, dh.table[i].Value); err != nil {
				return err
			}
		}
	}
	fmt.Printf("Таблица (текст) успешно сохранена в %s\n", filename)
	return nil
}

// DeserializeText загружает таблицу из текстового файла
func (dh *DoubleHash[T]) DeserializeText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open file for reading: %w", err)
	}
	defer file.Close()

	var newTableSize, newElementsCount uint32
	if _, err := fmt.Fscan(file, &newTableSize, &newElementsCount); err != nil {
		return fmt.Errorf("could not read header: %w", err)
	}

	if newTableSize == 0 {
		return fmt.Errorf("size of table equal to zero")
	}

	// Инициализация новой таблицы
	dh.tableSize = newTableSize
	dh.elementsCount = newElementsCount
	dh.table = make([]HashNode[T], dh.tableSize+1)

	for {
		var idx uint32
		var key string
		var value T

		// Fscan ожидает пробелы между элементами.
		_, err := fmt.Fscan(file, &idx, &key, &value)
		if err != nil {
			// Конец файла — это нормально
			if err.Error() == "EOF" {
				break
			}
			return fmt.Errorf("error reading data: %w", err)
		}

		if idx >= dh.tableSize {
			return fmt.Errorf("index in file (%d) exceeds table size (%d)", idx, dh.tableSize)
		}

		dh.table[idx] = HashNode[T]{Key: key, Value: value, IsOccupied: true}
	}

	fmt.Printf("Таблица (текст) успешно загружена из %s\n", filename)
	return nil
}

// SerializeBin сохраняет таблицу в бинарный файл
func (dh *DoubleHash[T]) SerializeBin(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open binary file for writing: %w", err)
	}
	defer file.Close()

	// Записываем размеры заголовка
	if err := binary.Write(file, binary.LittleEndian, dh.tableSize); err != nil {
		return err
	}
	if err := binary.Write(file, binary.LittleEndian, dh.elementsCount); err != nil {
		return err
	}

	for i := uint32(0); i < dh.tableSize; i++ {
		occupied := dh.table[i].IsOccupied

		// Пишем флаг занятости
		if err := binary.Write(file, binary.LittleEndian, occupied); err != nil {
			return err
		}

		if occupied {
			// Пишем длину ключа
			keyBytes := []byte(dh.table[i].Key)
			keyLen := uint32(len(keyBytes))
			if err := binary.Write(file, binary.LittleEndian, keyLen); err != nil {
				return err
			}

			// Пишем сам ключ
			if _, err := file.Write(keyBytes); err != nil {
				return err
			}

			// Пишем значение
			if err := binary.Write(file, binary.LittleEndian, dh.table[i].Value); err != nil {
				return fmt.Errorf("failed to write value (type %T is likely not fixed-size): %w", dh.table[i].Value, err)
			}
		}
	}

	fmt.Printf("Таблица (бинарн. без gob) сохранена в %s\n", filename)
	return nil
}

// DeserializeBin загружает таблицу из бинарного файла
func (dh *DoubleHash[T]) DeserializeBin(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open binary file for reading: %w", err)
	}
	defer file.Close()

	// Читаем заголовок
	var newTableSize, newElementsCount uint32
	if err := binary.Read(file, binary.LittleEndian, &newTableSize); err != nil {
		return err
	}
	if err := binary.Read(file, binary.LittleEndian, &newElementsCount); err != nil {
		return err
	}

	dh.tableSize = newTableSize
	dh.elementsCount = newElementsCount
	dh.table = make([]HashNode[T], dh.tableSize+1)

	for i := uint32(0); i < dh.tableSize; i++ {
		var occupied bool
		// Читаем флаг занятости
		if err := binary.Read(file, binary.LittleEndian, &occupied); err != nil {
			return fmt.Errorf("read error at index %d: %w", i, err)
		}

		if occupied {
			// Читаем длину ключа
			var keyLen uint32
			if err := binary.Read(file, binary.LittleEndian, &keyLen); err != nil {
				return err
			}

			// Читаем сам ключ
			keyBuf := make([]byte, keyLen)
			if _, err := io.ReadFull(file, keyBuf); err != nil {
				return fmt.Errorf("failed to read key string")
			}
			key := string(keyBuf)

			// Читаем значение
			var value T
			if err := binary.Read(file, binary.LittleEndian, &value); err != nil {
				return fmt.Errorf("failed to read value: %w", err)
			}

			dh.table[i] = HashNode[T]{Key: key, Value: value, IsOccupied: true}
		} else {
			dh.table[i].IsOccupied = false
		}
	}

	fmt.Printf("Таблица (бинарн. без gob) загружена из %s\n", filename)
	return nil
}
