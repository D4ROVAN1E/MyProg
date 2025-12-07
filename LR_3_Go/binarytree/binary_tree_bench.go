package binarytree

import (
	"math/rand"
	"os"
	"testing"
)

const (
	TreeDataSize = 50000
	IOSize       = 10000
)

// generateRandomData создает срез случайных чисел
func generateRandomData(size int) []int {
	data := make([]int, size)
	for i := 0; i < size; i++ {
		// Используем широкий диапазон, чтобы уменьшить количество дубликатов
		data[i] = rand.Intn(size * 10)
	}
	return data
}

// BenchmarkInsert измеряет скорость вставки элементов в дерево.
func BenchmarkInsert(b *testing.B) {
	// Генерируем данные один раз, чтобы не включать это в замер
	data := generateRandomData(TreeDataSize)
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		tree := NewFullBinaryTree[int]()
		// Вставляем 50k элементов
		for _, val := range data {
			tree.Insert(val)
		}
	}
}

// BenchmarkIsFull измеряет скорость полного обхода дерева.
func BenchmarkIsFull(b *testing.B) {
	// Подготовка: строим дерево один раз
	b.StopTimer()
	tree := NewFullBinaryTree[int]()
	data := generateRandomData(TreeDataSize)
	for _, val := range data {
		tree.Insert(val)
	}
	b.StartTimer()

	for i := 0; i < b.N; i++ {
		// Результат не важен, важен процесс обхода
		_ = tree.IsFull()
	}
}

// BenchmarkClone измеряет скорость глубокого копирования дерева.
func BenchmarkClone(b *testing.B) {
	b.StopTimer()
	tree := NewFullBinaryTree[int]()
	// Берем меньший размер, так как Clone очень затратен
	smallData := generateRandomData(10000)
	for _, val := range smallData {
		tree.Insert(val)
	}
	b.StartTimer()

	for i := 0; i < b.N; i++ {
		_ = tree.Clone()
	}
}

// BenchmarkTextIO измеряет скорость сохранения и загрузки в текстовом формате.
func BenchmarkTextIO(b *testing.B) {
	filename := "tree_bench.txt"
	defer os.Remove(filename)

	// Подготовка дерева для сохранения
	tree := NewFullBinaryTree[int]()
	data := generateRandomData(IOSize)
	for _, val := range data {
		tree.Insert(val)
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// SaveText (Write)
		if err := tree.SaveText(filename); err != nil {
			b.Fatal(err)
		}

		// LoadText
		newTree := NewFullBinaryTree[int]()
		if err := newTree.LoadText(filename); err != nil {
			b.Fatal(err)
		}
	}
}

// BenchmarkBinaryIO измеряет скорость сохранения и загрузки в бинарном формате.
func BenchmarkBinaryIO(b *testing.B) {
	filename := "tree_bench.bin"
	defer os.Remove(filename)

	tree := NewFullBinaryTree[int32]() // Используем int32 для точного размера binary.Write
	for i := 0; i < IOSize; i++ {
		tree.Insert(int32(rand.Intn(100000)))
	}

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		// SaveBinary
		if err := tree.SaveBinary(filename); err != nil {
			b.Fatal(err)
		}

		// LoadBinary
		newTree := NewFullBinaryTree[int32]()
		if err := newTree.LoadBinary(filename); err != nil {
			b.Fatal(err)
		}
	}
}
