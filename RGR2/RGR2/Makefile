# Имя компилятора
CXX = g++

# Флаги компилятора
CXXFLAGS = -Wall -std=c++17

# Флаги для создания .so
SO_FLAGS = -fPIC -shared

# Флаги компоновщика
LDFLAGS = -L. -Wl,-rpath,.
LIBS = -lbifid -laffine -lrailway

# Цель по умолчанию
all: RGR2

# Правило для создания исполняемого файла RGR2
RGR2: RGR2.o libbifid.so libaffine.so librailway.so
	$(CXX) $(CXXFLAGS) -o $@ RGR2.o $(LDFLAGS) $(LIBS)

# Правила для создания библиотек
libbifid.so: Bifid.cpp Bifid.h
	$(CXX) $(SO_FLAGS) -o $@ Bifid.cpp

libaffine.so: Affine.cpp Affine.h
	$(CXX) $(SO_FLAGS) -o $@ Affine.cpp

librailway.so: Railway_fence.cpp Railway_fence.h
	$(CXX) $(SO_FLAGS) -o $@ Railway_fence.cpp

# Правило для компиляции объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Цель для очистки
clean:
	rm -f *.o *.so RGR2
