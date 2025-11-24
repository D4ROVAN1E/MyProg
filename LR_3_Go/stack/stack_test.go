package stack

import (
	"bytes"
	"io"
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// --- Вспомогательные функции ---

// captureOutput перехватывает вывод в stdout для проверки метода Print
func captureOutput(f func()) string {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	f()

	w.Close()
	os.Stdout = old

	var buf bytes.Buffer
	io.Copy(&buf, r)
	return buf.String()
}

// --- Тесты базовой функциональности ---

func TestNewStack(t *testing.T) {
	s := NewStack[int]()
	if s == nil {
		t.Fatal("NewStack returned nil")
	}
	if s.Size() != 0 {
		t.Errorf("Expected size 0, got %d", s.Size())
	}
	// Проверка начальной емкости (внутренняя проверка через рефлексию или косвенно)
	// В данном случае просто убеждаемся, что он работает.
}

func TestNewStackWithCapacity(t *testing.T) {
	// Сценарий 1: Успешное создание
	s, err := NewStackWithCapacity[string](10)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}
	if s.Size() != 0 {
		t.Errorf("Expected size 0, got %d", s.Size())
	}

	// Сценарий 2: Ошибка (capacity <= 0)
	_, err = NewStackWithCapacity[string](0)
	if err == nil {
		t.Error("Expected error for capacity 0, got nil")
	}
	if err.Error() != "initial capacity must be greater than 0" {
		t.Errorf("Incorrect error message: %v", err)
	}

	_, err = NewStackWithCapacity[string](-5)
	if err == nil {
		t.Error("Expected error for negative capacity, got nil")
	}
}

func TestPushAndPop(t *testing.T) {
	s := NewStack[int]()

	// Push
	s.Push(10)
	s.Push(20)
	if s.Size() != 2 {
		t.Errorf("Expected size 2, got %d", s.Size())
	}

	// Pop успешно
	val, err := s.Pop()
	if err != nil {
		t.Errorf("Unexpected error on Pop: %v", err)
	}
	if val != 20 {
		t.Errorf("Expected 20, got %d", val)
	}

	val, err = s.Pop()
	if err != nil {
		t.Errorf("Unexpected error on Pop: %v", err)
	}
	if val != 10 {
		t.Errorf("Expected 10, got %d", val)
	}

	// Pop из пустого стека (Ошибка)
	_, err = s.Pop()
	if err == nil {
		t.Error("Expected error when popping from empty stack")
	}
	if err.Error() != "stack underflow: cannot pop from an empty stack" {
		t.Errorf("Incorrect error message: %v", err)
	}
}

func TestAutoResize(t *testing.T) {
	s, _ := NewStackWithCapacity[int](1)
	// Добавляем больше элементов, чем начальная емкость
	for i := 0; i < 10; i++ {
		s.Push(i)
	}
	if s.Size() != 10 {
		t.Errorf("Expected size 10, got %d", s.Size())
	}
}

func TestClone(t *testing.T) {
	original := NewStack[int]()
	original.Push(1)
	original.Push(2)

	clone := original.Clone()

	// Проверка независимости
	clone.Pop()
	clone.Push(999)

	if original.Size() != 2 {
		t.Error("Original stack modified after clone manipulation")
	}

	val, _ := original.Pop()
	if val != 2 {
		t.Error("Original stack content corrupted")
	}
}

func TestPrint(t *testing.T) {
	output := captureOutput(func() {
		s := NewStack[string]()
		s.Push("A")
		s.Push("B")
		s.Print()
	})

	expected := "A B \n" // fmt.Println добавляет \n в конце
	if output != expected {
		// В зависимости от ОС переводы строк могут отличаться, упростим проверку
		if !strings.Contains(output, "A B") {
			t.Errorf("Expected output containing 'A B', got %q", output)
		}
	}
}

// --- Тесты сохранения/загрузки (включая ошибки) ---

func TestSaveText(t *testing.T) {
	tmpDir := t.TempDir()
	goodFile := filepath.Join(tmpDir, "stack.txt")

	// Сценарий 1: Успешное сохранение
	s := NewStack[int]()
	s.Push(100)
	s.Push(200)
	if err := s.SaveText(goodFile); err != nil {
		t.Fatalf("SaveText failed: %v", err)
	}

	// Проверяем содержимое файла вручную, чтобы убедиться в формате
	content, _ := os.ReadFile(goodFile)
	strContent := string(content)
	if !strings.Contains(strContent, "2") || !strings.Contains(strContent, "100 200") {
		t.Errorf("File content unexpected: %s", strContent)
	}

	// Сценарий 2: Ошибка создания файла (передаем директорию вместо файла или пустую строку)
	// В Linux/Unix попытка открыть директорию на запись вызовет ошибку.
	if err := s.SaveText(tmpDir); err == nil {
		t.Error("Expected error when saving to a directory path, got nil")
	}
}

func TestLoadText(t *testing.T) {
	tmpDir := t.TempDir()

	// Сценарий 1: Успешная загрузка
	goodFile := filepath.Join(tmpDir, "good_load.txt")
	// Создаем валидный файл: Размер 2, данные 10 20
	os.WriteFile(goodFile, []byte("2\n10 20"), 0644)

	s := NewStack[int]()
	if err := s.LoadText(goodFile); err != nil {
		t.Fatalf("LoadText failed: %v", err)
	}
	if s.Size() != 2 {
		t.Errorf("Expected size 2, got %d", s.Size())
	}

	// Сценарий 2: Ошибка открытия файла (нет файла)
	if err := s.LoadText(filepath.Join(tmpDir, "non_existent.txt")); err == nil {
		t.Error("Expected error loading non-existent file")
	}

	// Сценарий 3: Ошибка чтения размера (файл пустой или мусор)
	badSizeFile := filepath.Join(tmpDir, "bad_size.txt")
	os.WriteFile(badSizeFile, []byte("NOT_A_NUMBER"), 0644)
	if err := s.LoadText(badSizeFile); err == nil {
		t.Error("Expected error parsing invalid size")
	} else if !strings.Contains(err.Error(), "failed to read stack size") {
		t.Errorf("Unexpected error message: %v", err)
	}

	// Сценарий 4: Ошибка чтения данных (размер указан больше, чем данных)
	badDataFile := filepath.Join(tmpDir, "bad_data.txt")
	// Обещаем 5 элементов, даем 1
	os.WriteFile(badDataFile, []byte("5\n10"), 0644)
	if err := s.LoadText(badDataFile); err == nil {
		t.Error("Expected error reading insufficient data")
	} else if !strings.Contains(err.Error(), "failed to read data at index") {
		t.Errorf("Unexpected error message: %v", err)
	}
}

func TestSaveBinary(t *testing.T) {
	tmpDir := t.TempDir()
	goodFile := filepath.Join(tmpDir, "stack.bin")

	s := NewStack[float64]()
	s.Push(3.14)

	// Сценарий 1: Успешное сохранение
	if err := s.SaveBinary(goodFile); err != nil {
		t.Fatalf("SaveBinary failed: %v", err)
	}

	// Сценарий 2: Ошибка создания файла (путь - директория)
	if err := s.SaveBinary(tmpDir); err == nil {
		t.Error("Expected error saving binary to directory")
	}

	// Сценарий 3 (теоретический): Ошибка gob encoding.
	// gob очень надежен для базовых типов слайсов, вызвать ошибку encode
	// сложно без моков или специфичных типов, не поддерживаемых gob.
	// Но так как Stack[T] дженерик, можно попробовать создать стек с функцией или каналом,
	// которые gob не поддерживает.

	sFunc := NewStack[func()]()
	sFunc.Push(func() {}) // Функции нельзя сериализовать

	binFileFunc := filepath.Join(tmpDir, "func.bin")
	if err := sFunc.SaveBinary(binFileFunc); err == nil {
		t.Error("Expected gob encode error for unserializable type (func)")
	}
}

func TestLoadBinary(t *testing.T) {
	tmpDir := t.TempDir()

	// Подготовка файла для успешной загрузки
	goodFile := filepath.Join(tmpDir, "stack.bin")
	sOrig := NewStack[float64]()
	sOrig.Push(1.23)
	sOrig.SaveBinary(goodFile)

	// Сценарий 1: Успешная загрузка
	sLoad := NewStack[float64]()
	if err := sLoad.LoadBinary(goodFile); err != nil {
		t.Fatalf("LoadBinary failed: %v", err)
	}
	val, _ := sLoad.Pop()
	if math.Abs(val-1.23) > 0.0001 {
		t.Errorf("Expected 1.23, got %f", val)
	}

	// Сценарий 2: Ошибка открытия (файл не существует)
	if err := sLoad.LoadBinary("non_existent.bin"); err == nil {
		t.Error("Expected error opening non-existent binary file")
	}

	// Сценарий 3: Ошибка декодирования (коррумпированный файл)
	badFile := filepath.Join(tmpDir, "corrupt.bin")
	os.WriteFile(badFile, []byte("THIS IS NOT GOB DATA"), 0644)

	if err := sLoad.LoadBinary(badFile); err == nil {
		t.Error("Expected error decoding corrupt binary file")
	} else if !strings.Contains(err.Error(), "failed to decode data") {
		t.Errorf("Unexpected error message: %v", err)
	}
}
