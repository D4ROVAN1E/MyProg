package dhash

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
	"testing"
)

// Вспомогательные функции

// captureOutput перехватывает вывод в stdout
func captureOutput(f func()) string {
	r, w, _ := os.Pipe()
	stdout := os.Stdout
	os.Stdout = w

	f()

	w.Close()
	os.Stdout = stdout

	var buf bytes.Buffer
	io.Copy(&buf, r)
	return buf.String()
}

// Основные функциональные тесты

func TestConstructor(t *testing.T) {
	t.Run("ValidSize", func(t *testing.T) {
		dh, err := NewDoubleHash[int](10)
		if err != nil {
			t.Fatalf("Expected no error, got %v", err)
		}
		if dh.Size() != 0 {
			t.Errorf("Expected size 0, got %d", dh.Size())
		}
		if !dh.Empty() {
			t.Error("Expected empty table")
		}
	})

	t.Run("ZeroSize", func(t *testing.T) {
		_, err := NewDoubleHash[int](0)
		if err == nil {
			t.Error("Expected error when creating table with size 0")
		}
	})
}

func TestInsertFindUpdate(t *testing.T) {
	dh, _ := NewDoubleHash[string](5)

	// Вставка
	err := dh.Insert("key1", "value1")
	if err != nil {
		t.Errorf("Insert failed: %v", err)
	}
	if dh.Size() != 1 {
		t.Errorf("Expected size 1, got %d", dh.Size())
	}

	// Поиск
	val := dh.Find("key1")
	if val == nil || *val != "value1" {
		t.Error("Find returned wrong value or nil")
	}

	// Обновление значения
	err = dh.Insert("key1", "value2")
	if err != nil {
		t.Errorf("Update failed: %v", err)
	}
	if dh.Size() != 1 {
		t.Errorf("Size increased on update, expected 1, got %d", dh.Size())
	}
	val = dh.Find("key1")
	if val == nil || *val != "value2" {
		t.Error("Value not updated")
	}

	// Поиск несуществующего
	if dh.Find("missing") != nil {
		t.Error("Found non-existent key")
	}
}

func TestRemove(t *testing.T) {
	dh, _ := NewDoubleHash[int](5)

	// Удаление из пустой таблицы
	if dh.Remove("any") {
		t.Error("Remove on empty table should return false")
	}

	dh.Insert("A", 1)
	dh.Insert("B", 2)

	// Удаление несуществующего (с данными в таблице)
	if dh.Remove("C") {
		t.Error("Remove missing key should return false")
	}

	// Успешное удаление
	if !dh.Remove("A") {
		t.Error("Failed to remove existing key")
	}
	if dh.Size() != 1 {
		t.Errorf("Expected size 1, got %d", dh.Size())
	}
	if dh.Find("A") != nil {
		t.Error("Key A should be gone")
	}
}

func TestClear(t *testing.T) {
	dh, _ := NewDoubleHash[int](5)
	dh.Insert("A", 1)
	dh.Clear()
	if !dh.Empty() {
		t.Error("Table not empty after Clear")
	}
	if dh.Size() != 0 {
		t.Error("Size not 0 after Clear")
	}
}

func TestResize(t *testing.T) {
	// Создаем маленькую таблицу
	dh, _ := NewDoubleHash[int](2)

	dh.Insert("1", 1)
	dh.Insert("2", 2)

	prevSize := dh.Size()
	if prevSize != 2 {
		t.Errorf("Expected size 2, got %d", prevSize)
	}

	// Вставка третьего элемента должна вызвать resize
	dh.Insert("3", 3)

	if dh.Size() != 3 {
		t.Errorf("Expected size 3, got %d", dh.Size())
	}

	// Проверяем, что данные сохранились
	if *dh.Find("1") != 1 || *dh.Find("2") != 2 || *dh.Find("3") != 3 {
		t.Error("Data corruption after resize")
	}
}

func TestPrint(t *testing.T) {
	dh, _ := NewDoubleHash[int](5)
	dh.Insert("print_key", 999)

	out := captureOutput(func() {
		dh.Print()
	})

	if !strings.Contains(out, "print_key") {
		t.Error("Print output missing key")
	}
	if !strings.Contains(out, "999") {
		t.Error("Print output missing value")
	}
	if !strings.Contains(out, "=== Хэш-таблица ===") {
		t.Error("Print output missing header")
	}
}

// Тесты специфичной логики

// TestEvenTableSize проверяет логику hash2, когда размер таблицы четный.
func TestEvenTableSize(t *testing.T) {
	// Размер 4 (четный).
	dh, _ := NewDoubleHash[int](4)

	// Вставляем несколько ключей, чтобы статистически попасть в ветку с четным result
	keys := []string{"a", "b", "c", "d", "e", "f"}
	for i, k := range keys {
		dh.Insert(k, i)
	}

	// Проверяем, что все нашлось (значит коллизии и шаги разрешились корректно)
	for i, k := range keys {
		val := dh.Find(k)
		if val == nil || *val != i {
			t.Errorf("Failed to find key %s in even-sized table", k)
		}
	}
}

// Тесты сериализации (Text)

func TestSerializeText_Errors(t *testing.T) {
	dh, _ := NewDoubleHash[int](5)
	dh.Insert("A", 1)

	// Ошибка открытия файла на запись (пустое имя)
	if err := dh.SerializeText(""); err == nil {
		t.Error("Expected error for empty filename")
	}
}

func TestDeserializeText_FullFlow(t *testing.T) {
	tmpDir := t.TempDir()
	validFile := filepath.Join(tmpDir, "valid.txt")

	dhOriginal, _ := NewDoubleHash[float64](5)
	dhOriginal.Insert("pi", 3.14)
	dhOriginal.Insert("e", 2.71)

	// Успешное сохранение
	if err := dhOriginal.SerializeText(validFile); err != nil {
		t.Fatalf("SerializeText failed: %v", err)
	}

	// Успешная загрузка
	dhNew, _ := NewDoubleHash[float64](1)
	if err := dhNew.DeserializeText(validFile); err != nil {
		t.Fatalf("DeserializeText failed: %v", err)
	}
	if dhNew.Size() != 2 {
		t.Errorf("Expected 2 items, got %d", dhNew.Size())
	}

	// Тестирование ошибок загрузки

	// Файл не существует
	if err := dhNew.DeserializeText(filepath.Join(tmpDir, "missing.txt")); err == nil {
		t.Error("Expected error for missing file")
	}

	// Ошибка чтения заголовка (пустой файл)
	emptyFile := filepath.Join(tmpDir, "empty.txt")
	os.WriteFile(emptyFile, []byte(""), 0644)
	if err := dhNew.DeserializeText(emptyFile); err == nil {
		t.Error("Expected error for empty file header")
	}

	// Размер таблицы равен 0
	zeroSizeFile := filepath.Join(tmpDir, "zerosize.txt")
	os.WriteFile(zeroSizeFile, []byte("0 10"), 0644)
	if err := dhNew.DeserializeText(zeroSizeFile); err == nil || err.Error() != "size of table equal to zero" {
		t.Errorf("Expected 'size of table equal to zero', got %v", err)
	}

	// Ошибка формата данных (ожидаем int key T, даем мусор)
	badBodyFile := filepath.Join(tmpDir, "badbody.txt")
	// Размер 5, элементов 1. Строка: "индекс(ок) ключ(ок) значение(BAD)"
	os.WriteFile(badBodyFile, []byte("5 1\n0 key NOT_A_NUMBER"), 0644)
	if err := dhNew.DeserializeText(badBodyFile); err == nil {
		t.Error("Expected error for bad data body")
	}

	// Индекс превышает размер таблицы
	badIndexFile := filepath.Join(tmpDir, "badindex.txt")
	// Размер 5, элементов 1. Индекс 10 ( > 5)
	os.WriteFile(badIndexFile, []byte("5 1\n10 key 1.0"), 0644)
	if err := dhNew.DeserializeText(badIndexFile); err == nil {
		t.Error("Expected error for index out of bounds")
	} else if !strings.Contains(err.Error(), "exceeds table size") {
		t.Errorf("Expected 'exceeds table size' error, got: %v", err)
	}
}

// Тесты сериализации (Binary)

func TestSerializeBin_Errors(t *testing.T) {
	dh, _ := NewDoubleHash[int](5)
	dh.Insert("A", 1)

	// Ошибка открытия файла
	if err := dh.SerializeBin(""); err == nil {
		t.Error("Expected error for empty filename")
	}
}

func TestDeserializeBin_Errors(t *testing.T) {
	tmpDir := t.TempDir()
	dh, _ := NewDoubleHash[int](1)

	// Файл не существует
	if err := dh.DeserializeBin("non_existent.bin"); err == nil {
		t.Error("Expected error for missing bin file")
	}

	// Файл слишком короткий (нет заголовка)
	shortFile := filepath.Join(tmpDir, "short.bin")
	os.WriteFile(shortFile, []byte{1, 2}, 0644)
	if err := dh.DeserializeBin(shortFile); err == nil {
		t.Error("Expected error for short binary header")
	}

	// Неожиданный конец файла при чтении occupied флага
	// Записываем корректный заголовок (size=5, count=1), но ничего больше
	truncatedFile := filepath.Join(tmpDir, "truncated.bin")
	buf := new(bytes.Buffer)
	binary.Write(buf, binary.LittleEndian, uint32(5)) // size
	binary.Write(buf, binary.LittleEndian, uint32(1)) // count
	os.WriteFile(truncatedFile, buf.Bytes(), 0644)

	if err := dh.DeserializeBin(truncatedFile); err == nil {
		t.Error("Expected error when reading occupied flag from truncated file")
	}

	// Ошибка чтения длины ключа (occupied = true, но файла нет дальше)
	truncKeyLenFile := filepath.Join(tmpDir, "trunckeylen.bin")
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(5)) // size
	binary.Write(buf, binary.LittleEndian, uint32(1)) // count
	binary.Write(buf, binary.LittleEndian, true)      // occupied
	os.WriteFile(truncKeyLenFile, buf.Bytes(), 0644)

	if err := dh.DeserializeBin(truncKeyLenFile); err == nil {
		t.Error("Expected error when reading key len from truncated file")
	}

	// Длина ключа слишком большая (> 1000000)
	hugeKeyFile := filepath.Join(tmpDir, "hugekey.bin")
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(5))       // size
	binary.Write(buf, binary.LittleEndian, uint32(1))       // count
	binary.Write(buf, binary.LittleEndian, true)            // occupied
	binary.Write(buf, binary.LittleEndian, uint32(1000001)) // keyLen HUGE
	os.WriteFile(hugeKeyFile, buf.Bytes(), 0644)

	if err := dh.DeserializeBin(hugeKeyFile); err == nil {
		t.Error("Expected error for key length too large")
	} else if err.Error() != "failed to read key string" {
		t.Errorf("Expected 'failed to read key string', got %v", err)
	}

	// Ошибка чтения самого ключа (keyLen нормальный, но данных нет)
	noKeyDataFile := filepath.Join(tmpDir, "nokeydata.bin")
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(5)) // size
	binary.Write(buf, binary.LittleEndian, uint32(1)) // count
	binary.Write(buf, binary.LittleEndian, true)      // occupied
	binary.Write(buf, binary.LittleEndian, uint32(5)) // keyLen = 5
	// Не пишем байты ключа
	os.WriteFile(noKeyDataFile, buf.Bytes(), 0644)

	if err := dh.DeserializeBin(noKeyDataFile); err == nil {
		t.Error("Expected error when reading key data")
	}

	// Ошибка декодирования значения
	// Ситуация: записали всё корректно, но байты значения битые или не соответствуют типу
	badGobFile := filepath.Join(tmpDir, "badgob.bin")
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(5)) // size
	binary.Write(buf, binary.LittleEndian, uint32(1)) // count
	binary.Write(buf, binary.LittleEndian, true)      // occupied
	binary.Write(buf, binary.LittleEndian, uint32(3)) // keyLen = 3
	buf.WriteString("key")                            // key bytes
	// Вместо валидного gob для int пишем мусор
	buf.Write([]byte{0xFF, 0xFF, 0xFF})
	os.WriteFile(badGobFile, buf.Bytes(), 0644)

	if err := dh.DeserializeBin(badGobFile); err == nil {
		t.Error("Expected error for gob decode failure")
	}
}

func TestFullBinaryCycle(t *testing.T) {
	// Проверка "счастливого пути" для Binary
	tmpFile := filepath.Join(t.TempDir(), "ok.bin")

	dh, _ := NewDoubleHash[int32](5)
	dh.Insert("A", 100)
	// Создаем "дырку" (удаленный элемент), чтобы проверить false в occupied
	dh.Insert("B", 200)
	dh.Remove("B")

	if err := dh.SerializeBin(tmpFile); err != nil {
		t.Fatalf("SerializeBin failed: %v", err)
	}

	dhLoaded, _ := NewDoubleHash[int32](1)
	if err := dhLoaded.DeserializeBin(tmpFile); err != nil {
		t.Fatalf("DeserializeBin failed: %v", err)
	}

	if dhLoaded.Size() != 1 {
		t.Errorf("Expected size 1 (B removed), got %d", dhLoaded.Size())
	}
	if *dhLoaded.Find("A") != 100 {
		t.Error("Value A lost")
	}
	if dhLoaded.Find("B") != nil {
		t.Error("B should be removed")
	}
}

// TestCollisionCycle проверяет, что мы можем найти элемент, даже если он стоит
func TestCollisionCycle(t *testing.T) {

	dh, _ := NewDoubleHash[int](10)
	// Вставляем много элементов
	for i := 0; i < 8; i++ {
		key := fmt.Sprintf("k%d", i)
		dh.Insert(key, i)
	}

	// Проверяем поиск каждого
	for i := 0; i < 8; i++ {
		key := fmt.Sprintf("k%d", i)
		if val := dh.Find(key); val == nil || *val != i {
			t.Errorf("Failed to find key %s in populated table", key)
		}
	}
}
