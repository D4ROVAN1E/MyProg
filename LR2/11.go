package main

import (
	"fmt"
	"strings"
)

func main() {
	var N, M int
	var S string

	// Чтение входных данных
	_, err := fmt.Scanf("%d %d %s", &N, &M, &S)
	if err != nil {
		fmt.Println("Invalid input")
		return
	}

	// Проверка корректности размеров доски
	if N < 1 || M < 1 {
		fmt.Println("Invalid input")
		return
	}

	// Проверка допустимости символов в строке команд
	for _, c := range S {
		if !strings.ContainsRune("LRUD", c) {
			fmt.Println("Invalid input")
			return
		}
	}

	// Инициализация минимальных координат и счётчиков команд
	minX, minY := 1, 1
	L, R, U, D := 0, 0, 0, 0

	// Обработка команд с динамическим обновлением минимальных координат
	for _, c := range S {
		switch c {
		case 'L':
			L++
			if minX-L < 1 {
				minX++
			}
			if R > 0 {
				R--
			}
		case 'R':
			R++
			if L > 0 {
				L--
			}
		case 'U':
			U++
			if minY-U < 1 {
				minY++
			}
			if D > 0 {
				D--
			}
		case 'D':
			D++
			if U > 0 {
				U--
			}
		}
	}

	// Проверка существования допустимой начальной позиции
	if minX > M || minY > N {
		fmt.Println("No")
	} else {
		fmt.Printf("(%d,%d)\n", minX, minY)
	}
}

// Вспомогательные функции для вычисления минимума и максимума
func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
