package cuckoo

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"math"
	"os"
	"strings"
	"testing"
)

// Вспомогательная функция для перехвата stdout
func captureOutput(f func()) string {
	r, w, _ := os.Pipe()
	origOut := os.Stdout
	os.Stdout = w

	f()

	w.Close()
	os.Stdout = origOut

	var buf bytes.Buffer
	io.Copy(&buf, r)
	return buf.String()
}

// Core Functionality

func TestConstructionAndEmpty(t *testing.T) {
	// Тест дефолтной инициализации
	hash := NewCuckooHash[int](0)
	if !hash.Empty() {
		t.Error("Expected empty hash")
	}
	if hash.Size() != 0 {
		t.Errorf("Expected size 0, got %d", hash.Size())
	}
	// Проверка внутренней tableSize (должно стать 3)
	if hash.tableSize != 3 {
		t.Errorf("Expected internal tableSize 3, got %d", hash.tableSize)
	}
}

func TestInsertFindUpdate(t *testing.T) {
	hash := NewCuckooHash[int](3)

	// Вставка
	hash.Insert("key1", 100)
	if hash.Empty() {
		t.Error("Hash should not be empty")
	}
	if hash.Size() != 1 {
		t.Errorf("Expected size 1, got %d", hash.Size())
	}

	// Поиск существующего (h1)
	val := hash.Find("key1")
	if val == nil {
		t.Fatal("key1 not found")
	}
	if *val != 100 {
		t.Errorf("Expected 100, got %d", *val)
	}

	// Поиск несуществующего
	if hash.Find("key_missing") != nil {
		t.Error("Found non-existent key")
	}

	// Обновление (Update logic)
	hash.Insert("key1", 200)
	if hash.Size() != 1 {
		t.Errorf("Size changed after update, expected 1, got %d", hash.Size())
	}
	val = hash.Find("key1")
	if *val != 200 {
		t.Errorf("Expected 200 after update, got %d", *val)
	}
}

// TestHash2CollisionAndUpdate форсирует использование второй хеш-функции
func TestHash2CollisionAndUpdate(t *testing.T) {
	// Создаем ситуацию, где элемент обязан лежать во второй позиции.
	// Мы используем доступ к кишкам (hash1), чтобы найти коллизию.
	hash := NewCuckooHash[int](10)

	// Находим два ключа с одинаковым hash1
	var key1, key2 string
	targetMap := make(map[uint32]string)

	for i := 0; i < 1000; i++ {
		k := fmt.Sprintf("collision_cand_%d", i)
		h1 := hash.hash1(k)
		if existing, ok := targetMap[h1]; ok {
			key1 = existing
			key2 = k
			break
		}
		targetMap[h1] = k
	}

	if key1 == "" || key2 == "" {
		t.Fatal("Could not generate hash1 collision for test")
	}

	hash.Insert(key1, 1)
	hash.Insert(key2, 2) // key2 должен вытолкнуть key1 или встать на hash2

	// Проверяем, что оба на месте
	v1 := hash.Find(key1)
	v2 := hash.Find(key2)

	if v1 == nil || *v1 != 1 {
		t.Error("Key1 lost or wrong value")
	}
	if v2 == nil || *v2 != 2 {
		t.Error("Key2 lost or wrong value")
	}

	// Теперь обновляем key2. Если он лежит в слоте hash2, это проверит ветку обновления hash2
	hash.Insert(key2, 999)
	v2 = hash.Find(key2)
	if *v2 != 999 {
		t.Errorf("Failed to update key that might be in hash2 slot. Got %d", *v2)
	}
}

func TestRemove(t *testing.T) {
	hash := NewCuckooHash[string](3)
	hash.Insert("alpha", "val1")
	hash.Insert("beta", "val2")

	if hash.Size() != 2 {
		t.Errorf("Expected size 2, got %d", hash.Size())
	}

	// Удаление существующего
	if !hash.Remove("alpha") {
		t.Error("Failed to remove alpha")
	}
	if hash.Size() != 1 {
		t.Errorf("Expected size 1, got %d", hash.Size())
	}
	if hash.Find("alpha") != nil {
		t.Error("alpha still exists after removal")
	}

	// Удаление несуществующего
	if hash.Remove("omega") {
		t.Error("Removed non-existent key omega")
	}
	if hash.Size() != 1 {
		t.Errorf("Size changed after failed remove, expected 1, got %d", hash.Size())
	}

	// Очистка
	hash.Clear()
	if !hash.Empty() {
		t.Error("Hash not empty after Clear")
	}
	if hash.Size() != 0 {
		t.Error("Size not 0 after clear")
	}
}

// TestRemoveFromHash2Slot проверяет удаление элемента, который находится по адресу hash2
func TestRemoveFromHash2Slot(t *testing.T) {
	hash := NewCuckooHash[int](10)
	// Пытаемся создать ситуацию вытеснения
	key1 := "k1"
	// Вставляем key1
	hash.Insert(key1, 1)
	h1_first := hash.hash1(key1)

	// Генерируем key2 с таким же h1
	var key2 string
	for i := 0; i < 1000; i++ {
		k := fmt.Sprintf("k%d", i)
		if k != key1 && hash.hash1(k) == h1_first {
			key2 = k
			break
		}
	}

	// Вставляем key2. Он должен занять слот h1, а key1 улетит
	hash.Insert(key2, 2)

	// Удаляем оба
	if !hash.Remove(key1) {
		t.Errorf("Failed to remove key1")
	}
	if !hash.Remove(key2) {
		t.Errorf("Failed to remove key2")
	}
	if !hash.Empty() {
		t.Errorf("Hash not empty after removals, size: %d", hash.Size())
	}
}

func TestHeavyLoadAndResize(t *testing.T) {
	// Маленький размер для провокации resize
	hash := NewCuckooHash[int](2)
	n := 50 // Достаточно, чтобы вызвать несколько ресайзов

	for i := 0; i < n; i++ {
		hash.Insert(fmt.Sprintf("key%d", i), i)
	}

	if int(hash.Size()) != n {
		t.Errorf("Expected size %d, got %d", n, hash.Size())
	}

	for i := 0; i < n; i++ {
		key := fmt.Sprintf("key%d", i)
		val := hash.Find(key)
		if val == nil {
			t.Fatalf("Key not found: %s", key)
		}
		if *val != i {
			t.Errorf("Value mismatch for %s: expected %d, got %d", key, i, *val)
		}
	}
}

// TestCycleForce проверяет ситуацию цикла и ресайза
func TestCycleForce(t *testing.T) {
	// Создаем маленькую таблицу
	hash := NewCuckooHash[int](2)
	// Вставка 5 элементов в таблицу размером 2 гарантированно вызовет resize
	hash.Insert("a", 1)
	hash.Insert("b", 2)
	hash.Insert("c", 3)
	hash.Insert("d", 4)
	hash.Insert("e", 5)

	if hash.Size() != 5 {
		t.Errorf("Size mismatch: %d", hash.Size())
	}
}

// Specific Logic Coverage

// TestMagicFindLogic
func TestMagicFindLogic(t *testing.T) {
	hash := NewCuckooHash[int](120)

	// Находим ключ, дающий hash1 == 99
	var magicKey string
	found := false
	for i := 0; i < 50000; i++ {
		k := fmt.Sprintf("magic_%d", i)
		if hash.hash1(k) == 99 {
			magicKey = k
			found = true
			break
		}
	}

	if !found {
		t.Skip("Could not find a key with hash1 == 99, skipping magic logic test")
	}

	// "Закладка" работает, если table[49] занят и ключи совпадают.
	// Поскольку мы в том же пакете, можем модифицировать table напрямую.
	// Но сначала убедимся, что table[49] существует
	if len(hash.table) <= 49 {
		t.Fatal("Table too small for magic index test")
	}

	// Имитируем ситуацию, когда элемент лежит по индексу 49,
	// хотя его хеш указывает на 99.
	hash.table[49] = HashNode[int]{
		Key:        magicKey,
		Value:      777,
		IsOccupied: true,
	}

	val := hash.Find(magicKey)
	if val == nil {
		t.Fatal("Magic logic failed: key not found at index 49 when h1=99")
	}
	if *val != 777 {
		t.Errorf("Magic logic value mismatch: %d", *val)
	}
}

// TestHash2EvenLogic
func TestHash2EvenLogic(t *testing.T) {
	// Создаем таблицу с ЧЕТНЫМ размером
	hash := NewCuckooHash[int](100)

	// Нам нужен ключ, у которого (sum % (tableSize - 1)) + 1 будет четным.
	// Пусть sum = 100. 100 % 99 = +1
	// Ключ "d" (код 100).

	key := "d"
	h2 := hash.hash2(key)

	// Должен сработать инкремент: result =

	if h2 != 3 {
		t.Errorf("Expected hash2 to be odd (incremented) for even table size. Got %d", h2)
	}
}

// Copy Semantics

func TestCopySemantics(t *testing.T) {
	original := NewCuckooHash[int](3)
	original.Insert("a", 1)
	original.Insert("b", 2)

	copyHash := original.Copy()

	if copyHash.Size() != 2 {
		t.Errorf("Copy size mismatch: %d", copyHash.Size())
	}
	if *copyHash.Find("a") != 1 {
		t.Error("Copy content mismatch")
	}

	// Проверка глубокого копирования
	copyHash.Insert("a", 999)
	copyHash.Remove("b")

	if *original.Find("a") != 1 {
		t.Error("Original modified after changing copy (Deep Copy failed)")
	}
	if original.Find("b") == nil {
		t.Error("Element removed from original after removing from copy")
	}
}

// Serialization

const TEXT_FILE = "test_db.txt"
const BIN_FILE = "test_db.bin"

func TestTextSerialization(t *testing.T) {
	defer os.Remove(TEXT_FILE)

	{
		hash := NewCuckooHash[float64](3)
		hash.Insert("pi", 3.14159)
		hash.Insert("e", 2.71828)
		if err := hash.SerializeText(TEXT_FILE); err != nil {
			t.Fatalf("SerializeText failed: %v", err)
		}
	}

	loadedHash := NewCuckooHash[float64](3)
	if err := loadedHash.DeserializeText(TEXT_FILE); err != nil {
		t.Fatalf("DeserializeText failed: %v", err)
	}

	if loadedHash.Size() != 2 {
		t.Errorf("Expected size 2, got %d", loadedHash.Size())
	}

	val := loadedHash.Find("pi")
	if val == nil {
		t.Fatal("pi not found")
	}
	if math.Abs(*val-3.14159) > 0.0001 {
		t.Errorf("Value mismatch for pi")
	}
}

func TestTextSerializationErrors(t *testing.T) {
	defer os.Remove(TEXT_FILE)

	// Missing file
	h := NewCuckooHash[int](1)
	if err := h.DeserializeText("missing_file.txt"); err == nil {
		t.Error("Expected error for missing file")
	}

	// Bad header (not numbers)
	os.WriteFile(TEXT_FILE, []byte("BAD HEADER\n"), 0644)
	if err := h.DeserializeText(TEXT_FILE); err == nil {
		t.Error("Expected error for bad header")
	}

	// Index out of bounds
	// Header says size 5, but index is 10
	os.WriteFile(TEXT_FILE, []byte("5 1\n10 key 123\n"), 0644)
	if err := h.DeserializeText(TEXT_FILE); err == nil {
		t.Error("Expected error for index out of bounds")
	}

	// Write error (invalid path, e.g. directory)
	// This depends on OS, skipping usually or using / invalid path
	if err := h.SerializeText(""); err == nil {
		t.Error("Expected error for empty filename")
	}
}

func TestBinarySerialization(t *testing.T) {
	defer os.Remove(BIN_FILE)

	{
		hash := NewCuckooHash[int32](3)
		for i := int32(0); i < 10; i++ {
			hash.Insert(fmt.Sprintf("%d", i), i*10)
		}
		if err := hash.SerializeBin(BIN_FILE); err != nil {
			t.Fatalf("SerializeBin failed: %v", err)
		}
	}

	loadedHash := NewCuckooHash[int32](3)
	if err := loadedHash.DeserializeBin(BIN_FILE); err != nil {
		t.Fatalf("DeserializeBin failed: %v", err)
	}

	if loadedHash.Size() != 10 {
		t.Errorf("Expected size 10, got %d", loadedHash.Size())
	}
	val := loadedHash.Find("5")
	if val == nil || *val != 50 {
		t.Errorf("Value mismatch for key 5")
	}
}

func TestBinarySerializationCorrupt(t *testing.T) {
	defer os.Remove(BIN_FILE)
	h := NewCuckooHash[int](3)

	// Missing file
	if err := h.DeserializeBin("missing.bin"); err == nil {
		t.Error("Expected error for missing bin file")
	}

	// Corrupt Table Size (Empty file)
	os.WriteFile(BIN_FILE, []byte{}, 0644)
	if err := h.DeserializeBin(BIN_FILE); err == nil {
		t.Error("Expected error for empty file (reading size)")
	}

	// Corrupt Elements Count (File has size, misses count)
	buf := new(bytes.Buffer)
	binary.Write(buf, binary.LittleEndian, uint32(5)) // tableSize
	// no count
	os.WriteFile(BIN_FILE, buf.Bytes(), 0644)
	if err := h.DeserializeBin(BIN_FILE); err == nil {
		t.Error("Expected error for missing elements count")
	}

	// Corrupt Occupied Flag (truncated loop)
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(1)) // tableSize
	binary.Write(buf, binary.LittleEndian, uint32(1)) // count
	// no occupied flag data
	os.WriteFile(BIN_FILE, buf.Bytes(), 0644)
	if err := h.DeserializeBin(BIN_FILE); err == nil {
		t.Error("Expected error reading occupied flag")
	}

	// Corrupt Key Length
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(1)) // size
	binary.Write(buf, binary.LittleEndian, uint32(1)) // count
	binary.Write(buf, binary.LittleEndian, true)      // occupied
	// no key len
	os.WriteFile(BIN_FILE, buf.Bytes(), 0644)
	if err := h.DeserializeBin(BIN_FILE); err == nil {
		t.Error("Expected error reading key len")
	}

	// Corrupt Key Data (len is 5, but EOF)
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(1))
	binary.Write(buf, binary.LittleEndian, uint32(1))
	binary.Write(buf, binary.LittleEndian, true)
	binary.Write(buf, binary.LittleEndian, uint32(100))     // key len big
	binary.Write(buf, binary.LittleEndian, []byte("short")) // key data short
	os.WriteFile(BIN_FILE, buf.Bytes(), 0644)
	if err := h.DeserializeBin(BIN_FILE); err == nil {
		t.Error("Expected error reading key buffer")
	}

	// Corrupt Value
	buf.Reset()
	binary.Write(buf, binary.LittleEndian, uint32(1))
	binary.Write(buf, binary.LittleEndian, uint32(1))
	binary.Write(buf, binary.LittleEndian, true)
	key := []byte("k")
	binary.Write(buf, binary.LittleEndian, uint32(len(key)))
	buf.Write(key)
	// no value (int is 4 or 8 bytes, write nothing)
	os.WriteFile(BIN_FILE, buf.Bytes(), 0644)
	if err := h.DeserializeBin(BIN_FILE); err == nil {
		t.Error("Expected error reading value")
	}

	// Write Error test
	if err := h.SerializeBin(""); err == nil {
		t.Error("Expected error writing to empty filename")
	}
}

// Print

func TestPrintMethod(t *testing.T) {
	hash := NewCuckooHash[int](3)
	hash.Insert("test1", 1)
	hash.Insert("test2", 2)

	output := captureOutput(func() {
		hash.Print()
	})

	if !strings.Contains(output, "test1 => 1") {
		t.Error("Output missing test1")
	}
	if !strings.Contains(output, "test2 => 2") {
		t.Error("Output missing test2")
	}
	if !strings.Contains(output, "Cuckoo Хэш-таблица") {
		t.Error("Missing header")
	}
}
