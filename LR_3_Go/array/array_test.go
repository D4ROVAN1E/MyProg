package array

import (
	"math"
	"os"
	"testing"
)

// cleanFile — вспомогательная функция для очистки после тестов
func cleanFile(filename string) {
	os.Remove(filename)
}

// TestConstructors покрывает NewArray и NewArrayWithCap
func TestConstructors(t *testing.T) {
	// Стандартный конструктор
	arr := NewArray[int]()
	if arr.GetSize() != 0 || arr.GetCapacity() != 1 {
		t.Errorf("NewArray failed: size=%d, cap=%d", arr.GetSize(), arr.GetCapacity())
	}

	// Конструктор с емкостью
	arrCap := NewArrayWithCap[int](10)
	if arrCap.GetCapacity() != 10 {
		t.Errorf("NewArrayWithCap failed capacity: got %d", arrCap.GetCapacity())
	}
	if arrCap.GetSize() != 9 {
		t.Errorf("NewArrayWithCap logic mismatch: expected size 9, got %d", arrCap.GetSize())
	}

	// Edge Case: Емкость < 1
	arrNeg := NewArrayWithCap[int](-5)
	if arrNeg.GetCapacity() != 1 {
		t.Errorf("NewArrayWithCap negative check failed: expected cap 1, got %d", arrNeg.GetCapacity())
	}
	// size = cap - 1 => 1 - 1 = 0
	if arrNeg.GetSize() != 0 {
		t.Errorf("NewArrayWithCap negative check failed: expected size 0, got %d", arrNeg.GetSize())
	}
}

// TestCoreOperations покрывает Get, Set, PushBack
func TestCoreOperations(t *testing.T) {
	arr := NewArray[string]()
	arr.PushBack("A")
	arr.PushBack("B")

	// Valid Get
	val, err := arr.Get(1)
	if err != nil || val != "B" {
		t.Errorf("Get failed")
	}

	// Invalid Get (Index < 0)
	_, err = arr.Get(-1)
	if err == nil {
		t.Error("Expected error for Get(-1)")
	}

	// Invalid Get (Index >= Size)
	_, err = arr.Get(2)
	if err == nil {
		t.Error("Expected error for Get(2)")
	}

	// Valid Set
	err = arr.Set(0, "C")
	if err != nil {
		t.Errorf("Set failed: %v", err)
	}
	v, _ := arr.Get(0)
	if v != "C" {
		t.Errorf("Set did not update value")
	}

	// Invalid Set
	if err := arr.Set(100, "Z"); err == nil {
		t.Error("Expected error for Set out of bounds")
	}
}

// TestInsertDeleteSwap покрывает InsertByInd, DeleteByInd, SwapByInd
func TestInsertDeleteSwap(t *testing.T) {
	arr := NewArray[int]()
	arr.PushBack(10) // [10]

	// Insert в конец (особая ветка if index == len)
	err := arr.InsertByInd(1, 20) // [10, 20]
	if err != nil {
		t.Errorf("Insert at end failed: %v", err)
	}
	if arr.GetSize() != 2 {
		t.Errorf("Size incorrect after insert")
	}

	// Insert в середину
	arr.InsertByInd(0, 5) // [5, 10, 20]
	v, _ := arr.Get(0)
	if v != 5 {
		t.Errorf("Insert at 0 failed")
	}

	// Insert Error
	if err := arr.InsertByInd(50, 1); err == nil {
		t.Error("Insert out of bounds should fail")
	}

	// Delete
	arr.DeleteByInd(1) // удаляем 10 -> [5, 20]
	if arr.GetSize() != 2 {
		t.Errorf("Delete size check failed")
	}
	v2, _ := arr.Get(1)
	if v2 != 20 {
		t.Errorf("Delete logic check failed")
	}

	// Delete Error
	if err := arr.DeleteByInd(5); err == nil {
		t.Error("Delete out of bounds should fail")
	}

	// Swap
	arr.SwapByInd(0, 99)
	v0, _ := arr.Get(0)
	if v0 != 99 {
		t.Errorf("Swap failed")
	}

	// Swap Error
	if err := arr.SwapByInd(-1, 1); err == nil {
		t.Error("Swap invalid index should fail")
	}
}

// TestSizeAndCapacity покрывает SetCapacity, SetSize, Clone
func TestSizeAndCapacity(t *testing.T) {
	arr := NewArray[int]()
	arr.PushBack(1)
	arr.PushBack(2)

	// Clone
	clone := arr.Clone()
	if clone.GetSize() != 2 {
		t.Error("Clone size mismatch")
	}
	clone.Set(0, 100)
	origVal, _ := arr.Get(0)
	if origVal == 100 {
		t.Error("Clone is shallow, expected deep copy")
	}

	// SetCapacity Valid
	if err := arr.SetCapacity(10); err != nil {
		t.Error(err)
	}
	if arr.GetCapacity() != 10 {
		t.Error("SetCapacity failed")
	}

	// SetCapacity Invalid (New < Size)
	if err := arr.SetCapacity(1); err == nil {
		t.Error("SetCapacity should fail if new cap < size")
	}

	// SetSize Valid
	if err := arr.SetSize(1); err != nil {
		t.Error(err)
	}
	if arr.GetSize() != 1 {
		t.Error("SetSize failed")
	}

	// SetSize Invalid (New > Cap)
	if err := arr.SetSize(20); err == nil {
		t.Error("SetSize should fail if new size > cap")
	}
}

// TestIO_Text покрывает SaveText и LoadText со всеми ошибками
func TestIO_Text(t *testing.T) {
	filename := "test_text.txt"
	defer cleanFile(filename)

	arr := NewArray[int]()
	arr.PushBack(10)
	arr.PushBack(20)

	// Успешное сохранение
	if err := arr.SaveText(filename); err != nil {
		t.Fatalf("SaveText failed: %v", err)
	}

	// Успешная загрузка
	arrLoad := NewArray[int]()
	if err := arrLoad.LoadText(filename); err != nil {
		t.Fatalf("LoadText failed: %v", err)
	}
	if arrLoad.GetSize() != 2 {
		t.Error("LoadText size mismatch")
	}

	// Ошибка сохранения (недопустимое имя файла/папки)
	if err := arr.SaveText("."); err == nil {
		// Если вдруг "." разрешено, попробуем несуществующую директорию
		if err := arr.SaveText("/non_existent_dir/file.txt"); err == nil {
			t.Error("SaveText should fail for bad path")
		}
	}

	// Ошибка загрузки (файл не найден)
	if err := arrLoad.LoadText("missing.txt"); err == nil {
		t.Error("LoadText should fail for missing file")
	}

	// Ошибка загрузки (битый файл: нет размера)
	emptyFile := "empty.txt"
	os.WriteFile(emptyFile, []byte(""), 0644)
	defer cleanFile(emptyFile)
	if err := arrLoad.LoadText(emptyFile); err == nil {
		t.Error("LoadText should fail on empty file (no size)")
	}

	// Ошибка загрузки (битый файл: размер есть, данных нет)
	corruptFile := "corrupt.txt"
	// Пишем: "5 10" (размер 5, а элементов только 1)
	os.WriteFile(corruptFile, []byte("5 10"), 0644)
	defer cleanFile(corruptFile)
	if err := arrLoad.LoadText(corruptFile); err == nil {
		t.Error("LoadText should fail when data is incomplete")
	}
}

// TestIO_Binary покрывает SaveBinary и LoadBinary
func TestIO_Binary(t *testing.T) {
	filename := "test_bin.bin"
	defer cleanFile(filename)

	arr := NewArray[float64]()
	arr.PushBack(1.23)
	arr.PushBack(4.56)

	// Успешное сохранение
	if err := arr.SaveBinary(filename); err != nil {
		t.Fatalf("SaveBinary failed: %v", err)
	}

	// Успешная загрузка
	arrLoad := NewArray[float64]()
	if err := arrLoad.LoadBinary(filename); err != nil {
		t.Fatalf("LoadBinary failed: %v", err)
	}
	val, _ := arrLoad.Get(0)
	if math.Abs(val-1.23) > 0.0001 {
		t.Error("Binary load value mismatch")
	}

	// Ошибка сохранения: недопустимый тип для binary.Write
	// binary.Write не умеет писать string автоматически
	arrStr := NewArray[string]()
	arrStr.PushBack("Fail")
	if err := arrStr.SaveBinary("fail.bin"); err == nil {
		cleanFile("fail.bin")
		t.Error("SaveBinary should fail for non-fixed size types (string)")
	} else {
		cleanFile("fail.bin") // на случай если файл создался 0 байт
	}

	// Ошибка сохранения: ошибка открытия файла
	if err := arr.SaveBinary("/nop/file.bin"); err == nil {
		t.Error("SaveBinary should fail for bad path")
	}

	// Ошибка загрузки: файл не найден
	if err := arrLoad.LoadBinary("missing.bin"); err == nil {
		t.Error("LoadBinary should fail for missing file")
	}

	// Ошибка загрузки: битый размер (пустой файл)
	badSizeFile := "bad_size.bin"
	os.WriteFile(badSizeFile, []byte{}, 0644)
	defer cleanFile(badSizeFile)
	if err := arrLoad.LoadBinary(badSizeFile); err == nil {
		t.Error("LoadBinary should fail reading size from empty file")
	}

	// Ошибка загрузки: неполные данные
	// Создадим файл с корректным заголовком (размер 10), но без данных
	incompleteFile := "incomplete.bin"
	f, _ := os.Create(incompleteFile)
	// Записываем int32 = 10
	importBinaryWrite(t, f, int32(10))
	f.Close()
	defer cleanFile(incompleteFile)

	if err := arrLoad.LoadBinary(incompleteFile); err == nil {
		t.Error("LoadBinary should fail when data is incomplete")
	}
}

// Вспомогательная функция для теста бинарной записи
func importBinaryWrite(t *testing.T, f *os.File, val int32) {
	// Ручная запись little endian int32 (10, 0, 0, 0)
	data := []byte{byte(val), byte(val >> 8), byte(val >> 16), byte(val >> 24)}
	f.Write(data)
}
