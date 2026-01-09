# ТЕСТЫ
TEST_SRCS = test_array.cpp \
            test_binary_tree.cpp \
            test_ch.cpp \
            test_dh.cpp \
            test_doubly_list.cpp \
            test_queue.cpp \
            test_singly_list.cpp \
            test_stack.cpp

# Исполняемые файлы тестов (test_name.cpp -> t_name)
TEST_EXES = $(patsubst test_%.cpp, t_%, $(TEST_SRCS))

# БЕНЧМАРКИ
BENCH_SRCS = bench_array.cpp \
             bench_biTree.cpp \
             bench_ch.cpp \
             bench_dh.cpp \
             bench_dl.cpp \
             bench_queue.cpp \
             bench_sl.cpp \
             bench_stack.cpp

# Исполняемые файлы бенчмарков (bench_name.cpp -> b_name)
BENCH_EXES = $(patsubst bench_%.cpp, b_%, $(BENCH_SRCS))

# НАСТРОЙКИ КОМПИЛЯТОРА
CXX = g++
# Путь к заголовкам Boost
BOOST_INC = -I "/mnt/c/local/boost_1_89_0"
# Библиотеки Boost, необходимые для таймеров (только для бенчмарков)
BOOST_LIBS = -lboost_timer -lboost_system -lboost_random

# Флаги:
# Для тестов нужно покрытие кода (--coverage)
TEST_FLAGS = --coverage
# Для бенчмарков критически важна оптимизация
BENCH_FLAGS = -O3

# ЦЕЛИ

# Основная цель (по умолчанию запускает тесты и отчет)
all: report

# Сборка тестов
t_%: test_%.cpp
	$(CXX) $(TEST_FLAGS) $< -o $@ $(BOOST_INC)

# Сборка бенчмарков
b_%: bench_%.cpp
	$(CXX) $(BENCH_FLAGS) $< -o $@ $(BOOST_INC) $(BOOST_LIBS)

# Компиляция всего (и тесты, и бенчмарки)
compile: $(TEST_EXES) $(BENCH_EXES)

# Запуск тестов
run_tests: $(TEST_EXES)
	@echo "Running Unit Tests"
	@for exe in $(TEST_EXES); do \
		echo "Testing ./$$exe..."; \
		./$$exe; \
	done

# Запуск бенчмарков
bench: $(BENCH_EXES)
	@echo "Running Benchmarks"
	@for exe in $(BENCH_EXES); do \
		echo "========================================"; \
		echo "Benchmarking ./$$exe..."; \
		./$$exe; \
		echo ""; \
	done

# Генерация отчета о покрытии (только для тестов)
report: run_tests
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info \
		'/mnt/c/msys64*' \
		'/mnt/c/local/*' \
		'/usr/*' \
		'*/test_*.cpp' \
		'*/bench_*.cpp' \
		--output-file coverage_filtered.info
	genhtml coverage_filtered.info --output-directory html_report
	@echo "Report generated in html_report/index.html"

# Очистка
clean:
	rm -f $(TEST_EXES) $(BENCH_EXES)
	rm -f *.gcda *.gcno *.info
	rm -rf html_report

.PHONY: all compile run_tests bench report clean