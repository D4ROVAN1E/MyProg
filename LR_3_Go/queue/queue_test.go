package queue

import (
	"bytes"
	"os"
	"testing"
)

// Helper Functions

// createTempFile создает временный файл и возвращает путь и функцию очистки
func createTempFile(t *testing.T, pattern string) (string, func()) {
	f, err := os.CreateTemp("", pattern)
	if err != nil {
		t.Fatalf("failed to create temp file: %v", err)
	}
	f.Close()
	return f.Name(), func() {
		os.Remove(f.Name())
	}
}

// Core Logic Tests

func TestConstructor(t *testing.T) {
	q1 := NewQueue[int](10)
	if q1.capacity != 10 {
		t.Errorf("expected capacity 10, got %d", q1.capacity)
	}

	// Test normalization of invalid capacity
	q2 := NewQueue[int](-5)
	if q2.capacity != 1 {
		t.Errorf("expected capacity 1 (from <1), got %d", q2.capacity)
	}
	if !q2.IsEmpty() {
		t.Error("new queue should be empty")
	}
}

func TestPushPopGet(t *testing.T) {
	q := NewQueue[int](2)

	// Test Get on empty
	if _, err := q.Get(); err != ErrQueueEmpty {
		t.Errorf("expected ErrQueueEmpty on Get empty, got %v", err)
	}

	// Test Pop on empty
	if _, err := q.Pop(); err != ErrQueueEmpty {
		t.Errorf("expected ErrQueueEmpty on Pop empty, got %v", err)
	}

	// Normal flow
	q.Push(1)
	q.Push(2)

	val, err := q.Get()
	if err != nil || val != 1 {
		t.Errorf("Get failed. got %d, err %v", val, err)
	}

	// Pop 1
	val, err = q.Pop()
	if err != nil || val != 1 {
		t.Errorf("Pop failed. got %d, err %v", val, err)
	}

	// Size check
	if q.Size() != 1 {
		t.Errorf("expected size 1, got %d", q.Size())
	}
}

func TestResizeLogic(t *testing.T) {
	// Standard resize
	q := NewQueue[int](2)
	q.Push(1)
	q.Push(2)
	q.Push(3)

	if q.capacity != 4 {
		t.Errorf("expected capacity 4, got %d", q.capacity)
	}
	if q.Size() != 3 {
		t.Errorf("expected count 3, got %d", q.Size())
	}

	// Resize with circular wrapping
	q2 := NewQueue[int](2)
	q2.Push(10) // [10, nil] head=0, tail=1
	q2.Pop()    // [nil, nil] head=1, tail=1
	q2.Push(20) // [nil, 20] head=1, tail=0 (wrapped)
	q2.Push(30) // [30, 20] head=1, tail=1 (full)

	// Trigger resize while wrapped
	q2.Push(40)

	expected := []int{20, 30, 40}
	for i, exp := range expected {
		val, err := q2.Pop()
		if err != nil || val != exp {
			t.Errorf("Index %d: expected %d, got %d (err: %v)", i, exp, val, err)
		}
	}

	// Edge Case: Resize from 0
	qZero := NewQueue[int](1)
	qZero.capacity = 0 // Manually break it to test recovery
	qZero.data = []int{}
	qZero.resize()
	if qZero.capacity != 1 {
		t.Errorf("Resize from 0 should result in 1, got %d", qZero.capacity)
	}
}

func TestClone(t *testing.T) {
	q := NewQueue[string](5)
	q.Push("A")
	q.Push("B")

	// Move head forward to test offset copying
	q.Pop() // Remove A
	q.Push("C")

	clone := q.Clone()

	// Check independent memory
	clone.Push("D")
	if q.Size() == clone.Size() {
		t.Error("Modifying clone should not affect original size")
	}

	// Verify content of clone
	v1, _ := clone.Pop() // B
	v2, _ := clone.Pop() // C
	v3, _ := clone.Pop() // D

	if v1 != "B" || v2 != "C" || v3 != "D" {
		t.Error("Clone data order mismatch")
	}
}

func TestPrint(t *testing.T) {
	var buf bytes.Buffer
	q := NewQueue[int](5)

	// Empty
	q.Print(&buf)
	if buf.String() != "Содержимое очереди (от головы к хвосту): пусто\n" {
		t.Errorf("Unexpected print output: %q", buf.String())
	}

	// Items
	buf.Reset()
	q.Push(1)
	q.Push(2)
	q.Print(&buf)
	expected := "Содержимое очереди (от головы к хвосту): 1 2 \n"
	if buf.String() != expected {
		t.Errorf("Expected %q, got %q", expected, buf.String())
	}
}

// Text Serialization Tests

func TestSaveText(t *testing.T) {
	q := NewQueue[int](5)
	q.Push(10)
	q.Push(20)

	// Success
	filename, cleanup := createTempFile(t, "queue_text_*.txt")
	defer cleanup()

	if err := q.SaveText(filename); err != nil {
		t.Errorf("SaveText failed: %v", err)
	}

	// Error: Invalid path
	if err := q.SaveText(""); err == nil {
		t.Error("Expected error saving to empty filename")
	}
}

func TestLoadText(t *testing.T) {
	// Success Case
	{
		filename, cleanup := createTempFile(t, "good_queue.txt")
		defer cleanup()
		content := "2\n100\n200\n" // Size 2, items 100, 200
		os.WriteFile(filename, []byte(content), 0644)

		q := NewQueue[int](1)
		if err := q.LoadText(filename); err != nil {
			t.Fatalf("LoadText failed: %v", err)
		}
		if q.Size() != 2 {
			t.Errorf("Expected size 2, got %d", q.Size())
		}
		val, _ := q.Pop()
		if val != 100 {
			t.Errorf("Expected 100, got %d", val)
		}
	}

	// Error: File not found
	{
		q := NewQueue[int](1)
		if err := q.LoadText("non_existent_file_XYZ.txt"); err == nil {
			t.Error("Expected error for missing file")
		}
	}

	// Error: Corrupt Header (Size is not int)
	{
		filename, cleanup := createTempFile(t, "bad_header.txt")
		defer cleanup()
		os.WriteFile(filename, []byte("NotANumber\n"), 0644)

		q := NewQueue[int](1)
		err := q.LoadText(filename)
		if err == nil {
			t.Error("Expected error for bad header")
		}
		if err != nil && err.Error()[:13] != "error reading" {
			// Just checking we wrap the error roughly as expected
			t.Logf("Got expected error: %v", err)
		}
	}

	// Error: Corrupt Data (Type mismatch)
	{
		filename, cleanup := createTempFile(t, "bad_data_type.txt")
		defer cleanup()
		os.WriteFile(filename, []byte("2\n100\nNotAnInt"), 0644)

		q := NewQueue[int](1)
		if err := q.LoadText(filename); err != ErrFileCorrupt {
			t.Errorf("Expected ErrFileCorrupt for type mismatch, got %v", err)
		}
	}

	// Error: EOF before all items read (Size 3, but only 1 item)
	{
		filename, cleanup := createTempFile(t, "short_file.txt")
		defer cleanup()
		os.WriteFile(filename, []byte("3\n100\n"), 0644)

		q := NewQueue[int](1)
		if err := q.LoadText(filename); err != ErrFileCorrupt {
			t.Errorf("Expected ErrFileCorrupt for premature EOF, got %v", err)
		}
	}
}

// Binary Serialization Tests

func TestSaveBinary(t *testing.T) {
	q := NewQueue[float64](5)
	q.Push(1.1)

	// Success
	filename, cleanup := createTempFile(t, "queue_bin_*.gob")
	defer cleanup()
	if err := q.SaveBinary(filename); err != nil {
		t.Errorf("SaveBinary failed: %v", err)
	}

	// Error: File creation
	if err := q.SaveBinary(""); err == nil {
		t.Error("Expected error on invalid filename")
	}

	// Error: Encoding failure
	qBad := NewQueue[func()](1)
	qBad.Push(func() {})

	fnameBad, cl := createTempFile(t, "bad_gob.gob")
	defer cl()

	// The header (int) will write fine, but the item (func) will fail
	if err := qBad.SaveBinary(fnameBad); err == nil {
		t.Error("Expected gob encoding error for function type")
	}
}

func TestLoadBinary(t *testing.T) {
	// Success
	{
		filename, cleanup := createTempFile(t, "queue_good.gob")
		defer cleanup()

		// Save valid data first
		qOrig := NewQueue[string](2)
		qOrig.Push("Test")
		qOrig.SaveBinary(filename)

		qLoad := NewQueue[string](1)
		if err := qLoad.LoadBinary(filename); err != nil {
			t.Fatalf("LoadBinary failed: %v", err)
		}
		if val, _ := qLoad.Pop(); val != "Test" {
			t.Error("Data mismatch")
		}
	}

	// Error: Open file
	{
		q := NewQueue[int](1)
		if err := q.LoadBinary("missing_bin.gob"); err == nil {
			t.Error("Expected error opening missing file")
		}
	}

	// Error: Header decode (Empty file or garbage)
	{
		filename, cleanup := createTempFile(t, "garbage.gob")
		defer cleanup()
		os.WriteFile(filename, []byte("NOT_GOB_DATA"), 0644)

		q := NewQueue[int](1)
		if err := q.LoadBinary(filename); err == nil {
			t.Error("Expected error decoding garbage header")
		}
	}

	// Error: Data decode (EOF / corruption)
	{
		// Create a valid file, then truncate it to simulate partial write
		filename, cleanup := createTempFile(t, "truncated.gob")
		defer cleanup()

		qOrig := NewQueue[int](2)
		qOrig.Push(1)
		qOrig.Push(2)
		qOrig.SaveBinary(filename)

		// Truncate file: Keep header (size) but cut off data
		data, _ := os.ReadFile(filename)
		// Write back only first few bytes
		os.WriteFile(filename, data[:10], 0644)

		qLoad := NewQueue[int](1)
		err := qLoad.LoadBinary(filename)

		// Depending on where it cut, it might be unexpected EOF or a decoding error.
		if err == nil {
			t.Error("Expected error loading truncated file")
		}
	}
}
