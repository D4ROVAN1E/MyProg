# Компилятор и флаги компиляции
CXX = g++-11
CXXFLAGS = -Wall -Wextra -std=c++23 -pthread

# Список исходных файлов.
SOURCES = task1.cpp task2.cpp task3.cpp 

# Генерируем имена исполняемых файлов (убираем .cpp из названий)
EXECS = $(SOURCES:.cpp=)

# Цель по умолчанию
all: build

# Сначала выполняется цель build, затем цикл запуска
run: build
	@for exe in $(EXECS); do \
		echo "--------------------------------"; \
		echo "Запуск $$exe ..."; \
		./$$exe; \
	done
	@echo "--------------------------------"
	@echo "Все задачи выполнены."


build: $(EXECS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< 


clean:
	rm -f $(EXECS)

# Объявляем псевдоцели
.PHONY: all run build clean