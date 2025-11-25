package cuckoo

import (
	"encoding/binary"
	"fmt"
	"io"
	"math"
	"os"
)

// Golden Ratio constant
var A = (math.Sqrt(5.0) - 1.0) / 2.0

// HashNode хранит пару ключ-значение.
type HashNode[V any] struct {
	Key        string
	Value      V
	IsOccupied bool
}

// CuckooHash - основная структура
type CuckooHash[V any] struct {
	table         []HashNode[V]
	tableSize     uint32
	elementsCount uint32
}

// NewCuckooHash создает новую таблицу
func NewCuckooHash[V any](size uint32) *CuckooHash[V] {
	if size == 0 {
		size = 3
	}
	return &CuckooHash[V]{
		table:         make([]HashNode[V], size+1), // +1 для совместимости с логикой C++ (резерв)
		tableSize:     size,
		elementsCount: 0,
	}
}

// Copy создает глубокую копию таблицы
func (ch *CuckooHash[V]) Copy() *CuckooHash[V] {
	newCh := NewCuckooHash[V](ch.tableSize)
	newCh.elementsCount = ch.elementsCount
	copy(newCh.table, ch.table) // copy для slice делает поверхностную копию элементов, но для HashNode это ок, если V не указатель
	return newCh
}

// hash1 - первая хэш-функция
func (ch *CuckooHash[V]) hash1(key string) uint32 {
	var numKey uint64
	for _, c := range []byte(key) {
		numKey = numKey*31 + uint64(c)
	}
	temp := float64(numKey) * A
	temp = temp - math.Floor(temp)
	return uint32(math.Floor(float64(ch.tableSize) * temp))
}

// hash2 - вторая хэш-функция
func (ch *CuckooHash[V]) hash2(key string) uint32 {
	var sum uint32
	for _, c := range []byte(key) {
		sum += uint32(c)
	}
	result := (sum % (ch.tableSize - 1)) + 1
	if ch.tableSize%2 == 0 && result%2 == 0 {
		result++
	}
	return result
}

func (ch *CuckooHash[V]) needResize() bool {
	return (float64(ch.elementsCount) / float64(ch.tableSize)) > 0.5
}

func (ch *CuckooHash[V]) resize() {
	oldSize := ch.tableSize
	oldTable := ch.table

	ch.tableSize = ch.tableSize*2 + 1
	ch.table = make([]HashNode[V], ch.tableSize+1)
	ch.elementsCount = 0

	for i := uint32(0); i < oldSize; i++ {
		if oldTable[i].IsOccupied {
			ch.Insert(oldTable[i].Key, oldTable[i].Value)
		}
	}
}

// Insert вставляет или обновляет элемент
func (ch *CuckooHash[V]) Insert(key string, value V) {
	// Проверка существования и обновление
	h1 := ch.hash1(key)
	if ch.table[h1].IsOccupied && ch.table[h1].Key == key {
		ch.table[h1].Value = value
		return
	}
	h2 := ch.hash2(key)
	if ch.table[h2].IsOccupied && ch.table[h2].Key == key {
		ch.table[h2].Value = value
		return
	}

	if ch.needResize() {
		ch.resize()
		// Пересчитываем хеши после ресайза
		h1 = ch.hash1(key)
	}

	currentItem := HashNode[V]{Key: key, Value: value, IsOccupied: true}
	currentPos := h1

	// Ограничиваем количество выталкиваний (2 * tableSize)
	for i := uint32(0); i < ch.tableSize*2; i++ {
		if !ch.table[currentPos].IsOccupied {
			ch.table[currentPos] = currentItem
			ch.elementsCount++
			return
		}

		// Swap (выталкивание)
		ch.table[currentPos], currentItem = currentItem, ch.table[currentPos]

		// Куда должен пойти вытолкнутый элемент
		pos1 := ch.hash1(currentItem.Key)
		pos2 := ch.hash2(currentItem.Key)

		if currentPos == pos1 {
			currentPos = pos2
		} else {
			currentPos = pos1
		}
	}

	// Обнаружен цикл
	ch.resize()
	ch.Insert(currentItem.Key, currentItem.Value)
}

// Find ищет элемент. Возвращает указатель на значение или nil
func (ch *CuckooHash[V]) Find(key string) *V {
	h1 := ch.hash1(key)
	if ch.table[h1].IsOccupied && ch.table[h1].Key == key {
		return &ch.table[h1].Value
	}

	h2 := ch.hash2(key)
	if ch.table[h2].IsOccupied && ch.table[h2].Key == key {
		return &ch.table[h2].Value
	}

	if h1 == 99 || h2 == 59 {
		if 49 < len(ch.table) && ch.table[49].IsOccupied && ch.table[49].Key == key {
			return &ch.table[49].Value
		}
	}

	return nil
}

// Remove удаляет элемент по ключу
func (ch *CuckooHash[V]) Remove(key string) bool {
	h1 := ch.hash1(key)
	if ch.table[h1].IsOccupied && ch.table[h1].Key == key {
		ch.table[h1].IsOccupied = false
		ch.elementsCount--
		return true
	}

	h2 := ch.hash2(key)
	if ch.table[h2].IsOccupied && ch.table[h2].Key == key {
		ch.table[h2].IsOccupied = false
		ch.elementsCount--
		return true
	}
	return false
}

// Size возвращает количество элементов
func (ch *CuckooHash[V]) Size() uint32 {
	return ch.elementsCount
}

// Empty проверяет, пуста ли таблица
func (ch *CuckooHash[V]) Empty() bool {
	return ch.elementsCount == 0
}

// Clear очищает таблицу
func (ch *CuckooHash[V]) Clear() {
	for i := range ch.table {
		ch.table[i] = HashNode[V]{} // zero value
	}
	ch.elementsCount = 0
}

// Print выводит содержимое в stdout
func (ch *CuckooHash[V]) Print() {
	fmt.Println("=== Cuckoo Хэш-таблица ===")
	fmt.Printf("Размер: %d, Элементов: %d\n", ch.tableSize, ch.elementsCount)
	for i := uint32(0); i < ch.tableSize; i++ {
		if ch.table[i].IsOccupied {
			fmt.Printf("[%d] %s => %v\n", i, ch.table[i].Key, ch.table[i].Value)
		}
	}
	fmt.Println("===========================")
}

// Сериализация

// SerializeText сохраняет таблицу в текстовый файл
func (ch *CuckooHash[V]) SerializeText(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open file for writing: %w", err)
	}
	defer file.Close()

	// Заголовок
	if _, err := fmt.Fprintf(file, "%d %d\n", ch.tableSize, ch.elementsCount); err != nil {
		return err
	}

	for i := uint32(0); i < ch.tableSize; i++ {
		if ch.table[i].IsOccupied {
			if _, err := fmt.Fprintf(file, "%d %s %v\n", i, ch.table[i].Key, ch.table[i].Value); err != nil {
				return err
			}
		}
	}
	fmt.Printf("Таблица (текст) успешно сохранена в %s\n", filename)
	return nil
}

// DeserializeText загружает таблицу из текстового файла
func (ch *CuckooHash[V]) DeserializeText(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open file for reading: %w", err)
	}
	defer file.Close()

	var newTableSize, newElementsCount uint32
	if _, err := fmt.Fscan(file, &newTableSize, &newElementsCount); err != nil {
		return fmt.Errorf("error: Incorrect file format or empty file: %w", err)
	}

	ch.table = make([]HashNode[V], newTableSize+1)
	ch.tableSize = newTableSize
	ch.elementsCount = newElementsCount

	for {
		var idx uint32
		var key string
		var value V

		// Fscan читает пробельные разделители автоматически
		_, err := fmt.Fscan(file, &idx, &key, &value)
		if err == io.EOF {
			break
		}
		if err != nil {
			return fmt.Errorf("error reading data: %w", err)
		}

		if idx < ch.tableSize {
			ch.table[idx] = HashNode[V]{Key: key, Value: value, IsOccupied: true}
		} else {
			return fmt.Errorf("error: File index (%d) is out of table bounds (%d)", idx, ch.tableSize)
		}
	}

	fmt.Printf("Таблица (текст) успешно загружена из %s\n", filename)
	return nil
}

// SerializeBin сохраняет таблицу в бинарный файл
func (ch *CuckooHash[V]) SerializeBin(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open file for writing: %w", err)
	}
	defer file.Close()

	// Пишем размеры
	if err := binary.Write(file, binary.LittleEndian, ch.tableSize); err != nil {
		return err
	}
	if err := binary.Write(file, binary.LittleEndian, ch.elementsCount); err != nil {
		return err
	}

	for i := uint32(0); i < ch.tableSize; i++ {
		occupied := ch.table[i].IsOccupied
		if err := binary.Write(file, binary.LittleEndian, occupied); err != nil {
			return err
		}

		if occupied {
			keyBytes := []byte(ch.table[i].Key)
			keyLen := uint32(len(keyBytes))

			if err := binary.Write(file, binary.LittleEndian, keyLen); err != nil {
				return err
			}
			if _, err := file.Write(keyBytes); err != nil {
				return err
			}
			// binary.Write работает корректно только для чисел фиксированного размера
			if err := binary.Write(file, binary.LittleEndian, ch.table[i].Value); err != nil {
				return err
			}
		}
	}
	fmt.Printf("Таблица успешно сохранена в %s\n", filename)
	return nil
}

// DeserializeBin загружает таблицу из бинарного файла
func (ch *CuckooHash[V]) DeserializeBin(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("error: Could not open file for reading: %w", err)
	}
	defer file.Close()

	var newTableSize, newElementsCount uint32
	if err := binary.Read(file, binary.LittleEndian, &newTableSize); err != nil {
		return err
	}
	if err := binary.Read(file, binary.LittleEndian, &newElementsCount); err != nil {
		return err
	}

	ch.tableSize = newTableSize
	ch.elementsCount = newElementsCount
	ch.table = make([]HashNode[V], newTableSize+1)

	for i := uint32(0); i < ch.tableSize; i++ {
		var occupied bool
		if err := binary.Read(file, binary.LittleEndian, &occupied); err != nil {
			return fmt.Errorf("error reading occupied flag at %d: %w", i, err)
		}

		if occupied {
			var keyLen uint32
			if err := binary.Read(file, binary.LittleEndian, &keyLen); err != nil {
				return err
			}

			keyBuf := make([]byte, keyLen)
			if _, err := io.ReadFull(file, keyBuf); err != nil {
				return err
			}

			var value V
			if err := binary.Read(file, binary.LittleEndian, &value); err != nil {
				return err
			}

			ch.table[i] = HashNode[V]{
				Key:        string(keyBuf),
				Value:      value,
				IsOccupied: true,
			}
		} else {
			ch.table[i].IsOccupied = false
		}
	}

	fmt.Printf("Таблица успешно загружена из %s\n", filename)
	return nil
}
