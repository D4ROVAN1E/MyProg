# Компилятор
CXX = g++
# Флаги компиляции: 
CXXFLAGS = -std=c++17 -Wall
# Флаги линковщика (подключение библиотек PostgreSQL и pqxx)
LDFLAGS = -lpqxx -lpq

# Имя исходного файла и итогового приложения
SRC = library_system.cpp
TARGET = library_app

# Цель по умолчанию
# Порядок: Проверка -> Сборка -> Запуск
all: check build run

# 1. Проверка наличия библиотек
# Использует dpkg для проверки установки пакетов в Ubuntu/Debian
check:
	@echo "[CHECK] Проверка зависимостей..."
	@dpkg -s libpqxx-dev > /dev/null 2>&1 || (echo "\033[0;31m[ERROR] Не найден пакет libpqxx-dev!\033[0m Установите его командой: sudo apt install libpqxx-dev"; exit 1)
	@dpkg -s libpq-dev > /dev/null 2>&1 || (echo "\033[0;31m[ERROR] Не найден пакет libpq-dev!\033[0m Установите его командой: sudo apt install libpq-dev"; exit 1)
	@echo "\033[0;32m[OK] Библиотеки найдены.\033[0m"

# 2. Компиляция
build: $(TARGET)

$(TARGET): $(SRC)
	@echo "[BUILD] Компиляция $(SRC)..."
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	@echo "\033[0;32m[OK] Программа успешно скомпилирована в файл './$(TARGET)'\033[0m"

# 3. Запуск
run:
	@echo "[RUN] Запуск программы..."
	@echo "---------------------------------------------------"
	@./$(TARGET)
	@echo "---------------------------------------------------"

# Очистка (удаление скомпилированного файла)
clean:
	@echo "[CLEAN] Удаление файла $(TARGET)..."
	@rm -f $(TARGET)
	@echo "[OK] Очистка выполнена."

.PHONY: all check build run clean