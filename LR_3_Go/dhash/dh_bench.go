package dhash

import (
	"math/rand"
	"os"
	"testing"
)

const (
	NumElements = 100000 // Количество элементов для теста
	StringLen   = 6      // Длина ключа
)

// generateKeys создает срез случайных ключей
func generateKeys(count int) []string {
	keys := make([]string, count)
	const charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

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
// Включает: вычисление двух хэшей, разрешение коллизий (пробирование) и ресайз.
func BenchmarkInsert(b *testing.B) {
	keys := generateKeys(NumElements)
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Создаем таблицу малого размера, чтобы спровоцировать resize
		ht, _ := NewDoubleHash[int](100)
		for j := 0; j < NumElements; j++ {
			ht.Insert(keys[j], j)
		}
	}
}

// BenchmarkFindHit измеряет поиск существующих элементов.
func BenchmarkFindHit(b *testing.B) {
	b.StopTimer()
	keys := generateKeys(NumElements)
	// Создаем таблицу с запасом, чтобы Load Factor был адекватным
	ht, _ := NewDoubleHash[int](uint32(NumElements * 2))
	for i, k := range keys {
		ht.Insert(k, i)
	}
	b.StartTimer()

	for i := 0; i < b.N; i++ {
		key := keys[i%NumElements]
		_ = ht.Find(key)
	}
}

// BenchmarkFindMiss измеряет поиск несуществующих элементов.
func BenchmarkFindMiss(b *testing.B) {
	b.StopTimer()
	keys := generateKeys(NumElements)
	ht, _ := NewDoubleHash[int](uint32(NumElements * 2))
	for i, k := range keys {
		ht.Insert(k, i)
	}

	// Генерируем ключи, которых точно нет
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
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		b.StopTimer() // Подготовка: заполняем таблицу
		ht, _ := NewDoubleHash[int](uint32(NumElements * 2))
		for j, k := range keys {
			ht.Insert(k, j)
		}
		b.StartTimer() // Замер: удаляем все элементы

		for _, k := range keys {
			ht.Remove(k)
		}
	}
}

// BenchmarkTextIO проверяет текстовую сериализацию.
func BenchmarkTextIO(b *testing.B) {
	filename := "dh_bench.txt"
	defer os.Remove(filename)

	const ioSize = 10000 // Уменьшенный размер для IO
	keys := generateKeys(ioSize)
	ht, _ := NewDoubleHash[int](uint32(ioSize * 2))
	for i, k := range keys {
		ht.Insert(k, i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Запись
		if err := ht.SerializeText(filename); err != nil {
			b.Fatal(err)
		}
		// Чтение
		newHt, _ := NewDoubleHash[int](1)
		if err := newHt.DeserializeText(filename); err != nil {
			b.Fatal(err)
		}
	}
}

// BenchmarkBinaryIO проверяет бинарную сериализацию.
func BenchmarkBinaryIO(b *testing.B) {
	filename := "dh_bench.bin"
	defer os.Remove(filename)

	const ioSize = 10000
	keys := generateKeys(ioSize)
	// Используем int32 для гарантии совместимости с binary.Write
	ht, _ := NewDoubleHash[int32](uint32(ioSize * 2))
	for i, k := range keys {
		ht.Insert(k, int32(i))
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Запись
		if err := ht.SerializeBin(filename); err != nil {
			b.Fatal(err)
		}
		// Чтение
		newHt, _ := NewDoubleHash[int32](1)
		if err := newHt.DeserializeBin(filename); err != nil {
			b.Fatal(err)
		}
	}
}
