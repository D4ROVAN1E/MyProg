package singlylist

import (
	"os"
	"path/filepath"
	"testing"
)

// checkListManual помогает проверить содержимое списка по порядку.
func checkListManual[T comparable](t *testing.T, list *ForwardList[T], expected []T) {
	t.Helper()

	// Проверка GetPrintString для пустых и непустых списков
	printStr := list.GetPrintString()
	if len(expected) == 0 {
		if list.Head != nil {
			t.Errorf("List should be empty, but Head is not nil")
		}
		if printStr != "Список пуст\n" {
			t.Errorf("Expected empty list string, got: %q", printStr)
		}
		return
	}

	current := list.Head
	for i, val := range expected {
		if current == nil {
			t.Errorf("Index %d: List ends prematurely, expected %v", i, val)
			return
		}
		if current.Key != val {
			t.Errorf("Index %d: got %v, expected %v", i, current.Key, val)
		}
		current = current.Next
	}
	if current != nil {
		t.Errorf("List contains more elements than expected")
	}
}

func TestNewListAndDeepCopy(t *testing.T) {
	// Empty Copy
	list := NewForwardList[int]()
	copyList := list.DeepCopy()
	checkListManual(t, copyList, []int{})

	// Non-empty Copy
	list.PushBack(1)
	list.PushBack(2)
	list.PushBack(3)

	copyList = list.DeepCopy()
	checkListManual(t, copyList, []int{1, 2, 3})

	// Verify deep copy independence
	list.DelHead()
	checkListManual(t, list, []int{2, 3})
	checkListManual(t, copyList, []int{1, 2, 3})
}

func TestPushOperations(t *testing.T) {
	list := NewForwardList[int]()

	// Create (resets list)
	list.PushBack(999)
	list.Create(10)
	checkListManual(t, list, []int{10})

	// PushHead
	list.PushHead(5)
	checkListManual(t, list, []int{5, 10})

	// PushBack on existing
	list.PushBack(20)
	checkListManual(t, list, []int{5, 10, 20})

	// PushBack on empty (explicit new list check)
	emptyList := NewForwardList[int]()
	emptyList.PushBack(100)
	checkListManual(t, emptyList, []int{100})
}

func TestPushForward(t *testing.T) {
	list := NewForwardList[int]()
	list.PushBack(10)
	list.PushBack(30)

	// Error case: nil pointer
	if err := list.PushForward(nil, 20); err == nil {
		t.Error("Expected error when pushing after nil")
	}

	// Success case
	head := list.Head
	if err := list.PushForward(head, 20); err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	checkListManual(t, list, []int{10, 20, 30})
}

func TestPushBefore(t *testing.T) {
	tests := []struct {
		name      string
		setup     []int
		target    int
		insert    int
		expectErr bool
		expected  []int
	}{
		{"Empty List", []int{}, 10, 5, true, []int{}},
		{"Head Match", []int{10, 20}, 10, 5, false, []int{5, 10, 20}},
		{"Middle Match", []int{10, 30}, 30, 20, false, []int{10, 20, 30}},
		{"Not Found", []int{10, 20}, 99, 5, true, []int{10, 20}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			list := NewForwardList[int]()
			for _, v := range tt.setup {
				list.PushBack(v)
			}

			err := list.PushBefore(tt.target, tt.insert)
			if (err != nil) != tt.expectErr {
				t.Errorf("PushBefore() error = %v, expectErr %v", err, tt.expectErr)
			}
			checkListManual(t, list, tt.expected)
		})
	}
}

func TestDelHeadAndBack(t *testing.T) {
	// DelHead
	l1 := NewForwardList[int]()
	if err := l1.DelHead(); err == nil {
		t.Error("DelHead on empty list should error")
	}

	l1.PushBack(1)
	l1.DelHead()
	checkListManual(t, l1, []int{})

	// DelBack
	l2 := NewForwardList[int]()
	if err := l2.DelBack(); err == nil {
		t.Error("DelBack on empty list should error")
	}

	// Single element case (Head.Next == nil)
	l2.PushBack(10)
	if err := l2.DelBack(); err != nil {
		t.Error(err)
	}
	checkListManual(t, l2, []int{})

	// Multiple elements
	l2.PushBack(10)
	l2.PushBack(20)
	l2.PushBack(30)
	l2.DelBack()
	checkListManual(t, l2, []int{10, 20})
}

func TestDelAfter(t *testing.T) {
	list := NewForwardList[int]()
	list.PushBack(10)
	list.PushBack(20)

	// Error: nil ptr
	if err := list.DelAfter(nil); err == nil {
		t.Error("Expected error for nil ptr")
	}

	// Error: no element after last
	tail := list.Head.Next
	if err := list.DelAfter(tail); err == nil {
		t.Error("Expected error when deleting after tail")
	}

	// Success
	head := list.Head
	if err := list.DelAfter(head); err != nil {
		t.Error(err)
	}
	checkListManual(t, list, []int{10})
}

func TestDelBefore(t *testing.T) {
	tests := []struct {
		name      string
		setup     []int
		target    int
		expectErr bool
		expected  []int
	}{
		{"Empty List", []int{}, 10, true, []int{}},
		{"Target is Head", []int{10, 20}, 10, true, []int{10, 20}},
		{"List too short (1 elem)", []int{10}, 10, true, []int{10}},
		{"Target is 2nd (Delete Head)", []int{10, 20, 30}, 20, false, []int{20, 30}},
		{"Target is 3rd (Normal)", []int{10, 20, 30, 40}, 30, false, []int{10, 30, 40}},
		{"Target Not Found", []int{10, 20, 30}, 99, true, []int{10, 20, 30}},
		{"Target Not Found (End of list)", []int{10, 20}, 99, true, []int{10, 20}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			list := NewForwardList[int]()
			for _, v := range tt.setup {
				list.PushBack(v)
			}
			err := list.DelBefore(tt.target)
			if (err != nil) != tt.expectErr {
				t.Errorf("DelBefore() error = %v, expectErr %v", err, tt.expectErr)
			}
			checkListManual(t, list, tt.expected)
		})
	}
}

func TestDelByValue(t *testing.T) {
	list := NewForwardList[int]()

	// Empty
	if err := list.DelByValue(10); err == nil {
		t.Error("Expect error on empty")
	}

	list.PushBack(10)
	list.PushBack(20)
	list.PushBack(30)

	// Head
	list.DelByValue(10)
	checkListManual(t, list, []int{20, 30})

	// Middle/End
	list.DelByValue(30)
	checkListManual(t, list, []int{20})

	// Not found
	if err := list.DelByValue(99); err == nil {
		t.Error("Expect error when not found")
	}
}

func TestGetByValue(t *testing.T) {
	list := NewForwardList[int]()
	if list.GetByValue(10) != nil {
		t.Error("Should return nil on empty")
	}

	list.PushBack(10)
	list.PushBack(20)

	if node := list.GetByValue(10); node == nil || node.Key != 10 {
		t.Error("Failed to get 10")
	}
	if node := list.GetByValue(99); node != nil {
		t.Error("Should return nil for missing")
	}
}

func TestPrint(t *testing.T) {
	list := NewForwardList[int]()
	list.Print() // "Список пуст"

	list.PushBack(1)
	list.PushBack(2)
	list.Print() // "1 -> 2 -> nil"
}

func TestFileIOSaveLoad(t *testing.T) {
	list := NewForwardList[int]()
	list.PushBack(10)
	list.PushBack(20)

	tmpDir := t.TempDir()
	validFile := filepath.Join(tmpDir, "list.txt")

	// Save Success
	if err := list.Save(validFile); err != nil {
		t.Fatalf("Save failed: %v", err)
	}

	// Load Success
	list2 := NewForwardList[int]()
	if err := list2.Load(validFile); err != nil {
		t.Fatalf("Load failed: %v", err)
	}
	checkListManual(t, list2, []int{10, 20})

	// Load logic: check correct append ordering
	// Create a file manually: "1 2 3"
	manualFile := filepath.Join(tmpDir, "manual.txt")
	os.WriteFile(manualFile, []byte("1 2 3"), 0644)
	list3 := NewForwardList[int]()
	list3.Load(manualFile)
	checkListManual(t, list3, []int{1, 2, 3})

	// Save Error (Invalid path)
	if err := list.Save(filepath.Join(tmpDir, "nonexistent_dir", "file.txt")); err == nil {
		t.Error("Expected error saving to invalid path")
	}

	// Load Error (File not found)
	if err := list.Load(filepath.Join(tmpDir, "missing.txt")); err == nil {
		t.Error("Expected error loading missing file")
	}

	// Load Error (Bad data format)
	badFile := filepath.Join(tmpDir, "bad.txt")
	os.WriteFile(badFile, []byte("10 ABC 30"), 0644)
	if err := list.Load(badFile); err == nil {
		t.Error("Expected error loading corrupt data")
	}
}

func TestFileIOSerialize(t *testing.T) {
	list := NewForwardList[int]()
	list.PushBack(100)
	list.PushBack(200)

	tmpDir := t.TempDir()
	validBin := filepath.Join(tmpDir, "list.bin")

	// Serialize Success
	if err := list.Serialize(validBin); err != nil {
		t.Fatalf("Serialize failed: %v", err)
	}

	// Deserialize Success
	list2 := NewForwardList[int]()
	if err := list2.Deserialize(validBin); err != nil {
		t.Fatalf("Deserialize failed: %v", err)
	}
	checkListManual(t, list2, []int{100, 200})

	// Serialize Error (Invalid path)
	if err := list.Serialize(filepath.Join(tmpDir, "no_dir", "file.bin")); err == nil {
		t.Error("Expected error serializing to invalid path")
	}

	// Deserialize Error (File not found)
	if err := list2.Deserialize(filepath.Join(tmpDir, "missing.bin")); err == nil {
		t.Error("Expected error deserializing missing file")
	}

	// Deserialize Error (Bad Gob data)
	badBin := filepath.Join(tmpDir, "bad.bin")
	os.WriteFile(badBin, []byte("Not a gob file"), 0644)
	if err := list2.Deserialize(badBin); err == nil {
		t.Error("Expected error deserializing bad gob data")
	}

	// Deserialize Empty/EOF (Should be valid empty list)
	emptyBin := filepath.Join(tmpDir, "empty.bin")
	os.Create(emptyBin) // create empty file
	listEmpty := NewForwardList[int]()
	if err := listEmpty.Deserialize(emptyBin); err != nil {
		t.Errorf("Deserializing empty file should not error, got: %v", err)
	}
	checkListManual(t, listEmpty, []int{})
}
