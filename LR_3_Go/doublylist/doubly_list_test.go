package doublylist

import (
	"bytes"
	"math"
	"os"
	"strings"
	"testing"
)

// checkHeadTail — вспомогательная функция для проверки целостности головы и хвоста
func checkHeadTail[T comparable](t *testing.T, list *DoublyList[T], headVal, tailVal T) {
	t.Helper()
	if list.Head == nil || list.Head.Key != headVal {
		t.Errorf("Expected Head %v, got %v", headVal, list.Head)
	}
	if list.Tail == nil || list.Tail.Key != tailVal {
		t.Errorf("Expected Tail %v, got %v", tailVal, list.Tail)
	}
}

func TestLCreate(t *testing.T) {
	list := NewDoublyList[int]()

	// Успешное создание
	err := list.LCreate(10)
	if err != nil {
		t.Errorf("LCreate failed: %v", err)
	}
	checkHeadTail(t, list, 10, 10)

	// Ошибка: список уже создан
	if err := list.LCreate(20); err == nil {
		t.Error("Expected error when calling LCreate on non-empty list")
	}
}

func TestPushAndStructure(t *testing.T) {
	list := NewDoublyList[int]()

	// LPushHead в пустой
	list.LPushHead(10)
	checkHeadTail(t, list, 10, 10)

	// LPushHead в непустой (обновление Prev старой головы)
	list.LPushHead(5)
	checkHeadTail(t, list, 5, 10)
	if list.Head.Next.Key != 10 || list.Head.Next.Prev.Key != 5 {
		t.Error("Links broken after LPushHead")
	}

	// LPushBack в непустой
	list.LPushBack(20)
	checkHeadTail(t, list, 5, 20) // [5, 10, 20]
	if list.Tail.Prev.Key != 10 || list.Tail.Prev.Next.Key != 20 {
		t.Error("Links broken after LPushBack")
	}

	// LPushBack в пустой
	list2 := NewDoublyList[int]()
	list2.LPushBack(100)
	checkHeadTail(t, list2, 100, 100)
}

func TestBasicDelete(t *testing.T) {
	list := NewDoublyList[int]()

	// Ошибки удаления из пустого списка
	if err := list.LDelHead(); err == nil {
		t.Error("Expected error deleting head from empty list")
	}
	if err := list.LDelBack(); err == nil {
		t.Error("Expected error deleting tail from empty list")
	}

	// Заполняем: [1, 2, 3]
	list.LPushBack(1)
	list.LPushBack(2)
	list.LPushBack(3)

	// Удаление головы (обычный случай) -> [2, 3]
	_ = list.LDelHead()
	checkHeadTail(t, list, 2, 3)
	if list.Head.Prev != nil {
		t.Error("Head.Prev should be nil")
	}

	// Удаление хвоста (обычный случай) -> [2]
	_ = list.LDelBack()
	checkHeadTail(t, list, 2, 2)
	if list.Tail.Next != nil {
		t.Error("Tail.Next should be nil")
	}

	// Удаление единственного элемента через Head -> []
	_ = list.LDelHead()
	if list.Head != nil || list.Tail != nil {
		t.Error("List should be empty after deleting last element via Head")
	}

	// Удаление единственного элемента через Back -> []
	list.LPushBack(100)
	_ = list.LDelBack()
	if list.Head != nil || list.Tail != nil {
		t.Error("List should be empty after deleting last element via Back")
	}
}

func TestPushRelative(t *testing.T) {
	list := NewDoublyList[int]()
	list.LPushBack(10)
	list.LPushBack(20)

	// Push Before

	// Ошибка: элемент не найден
	if err := list.LPushBefore(999, 5); err == nil {
		t.Error("Expected error for PushBefore unknown element")
	}

	// Случай: Вставка перед Head
	list.LPushBefore(10, 5) // [5, 10, 20]
	checkHeadTail(t, list, 5, 20)

	// Случай: Вставка в середине
	list.LPushBefore(20, 15) // [5, 10, 15, 20]
	node15 := list.LGetByValue(15)
	if node15.Prev.Key != 10 || node15.Next.Key != 20 {
		t.Error("Links broken after PushBefore (middle)")
	}

	// Push After

	// Ошибка: элемент не найден
	if err := list.LPushAfter(999, 30); err == nil {
		t.Error("Expected error for PushAfter unknown element")
	}

	// Случай: Вставка после Tail
	list.LPushAfter(20, 25) // [..., 20, 25]
	checkHeadTail(t, list, 5, 25)

	// Случай: Вставка в середине
	list.LPushAfter(10, 12) // [..., 10, 12, 15, ...]
	node12 := list.LGetByValue(12)
	if node12.Prev.Key != 10 || node12.Next.Key != 15 {
		t.Error("Links broken after PushAfter (middle)")
	}
}

func TestDeleteRelative(t *testing.T) {
	// LDelAfter
	list := NewDoublyList[int]()
	list.LPushBack(1)
	list.LPushBack(2)
	list.LPushBack(3)

	// Ошибка: элемент не найден
	if err := list.LDelAfter(99); err == nil {
		t.Error("Expected error LDelAfter unknown")
	}
	// Ошибка: нет элемента после (Tail)
	if err := list.LDelAfter(3); err == nil {
		t.Error("Expected error LDelAfter tail")
	}

	// Edge Case: Удаление элемента, который является Tail (удаляем 3 после 2)
	list.LDelAfter(2) // [1, 2]
	checkHeadTail(t, list, 1, 2)

	// Standard Case: Удаление из середины (удаляем 2 после 1) -> [1, 2(del), 3(new pushed)]
	list.LPushBack(3) // restore [1, 2, 3]
	list.LDelAfter(1) // [1, 3]
	if list.Head.Next.Key != 3 || list.Tail.Prev.Key != 1 {
		t.Error("Links broken after LDelAfter middle")
	}

	// LDelBefore
	list2 := NewDoublyList[int]()
	list2.LPushBack(1)
	list2.LPushBack(2)
	list2.LPushBack(3)

	// Ошибка: элемент не найден
	if err := list2.LDelBefore(99); err == nil {
		t.Error("Expected error LDelBefore unknown")
	}
	// Ошибка: нет элемента перед (Head)
	if err := list2.LDelBefore(1); err == nil {
		t.Error("Expected error LDelBefore head")
	}

	// Edge Case: Удаление элемента, который является Head (удаляем 1 перед 2)
	list2.LDelBefore(2) // [2, 3]
	checkHeadTail(t, list2, 2, 3)

	// Standard Case: Удаление из середины
	list2.LPushHead(1)  // restore [1, 2, 3]
	list2.LDelBefore(3) // [1, 3] (удаляем 2)
	if list2.Head.Next.Key != 3 || list2.Tail.Prev.Key != 1 {
		t.Error("Links broken after LDelBefore middle")
	}
}

func TestDelByValue(t *testing.T) {
	list := NewDoublyList[int]()
	list.LPushBack(1)
	list.LPushBack(2)
	list.LPushBack(3)

	// Не найден
	if err := list.LDelByValue(99); err == nil {
		t.Error("Should fail if value not found")
	}

	// Удаление Head
	list.LDelByValue(1) // [2, 3]
	checkHeadTail(t, list, 2, 3)

	// Удаление Tail
	list.LDelByValue(3) // [2]
	checkHeadTail(t, list, 2, 2)

	// Удаление Middle
	list.LPushHead(1)
	list.LPushBack(3)   // [1, 2, 3]
	list.LDelByValue(2) // [1, 3]
	if list.Head.Next.Key != 3 {
		t.Error("Links broken after LDelByValue middle")
	}
}

func TestGetByValue(t *testing.T) {
	list := NewDoublyList[int]()
	list.LPushBack(10)

	if node := list.LGetByValue(10); node == nil || node.Key != 10 {
		t.Error("Failed to get existing value")
	}

	if node := list.LGetByValue(999); node != nil {
		t.Error("Should return nil for non-existing value")
	}
}

func TestClone(t *testing.T) {
	list1 := NewDoublyList[int]()
	list1.LPushBack(1)

	// Клон непустого
	list2 := list1.Clone()
	if list2.Head.Key != 1 {
		t.Error("Clone failed")
	}

	// Проверка независимости
	list1.Head.Key = 999
	if list2.Head.Key == 999 {
		t.Error("Clone is shallow, expected deep copy")
	}

	// Клон пустого
	list3 := NewDoublyList[int]()
	list4 := list3.Clone()
	if list4.Head != nil {
		t.Error("Clone of empty list should be empty")
	}
}

func TestPrint(t *testing.T) {
	list := NewDoublyList[int]()
	var buf bytes.Buffer

	// Пустой список (Forward)
	list.Print(&buf, 1)
	if !strings.Contains(buf.String(), "Список пуст") {
		t.Error("Wrong output for empty list forward")
	}
	buf.Reset()

	// Пустой список (Backward)
	list.Print(&buf, 2)
	if !strings.Contains(buf.String(), "Список пуст") {
		t.Error("Wrong output for empty list backward")
	}
	buf.Reset()

	// Заполняем
	list.LPushBack(1)
	list.LPushBack(2)

	// Невалидный выбор
	if err := list.Print(&buf, 3); err == nil {
		t.Error("Expected error for invalid print choice")
	}

	// Нормальный вывод Forward
	buf.Reset()
	list.Print(&buf, 1)
	out := buf.String()
	if !strings.Contains(out, "1 <-> 2") {
		t.Errorf("Forward print mismatch: %s", out)
	}

	// Нормальный вывод Backward
	buf.Reset()
	list.Print(&buf, 2)
	out = buf.String()
	if !strings.Contains(out, "2 <-> 1") {
		t.Errorf("Backward print mismatch: %s", out)
	}
}

func TestFileIO_Text(t *testing.T) {
	list := NewDoublyList[int]()
	list.LPushBack(10)
	list.LPushBack(20)

	fname := "test_text.txt"
	defer os.Remove(fname)

	// Успешное сохранение
	if err := list.LSave(fname); err != nil {
		t.Fatalf("LSave failed: %v", err)
	}

	// Успешная загрузка
	list2 := NewDoublyList[int]()
	if err := list2.LLoad(fname); err != nil {
		t.Fatalf("LLoad failed: %v", err)
	}
	checkHeadTail(t, list2, 10, 20)

	// Ошибка: Загрузка несуществующего файла
	if err := list.LLoad("non_existent_file.txt"); err == nil {
		t.Error("Expected error loading non-existent file")
	}

	// Ошибка: Сохранение в недопустимый путь
	if err := list.LSave(""); err == nil {
		t.Error("Expected error saving to invalid path")
	}
}

func TestFileIO_Binary(t *testing.T) {
	list := NewDoublyList[float64]()
	list.LPushBack(1.1)
	list.LPushBack(2.2)

	fname := "test_bin.bin"
	defer os.Remove(fname)

	// Успешное сохранение
	if err := list.LSaveBin(fname); err != nil {
		t.Fatalf("LSaveBin failed: %v", err)
	}

	// Успешная загрузка
	list2 := NewDoublyList[float64]()
	if err := list2.LLoadBin(fname); err != nil {
		t.Fatalf("LLoadBin failed: %v", err)
	}

	if math.Abs(list2.Head.Key-1.1) > 0.001 {
		t.Error("Binary load value mismatch")
	}

	// Ошибка: Загрузка несуществующего файла
	if err := list.LLoadBin("non_existent_bin.bin"); err == nil {
		t.Error("Expected error loading non-existent binary file")
	}

	// Ошибка: Сохранение в недопустимый путь
	if err := list.LSaveBin(""); err == nil {
		t.Error("Expected error saving binary to invalid path")
	}
}
