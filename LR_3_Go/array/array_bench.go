package array

import (
	"os"
	"testing"
)

const (
	SmallDataSize = 10000
	LargeDataSize = 1000000
)

// BenchmarkPushBack измеряет скорость добавления в конец.
func BenchmarkPushBack(b *testing.B) {
	// b.N — это количество итераций, которое Go подберет сам для точности замера
	for i := 0; i < b.N; i++ {
		arr := NewArray[int]()
		for j := 0; j < SmallDataSize; j++ {
			arr.PushBack(j)
		}
	}
}

// BenchmarkPushBack_PreAllocated измеряет скорость, если мы заранее знаем размер (NewArrayWithCap).
func BenchmarkPushBack_PreAllocated(b *testing.B) {
	for i := 0; i < b.N; i++ {
		arr := NewArrayWithCap[int](SmallDataSize)
		for j := 0; j < SmallDataSize; j++ {
			arr.PushBack(j)
		}
	}
}

// BenchmarkInsertMiddle измеряет вставку в середину.
func BenchmarkInsertMiddle(b *testing.B) {
	// Создаем массив
	b.StopTimer() // Останавливаем таймер на время подготовки
	arr := NewArray[int]()
	for i := 0; i < SmallDataSize; i++ {
		arr.PushBack(i)
	}
	b.StartTimer() // Запускаем таймер

	// Вставляем b.N раз в середину
	for i := 0; i < b.N; i++ {
		// Вставляем всегда в середину текущего размера
		mid := arr.GetSize() / 2
		_ = arr.InsertByInd(mid, i)
	}
}

// BenchmarkAccessGet измеряет скорость чтения по индексу.
func BenchmarkAccessGet(b *testing.B) {
	// Подготовка
	arr := NewArray[int]()
	for i := 0; i < LargeDataSize; i++ {
		arr.PushBack(i)
	}

	size := arr.GetSize()

	b.ResetTimer() // Сбрасываем таймер перед началом цикла
	for i := 0; i < b.N; i++ {
		// Используем i как псевдослучайный индекс
		idx := i % size
		_, _ = arr.Get(idx)
	}
}

// BenchmarkBinaryIO измеряет скорость сохранения и загрузки в бинарном формате.
func BenchmarkBinaryIO(b *testing.B) {
	filename := "bench_test.bin"
	defer os.Remove(filename) // Удаляем файл после теста

	// Подготовка данных
	arr := NewArray[int32]()
	for i := 0; i < SmallDataSize; i++ { // 10k элементов для IO теста
		arr.PushBack(int32(i))
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Замеряем запись
		err := arr.SaveBinary(filename)
		if err != nil {
			b.Fatal(err)
		}

		// Замеряем чтение
		loadedArr := NewArray[int32]()
		err = loadedArr.LoadBinary(filename)
		if err != nil {
			b.Fatal(err)
		}
	}
}

// BenchmarkTextIO измеряет скорость сохранения и загрузки в текстовом формате.
func BenchmarkTextIO(b *testing.B) {
	filename := "bench_test.txt"
	defer os.Remove(filename)

	arr := NewArray[int]()
	for i := 0; i < 1000; i++ { // Меньше элементов, т.к. текст медленный
		arr.PushBack(i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		err := arr.SaveText(filename)
		if err != nil {
			b.Fatal(err)
		}

		loadedArr := NewArray[int]()
		err = loadedArr.LoadText(filename)
		if err != nil {
			b.Fatal(err)
		}
	}
}
