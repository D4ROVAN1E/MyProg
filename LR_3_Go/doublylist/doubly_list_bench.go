package doublylist

import (
	"math/rand"
	"os"
	"testing"
)

const (
	NumElements = 100000 // Размер для тестов поиска и вставки
	IOSize      = 50000  // Размер для тестов IO
)

// BenchmarkPushEnds сравнивает вставку в начало и в конец
func BenchmarkPushEnds(b *testing.B) {
	b.Run("PushHead", func(b *testing.B) {
		list := NewDoublyList[int]()
		b.ResetTimer()
		for i := 0; i < b.N; i++ {
			list.LPushHead(i)
		}
	})

	b.Run("PushBack", func(b *testing.B) {
		list := NewDoublyList[int]()
		b.ResetTimer()
		for i := 0; i < b.N; i++ {
			list.LPushBack(i)
		}
	})
}

// BenchmarkMiddleOperations тестирует вставку в середину
func BenchmarkMiddleOperations(b *testing.B) {
	// Подготовка: создаем список среднего размера
	const initialSize = 10000
	list := NewDoublyList[int]()
	for i := 0; i < initialSize; i++ {
		list.LPushBack(i)
	}

	b.ResetTimer()

	// Пытаемся вставить элемент после каждого четного существующего элемента
	// Это тяжелый тест, так как каждый вызов делает пробег по памяти
	for i := 0; i < b.N; i++ {
		// Используем остаток от деления, чтобы искать существующие ключи
		target := (i * 2) % initialSize
		_ = list.LPushAfter(target, -1)
	}
}

// BenchmarkSearch тестирует линейный поиск (LGetByValue).
func BenchmarkSearch(b *testing.B) {
	list := NewDoublyList[int]()
	for i := 0; i < NumElements; i++ {
		list.LPushBack(i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Ищем случайный элемент
		target := rand.Intn(NumElements)
		_ = list.LGetByValue(target)
	}
}

// BenchmarkTextIO тестирует сохранение и загрузку в текстовом виде.
func BenchmarkTextIO(b *testing.B) {
	filename := "list_bench.txt"
	defer os.Remove(filename)

	list := NewDoublyList[int]()
	for i := 0; i < IOSize; i++ {
		list.LPushBack(i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Save
		if err := list.LSave(filename); err != nil {
			b.Fatal(err)
		}

		// Load
		newList := NewDoublyList[int]()
		if err := newList.LLoad(filename); err != nil {
			b.Fatal(err)
		}
	}
}

// BenchmarkBinaryIO тестирует сохранение и загрузку в бинарном виде.
func BenchmarkBinaryIO(b *testing.B) {
	filename := "list_bench.bin"
	defer os.Remove(filename)

	// Используем int32 для корректной работы binary.Write/Read
	list := NewDoublyList[int32]()
	for i := 0; i < IOSize; i++ {
		list.LPushBack(int32(i))
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Save
		if err := list.LSaveBin(filename); err != nil {
			b.Fatal(err)
		}

		// Load
		newList := NewDoublyList[int32]()
		if err := newList.LLoadBin(filename); err != nil {
			b.Fatal(err)
		}
	}
}
