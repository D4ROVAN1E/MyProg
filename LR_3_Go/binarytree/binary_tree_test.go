package binarytree

import (
	"bytes"
	"encoding/binary"
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// Тесты базовой логики дерева

func TestConstructorAndInsert(t *testing.T) {
	tree := NewFullBinaryTree[int]()
	if tree.GetRoot() != nil {
		t.Error("New tree should have nil root")
	}

	// Вставка корня
	tree.Insert(10)
	if tree.GetRoot() == nil || tree.GetRoot().Key != 10 {
		t.Error("Root should be 10")
	}

	// Вставка детей
	tree.Insert(5)  // Меньше 10 -> влево
	tree.Insert(15) // Больше 10 -> вправо
	// Вставка дубликатов
	tree.Insert(15) // Равно 15 -> вправо от 15

	root := tree.GetRoot()
	if root.Left.Key != 5 {
		t.Errorf("Left child should be 5, got %v", root.Left.Key)
	}
	if root.Right.Key != 15 {
		t.Errorf("Right child should be 15, got %v", root.Right.Key)
	}
	if root.Right.Right.Key != 15 {
		t.Errorf("Duplicate should go to right child's right, got %v", root.Right.Right.Key)
	}
}

func TestIsFullTree(t *testing.T) {
	tests := []struct {
		name     string
		inserts  []int
		expected bool
	}{
		{"Empty tree", []int{}, true},
		{"Root only", []int{10}, true},
		{"One left child", []int{10, 5}, false},
		{"One right child", []int{10, 15}, false},
		{"Two children", []int{10, 5, 15}, true},
		{"Incomplete grandchild (left-left)", []int{10, 5, 15, 2}, false},
		{"Full grandchildren", []int{10, 5, 15, 2, 7, 12, 17}, true},
		{"Mixed Deep", []int{10, 5, 15, 2, 7}, true},
	}

	// Отдельная проверка на сложный случай рекурсии
	//       10
	//      /  \
	//     5    15
	//    / \
	//   2   nil -> Not Full
	t.Run("Deep Recursion Check Fail", func(t *testing.T) {
		tree := NewFullBinaryTree[int]()
		tree.Insert(10)
		tree.Insert(5)
		tree.Insert(15)
		tree.Insert(2) // У 5 теперь есть левый, но нет правого
		if tree.IsFull() {
			t.Error("Tree should not be full (node 5 has left child only)")
		}
	})

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			tree := NewFullBinaryTree[int]()
			for _, val := range tt.inserts {
				tree.Insert(val)
			}
			if got := tree.IsFull(); got != tt.expected {
				t.Errorf("IsFull() = %v, want %v", got, tt.expected)
			}
		})
	}
}

func TestClone(t *testing.T) {
	// Тест клонирования пустого дерева
	emptyTree := NewFullBinaryTree[int]()
	emptyClone := emptyTree.Clone()
	if emptyClone.GetRoot() != nil {
		t.Error("Cloned empty tree should be empty")
	}

	// Тест глубокого копирования
	tree1 := NewFullBinaryTree[int]()
	tree1.Insert(10)
	tree1.Insert(5)
	tree1.Insert(15)

	tree2 := tree1.Clone()

	if tree2.GetRoot() == nil || tree2.GetRoot().Key != 10 {
		t.Error("Cloned root data mismatch")
	}
	if tree1.GetRoot() == tree2.GetRoot() {
		t.Error("Pointers match, deep copy failed")
	}

	tree2.Insert(20)
	if tree1.IsFull() == false { // tree1 было полным, tree2 стало неполным
		t.Error("Original tree modified by clone operation")
	}
}

// Тесты вывода (Print)

func TestPrintMethods(t *testing.T) {
	tree := NewFullBinaryTree[int]()
	tree.Insert(2)
	tree.Insert(1)
	tree.Insert(3)

	var buf bytes.Buffer

	// Тест всех валидных опций
	tests := []struct {
		choice   int
		expected string
		name     string
	}{
		{1, "2 1 3", "Breadth-first"},
		{2, "2 1 3", "Pre-order"},
		{3, "1 2 3", "In-order"},
		{4, "1 3 2", "Post-order"},
		{5, "|___ 1", "Visual"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			buf.Reset()
			err := tree.Print(tt.choice, &buf)
			if err != nil {
				t.Fatalf("Print returned error: %v", err)
			}
			if !strings.Contains(buf.String(), tt.expected) {
				t.Errorf("Expected output containing %q, got %q", tt.expected, buf.String())
			}
		})
	}

	// Тест ошибочного выбора
	t.Run("Invalid Choice", func(t *testing.T) {
		if err := tree.Print(99, &buf); err == nil {
			t.Error("Expected error for invalid print choice")
		}
	})

	// Тест визуализации пустого дерева
	t.Run("Visual Empty", func(t *testing.T) {
		empty := NewFullBinaryTree[int]()
		buf.Reset()
		empty.Print(5, &buf)
		if !strings.Contains(buf.String(), "Дерево пустое") {
			t.Error("Should print empty message")
		}
	})

	// Тест BreadthFirst пустого дерева
	t.Run("BreadthFirst Empty", func(t *testing.T) {
		empty := NewFullBinaryTree[int]()
		buf.Reset()
		empty.Print(1, &buf)
		if buf.Len() != 0 {
			t.Error("BreadthFirst on empty tree should produce no output")
		}
	})
}

// Тесты Generics

func TestTemplateTypes(t *testing.T) {
	// Float64
	treeF := NewFullBinaryTree[float64]()
	treeF.Insert(10.5)
	treeF.Insert(5.5)
	if math.Abs(treeF.GetRoot().Left.Key-5.5) > 0.001 {
		t.Error("Float64 mismatch")
	}

	// String (cmp.Ordered)
	treeS := NewFullBinaryTree[string]()
	treeS.Insert("banana")
	treeS.Insert("apple")
	treeS.Insert("cherry")
	// "apple" < "banana" -> Left
	if treeS.GetRoot().Left.Key != "apple" {
		t.Error("String ordering mismatch")
	}
}

// Тесты I/O: Текстовый формат

func TestFileIO_Text_Success(t *testing.T) {
	tree := NewFullBinaryTree[int]()
	tree.Insert(10)
	tree.Insert(5)
	tree.Insert(15)

	tmpDir := t.TempDir()
	filename := filepath.Join(tmpDir, "text_success.txt")

	// Save
	if err := tree.SaveText(filename); err != nil {
		t.Fatalf("SaveText failed: %v", err)
	}

	// Load
	loadedTree := NewFullBinaryTree[int]()
	if err := loadedTree.LoadText(filename); err != nil {
		t.Fatalf("LoadText failed: %v", err)
	}

	// Check
	var buf bytes.Buffer
	loadedTree.Print(3, &buf) // In-order
	if !strings.Contains(buf.String(), "5 10 15") {
		t.Error("Data mismatch after reload")
	}
}

func TestFileIO_Text_EmptyAndErrors(t *testing.T) {
	// Save Empty Tree
	tree := NewFullBinaryTree[int]()
	tmpDir := t.TempDir()
	filename := filepath.Join(tmpDir, "empty.txt")

	if err := tree.SaveText(filename); err != nil {
		t.Fatal(err)
	}
	// Проверяем, что файл пустой или существует
	stat, err := os.Stat(filename)
	if err != nil || stat.Size() > 0 {
		t.Error("Empty tree should result in empty file")
	}

	// Load Non-Existent
	if err := tree.LoadText("missing_file.txt"); err == nil {
		t.Error("Expected error loading missing file")
	}

	// Load Malformed Data (Срабатывание Fscan error)
	badFile := filepath.Join(tmpDir, "bad_data.txt")
	os.WriteFile(badFile, []byte("NOT_A_NUMBER"), 0644)
	if err := tree.LoadText(badFile); err == nil {
		t.Error("Expected error loading malformed text data")
	}
}

func TestFileIO_WriteErrors(t *testing.T) {
	tree := NewFullBinaryTree[int]()
	tree.Insert(10)

	// Пытаемся записать в директорию, а не файл (это вызовет ошибку os.Create)
	tmpDir := t.TempDir()
	if err := tree.SaveText(tmpDir); err == nil {
		t.Error("Expected error writing text to a directory path")
	}
	if err := tree.SaveBinary(tmpDir); err == nil {
		t.Error("Expected error writing binary to a directory path")
	}
}

// Тесты I/O: Бинарный формат

func TestFileIO_Binary_Success(t *testing.T) {
	tree := NewFullBinaryTree[int32]()
	tree.Insert(10)
	tree.Insert(5)
	tree.Insert(15)

	tmpDir := t.TempDir()
	filename := filepath.Join(tmpDir, "tree.bin")

	if err := tree.SaveBinary(filename); err != nil {
		t.Fatalf("SaveBinary failed: %v", err)
	}

	loaded := NewFullBinaryTree[int32]()
	if err := loaded.LoadBinary(filename); err != nil {
		t.Fatalf("LoadBinary failed: %v", err)
	}

	if loaded.GetRoot().Key != 10 {
		t.Error("Root mismatch after binary load")
	}
}

func TestFileIO_Binary_EdgeCases(t *testing.T) {
	tmpDir := t.TempDir()
	tree := NewFullBinaryTree[int32]()

	// Load Missing File
	if err := tree.LoadBinary("missing.bin"); err == nil {
		t.Error("Expected error for missing binary file")
	}

	// Invalid Marker (Ожидается 0 или 1, пишем 5)
	// Формат рекурсии: [Marker(int8)] [Value(T) if Marker=1]
	badMarkerFile := filepath.Join(tmpDir, "bad_marker.bin")
	// Пишем один байт '5'
	os.WriteFile(badMarkerFile, []byte{5}, 0644)

	if err := tree.LoadBinary(badMarkerFile); err == nil || !strings.Contains(err.Error(), "invalid file format") {
		t.Errorf("Expected 'invalid file format' error, got: %v", err)
	}

	// Truncated File (Маркер 1 есть, а данных нет)
	truncatedFile := filepath.Join(tmpDir, "truncated.bin")
	// Пишем маркер '1' (существует), но не пишем int32 значение
	os.WriteFile(truncatedFile, []byte{1}, 0644)

	if err := tree.LoadBinary(truncatedFile); err == nil {
		t.Error("Expected error reading truncated binary file")
	}

	// Empty File (EOF сразу) -> Должно трактоваться как nil root
	// Значит пустое дерево загрузится без ошибок.
	emptyBin := filepath.Join(tmpDir, "empty.bin")
	os.WriteFile(emptyBin, []byte{}, 0644)
	if err := tree.LoadBinary(emptyBin); err != nil {
		t.Errorf("Loading empty binary file should satisfy nil root, got error: %v", err)
	}
	if tree.GetRoot() != nil {
		t.Error("Expected nil root from empty binary file")
	}
}

// Сценарий для тестирования ошибки чтения ВНУТРИ рекурсии
func TestFileIO_Binary_RecursiveError(t *testing.T) {
	tmpDir := t.TempDir()
	brokenDeepFile := filepath.Join(tmpDir, "broken_deep.bin")

	// Структура: Root(1) -> 10 -> Left(1) -> ERROR (EOF)
	// int32 = 4 bytes. int8 = 1 byte.
	// Root exists (1 byte: 0x01)
	// Root value (4 bytes: 10)
	// Left exists (1 byte: 0x01)
	// Left value (Missing!)
	buf := new(bytes.Buffer)
	binary.Write(buf, binary.LittleEndian, int8(1))
	binary.Write(buf, binary.LittleEndian, int32(10))
	binary.Write(buf, binary.LittleEndian, int8(1)) // Marker left exists
	// No data for left node

	os.WriteFile(brokenDeepFile, buf.Bytes(), 0644)

	tree := NewFullBinaryTree[int32]()
	if err := tree.LoadBinary(brokenDeepFile); err == nil {
		t.Error("Expected error due to unexpected EOF in deep recursion")
	}
}
