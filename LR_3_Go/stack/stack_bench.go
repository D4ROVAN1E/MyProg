package stack

import (
	"os"
	"testing"
)

const (
	// Размер стека для тестов ввода-вывода (I/O)
	ioPayloadSize = 10000
	txtFileName   = "bench_temp.txt"
	binFileName   = "bench_temp.bin"
)

// BenchmarkStack_Push проверяет скорость добавления элементов.
func BenchmarkStack_Push(b *testing.B) {
	s := NewStack[int]()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		s.Push(i)
	}
}

// BenchmarkStack_PushPop проверяет цикл "добавить-извлечь".
func BenchmarkStack_PushPop(b *testing.B) {
	s := NewStack[int]()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		s.Push(i)
		_, _ = s.Pop()
	}
}

// setupStack создает заполненный стек для тестов сохранения.
func setupStack(size int) *Stack[int] {
	s := NewStack[int]()
	for i := 0; i < size; i++ {
		s.Push(i)
	}
	return s
}

// BenchmarkStack_SaveText измеряет скорость сохранения в текстовый файл.
func BenchmarkStack_SaveText(b *testing.B) {
	s := setupStack(ioPayloadSize)
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := s.SaveText(txtFileName); err != nil {
			b.Fatalf("SaveText failed: %v", err)
		}
	}
	b.StopTimer()
	os.Remove(txtFileName)
}

// BenchmarkStack_LoadText измеряет скорость загрузки из текстового файла.
func BenchmarkStack_LoadText(b *testing.B) {
	// Подготовка: сначала сохраняем файл
	s := setupStack(ioPayloadSize)
	if err := s.SaveText(txtFileName); err != nil {
		b.Fatalf("Setup failed: %v", err)
	}

	// Пустой стек для загрузки
	loader := NewStack[int]()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := loader.LoadText(txtFileName); err != nil {
			b.Fatalf("LoadText failed: %v", err)
		}
	}
	b.StopTimer()
	os.Remove(txtFileName)
}

// BenchmarkStack_SaveBinary измеряет скорость сохранения в бинарный файл (gob).
func BenchmarkStack_SaveBinary(b *testing.B) {
	s := setupStack(ioPayloadSize)
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := s.SaveBinary(binFileName); err != nil {
			b.Fatalf("SaveBinary failed: %v", err)
		}
	}
	b.StopTimer()
	os.Remove(binFileName)
}

// BenchmarkStack_LoadBinary измеряет скорость загрузки из бинарного файла (gob).
func BenchmarkStack_LoadBinary(b *testing.B) {
	// Подготовка: сначала сохраняем файл
	s := setupStack(ioPayloadSize)
	if err := s.SaveBinary(binFileName); err != nil {
		b.Fatalf("Setup failed: %v", err)
	}

	loader := NewStack[int]()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		if err := loader.LoadBinary(binFileName); err != nil {
			b.Fatalf("LoadBinary failed: %v", err)
		}
	}
	b.StopTimer()
	os.Remove(binFileName)
}
