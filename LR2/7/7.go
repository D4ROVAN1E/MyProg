package main

import (
    "fmt"
)

// Rod представляет стержень с наличием колец разных цветов
type Rod struct {
    hasR bool // Наличие красного кольца
    hasB bool // Наличие синего кольца
    hasW bool // Наличие белого кольца
}

// countPoints вычисляет количество стержней с кольцами всех трех цветов
func countPoints(rings string) int {
    rods := make([]Rod, 10) // Инициализация 10 стержней (0-9)

    // Обработка каждой пары символов (цвет-позиция)
    for i := 0; i < len(rings); i += 2 {
        if i+1 >= len(rings) { // Проверка на нечетную длину строки
            break
        }

        color := rings[i]        // Цвет кольца
        posChar := rings[i+1]    // Символ позиции

        // Пропуск недопустимых позиций (не цифра)
        if posChar < '0' || posChar > '9' {
            continue
        }
        pos := int(posChar - '0') // Преобразование в числовую позицию

        // Обновление флагов стержня в зависимости от цвета
        switch color {
        case 'R':
            rods[pos].hasR = true
        case 'B':
            rods[pos].hasB = true
        case 'W':
            rods[pos].hasW = true
        default:
            // Пропуск недопустимых цветов
            continue
        }
    }

    // Подсчет стержней с кольцами всех цветов
    count := 0
    for _, rod := range rods {
        if rod.hasR && rod.hasB && rod.hasW {
            count++
        }
    }
    return count
}

func main() {
    var input string
    fmt.Println("Input:")
    fmt.Scanln(&input)           // Чтение входной строки
    fmt.Println(countPoints(input)) // Вывод результата
}