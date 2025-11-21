# Имена исходных файлов тестов
TEST_SRCS = test_array.cpp \
            test_binary_tree.cpp \
            test_ch.cpp \
            test_dh.cpp \
            test_doubly_list.cpp \
            test_queue.cpp \
            test_singly_list.cpp \
            test_stack.cpp

# Генерация имен исполняемых файлов (test_name.cpp -> t_name)
EXECUTABLES = $(patsubst test_%.cpp, t_%, $(TEST_SRCS))

# Компилятор и флаги
CXX = g++
CXXFLAGS = --coverage

# Основная цель (по умолчанию)
all: report

# Компиляция: правило для создания t_name из test_name.cpp
# $@ - имя цели (например, t_array)
# $< - имя зависимости (например, test_array.cpp)
t_%: test_%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -I "/mnt/c/local/boost_1_89_0"

# Цель для компиляции всех файлов
compile: $(EXECUTABLES)

# Запуск тестов 
run_tests: compile
	@echo "--- Running Tests ---"
	@for exe in $(EXECUTABLES); do \
		echo "Running ./$$exe..."; \
		./$$exe; \
	done

# Сбор покрытия, фильтрация и генерация HTML
report: run_tests
	lcov --capture --directory . --output-file coverage.info

	lcov --remove coverage.info \
		'/mnt/c/msys64*' \
		'/mnt/c/local/*' \
		'/usr/*' \
		'*/test_*.cpp' \
		--output-file coverage_filtered.info

	genhtml coverage_filtered.info --output-directory html_report

# Очистка сгенерированных файлов
clean:
	rm -f $(EXECUTABLES)
	rm -f *.gcda *.gcno *.info
	rm -rf html_report

.PHONY: all compile run_tests report clean
