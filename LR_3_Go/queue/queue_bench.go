package queue

import (
	"os"
	"testing"
)

const (
	BenchQueueSize = 10000 // Размер очереди для IO тестов
	OpsCount       = 1000  // Количество операций в батче
)

// BenchmarkPush_Resize измеряет скорость добавления с учетом ресайза.
func BenchmarkPush_Resize(b *testing.B) {
	for i := 0; i < b.N; i++ {
		// Начинаем с маленькой емкости, чтобы форсировать resize
		q := NewQueue[int](2)
		for j := 0; j < OpsCount; j++ {
			q.Push(j)
		}
	}
}

// BenchmarkCircularLogic (Ping-Pong) проверяет эффективность кольцевого буфера.
func BenchmarkCircularLogic(b *testing.B) {
	// Подготовка: создаем очередь достаточной емкости
	q := NewQueue[int](OpsCount * 2)
	// Заполняем наполовину
	for i := 0; i < OpsCount; i++ {
		q.Push(i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// Добавляем новый элемент
		q.Push(i)
		// Сразу забираем старый
		_, _ = q.Pop()
	}
}

// BenchmarkPop измеряет скорость извлечения.
func BenchmarkPop(b *testing.B) {
	// В этом тесте мы не можем просто удалять бесконечно,
	// поэтому делаем это циклами: Заполнили -> Очистили.
	b.StopTimer()
	q := NewQueue[int](OpsCount)
	b.StartTimer()

	for i := 0; i < b.N; i++ {
		// Останавливаем таймер для наполнения (нас интересует Pop)
		b.StopTimer()
		q.head = 0
		q.tail = 0
		q.count = 0
		for j := 0; j < OpsCount; j++ {
			q.Push(j)
		}
		b.StartTimer()

		// Замеряем опустошение
		for !q.IsEmpty() {
			_, _ = q.Pop()
		}
	}
}

// BenchmarkTextIO проверяет сохранение/загрузку через fmt.Fscan/Fprint.
func BenchmarkTextIO(b *testing.B) {
	filename := "queue_bench.txt"
	defer os.Remove(filename)

	// Подготовка
	q := NewQueue[int](BenchQueueSize)
	for i := 0; i < BenchQueueSize; i++ {
		q.Push(i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := q.SaveText(filename); err != nil {
			b.Fatal(err)
		}

		newQ := NewQueue[int](0)
		if err := newQ.LoadText(filename); err != nil {
			b.Fatal(err)
		}
	}
}

// BenchmarkGobIO проверяет сохранение/загрузку через encoding/gob.
func BenchmarkGobIO(b *testing.B) {
	filename := "queue_bench.gob"
	defer os.Remove(filename)

	q := NewQueue[int](BenchQueueSize)
	for i := 0; i < BenchQueueSize; i++ {
		q.Push(i)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := q.SaveBinary(filename); err != nil {
			b.Fatal(err)
		}

		newQ := NewQueue[int](0)
		if err := newQ.LoadBinary(filename); err != nil {
			b.Fatal(err)
		}
	}
}
