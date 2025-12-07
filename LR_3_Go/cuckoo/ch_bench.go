package cuckoo

import (
	"math/rand"
	"os"
	"testing"
)

const (
	NumElements = 100000 // Количество элементов для тестов
	StringLen   = 6      // Длина ключа
)

// generateKeys создает срез случайных уникальных строк
func generateKeys(count int) []string {
	keys := make([]string, count)
	const charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

	// Используем map для гарантии уникальности
	seen := make(map[string]bool)

	for i := 0; i < count; {
		b := make([]byte, StringLen)
		for j := range b {
			b[j] = charset[rand.Intn(len(charset))]
		}
		key := string(b)
		if !seen[key] {
			seen[key] = true
			keys[i] = key
			i++
		}
	}
	return keys
}

// BenchmarkInsert измеряет скорость вставки.
// Включает в себя работу хэш-функций, разрешение коллизий (swaps) и ресайзинг.
func BenchmarkInsert(b *testing.B) {
	// Генерируем ключи заранее
	keys := generateKeys(NumElements)
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Создаем таблицу с малым размером, чтобы спровоцировать resize
		ht := NewCuckooHash[int](100)
		for j := 0; j < NumElements; j++ {
			ht.Insert(keys[j], j)
		}
	}
}

// BenchmarkFindHit измеряет скорость поиска существующих элементов.
func BenchmarkFindHit(b *testing.B) {
	// Подготовка
	b.StopTimer()
	keys := generateKeys(NumElements)
	ht := NewCuckooHash[int](uint32(NumElements * 2)) // Достаточный размер
	for i, k := range keys {
		ht.Insert(k, i)
	}
	b.StartTimer()

	for i := 0; i < b.N; i++ {
		// Ищем ключи по кругу
		key := keys[i%NumElements]
		_ = ht.Find(key)
	}
}

// BenchmarkFindMiss измеряет скорость поиска несуществующих элементов.
func BenchmarkFindMiss(b *testing.B) {
	b.StopTimer()
	keys := generateKeys(NumElements)
	ht := NewCuckooHash[int](uint32(NumElements * 2))
	for i, k := range keys {
		ht.Insert(k, i)
	}

	// Генерируем ключи, которых точно нет (добавляем префикс)
	missingKeys := make([]string, len(keys))
	for i, k := range keys {
		missingKeys[i] = "MISS_" + k
	}
	b.StartTimer()

	for i := 0; i < b.N; i++ {
		key := missingKeys[i%NumElements]
		_ = ht.Find(key)
	}
}

// BenchmarkRemove измеряет удаление элементов.
func BenchmarkRemove(b *testing.B) {
	keys := generateKeys(NumElements)

	// Для корректного замера в цикле b.N нам нужно каждый раз восстанавливать таблицу,
	// но это дорого. Поэтому мы будем заполнять таблицу, а затем удалять все элементы.
	// b.N будет означать количество полных циклов "Заполнил-Очистил".

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		b.StopTimer() // Останавливаем таймер на время вставки
		ht := NewCuckooHash[int](uint32(NumElements * 2))
		for j, k := range keys {
			ht.Insert(k, j)
		}
		b.StartTimer() // Запускаем для удаления

		for _, k := range keys {
			ht.Remove(k)
		}
	}
}

// BenchmarkTextIO проверяет текстовую сериализацию.
func BenchmarkTextIO(b *testing.B) {
	filename := "ch_bench.txt"
	defer os.Remove(filename)

	const ioSize = 10000 // Меньше элементов для IO, чтобы не забивать диск
	keys := generateKeys(ioSize)
	ht := NewCuckooHash[int](uint32(ioSize * 2))
	for i, k := range keys {
		ht.Insert(k, i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := ht.SerializeText(filename); err != nil {
			b.Fatal(err)
		}

		newHt := NewCuckooHash[int](0)
		if err := newHt.DeserializeText(filename); err != nil {
			b.Fatal(err)
		}
	}
}

// BenchmarkBinaryIO проверяет бинарную сериализацию.
func BenchmarkBinaryIO(b *testing.B) {
	filename := "ch_bench.bin"
	defer os.Remove(filename)

	const ioSize = 10000
	keys := generateKeys(ioSize)
	// Используем int32 для гарантии совместимости с binary.Write/Read
	ht := NewCuckooHash[int32](uint32(ioSize * 2))
	for i, k := range keys {
		ht.Insert(k, int32(i))
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := ht.SerializeBin(filename); err != nil {
			b.Fatal(err)
		}

		newHt := NewCuckooHash[int32](0)
		if err := newHt.DeserializeBin(filename); err != nil {
			b.Fatal(err)
		}
	}
}
