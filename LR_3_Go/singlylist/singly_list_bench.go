package singlylist

import (
	"os"
	"testing"
)

const (
	ListSize = 20000
	IOSize   = 10000
)

// BenchmarkPush сравнивает вставку в начало (O(1)) и в конец (O(N)).
func BenchmarkPush(b *testing.B) {
	b.Run("PushHead", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			b.StopTimer()
			list := NewForwardList[int]()
			b.StartTimer()

			for j := 0; j < ListSize; j++ {
				list.PushHead(j)
			}
		}
	})

	b.Run("PushBack", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			b.StopTimer()
			list := NewForwardList[int]()
			b.StartTimer()

			for j := 0; j < ListSize; j++ {
				list.PushBack(j)
			}
		}
	})
}

// BenchmarkDelete сравнивает удаление с головы и с хвоста.
func BenchmarkDelete(b *testing.B) {
	b.Run("DelHead", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			b.StopTimer()
			// Подготовка списка
			list := NewForwardList[int]()
			for j := 0; j < ListSize; j++ {
				list.PushHead(j)
			}
			b.StartTimer()

			// Замер удаления
			for list.Head != nil {
				_ = list.DelHead()
			}
		}
	})

	b.Run("DelBack", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			b.StopTimer()
			list := NewForwardList[int]()
			for j := 0; j < ListSize; j++ {
				list.PushHead(j)
			}
			b.StartTimer()

			// Удаление с хвоста — очень медленная операция
			for list.Head != nil {
				_ = list.DelBack()
			}
		}
	})
}

// BenchmarkFindInsert тестирует поиск и вставку перед элементом.
func BenchmarkFindInsert(b *testing.B) {
	// Создаем список один раз
	list := NewForwardList[int]()
	// 10000 элементов
	for i := 0; i < 10000; i++ {
		list.PushHead(i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		_ = list.PushBefore(0, -1)
	}
}

// BenchmarkIO сравнивает текстовый и бинарный ввод-вывод.
func BenchmarkIO(b *testing.B) {
	txtFile := "slist_bench.txt"
	binFile := "slist_bench.bin"
	defer os.Remove(txtFile)
	defer os.Remove(binFile)

	list := NewForwardList[int]()
	for i := 0; i < IOSize; i++ {
		list.PushHead(i)
	}

	b.Run("Text", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			// Save
			if err := list.Save(txtFile); err != nil {
				b.Fatal(err)
			}
			// Load
			newList := NewForwardList[int]()
			if err := newList.Load(txtFile); err != nil {
				b.Fatal(err)
			}
		}
	})

	b.Run("Binary", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			// Serialize
			if err := list.Serialize(binFile); err != nil {
				b.Fatal(err)
			}
			// Deserialize
			newList := NewForwardList[int]()
			if err := newList.Deserialize(binFile); err != nil {
				b.Fatal(err)
			}
		}
	})
}
