import Foundation

// Чтение входных данных
guard let inputLine = readLine() else {
    print("Invalid input")
    exit(0)
}

// Разделение введенной строки на компоненты: N, M, S
let parts = inputLine.split(separator: " ", maxSplits: 2, omittingEmptySubsequences: true)
guard parts.count == 3 else {
    print("Invalid input")
    exit(0)
}

// Проверка корректности чисел N и M
guard let n = Int(parts[0]), let m = Int(parts[1]), n >= 1, m >= 1 else {
    print("Invalid input")
    exit(0)
}

let s = String(parts[2])

// Проверка допустимости символов в строке S
for char in s {
    guard ["L", "R", "U", "D"].contains(String(char)) else {
        print("Invalid input")
        exit(0)
    }
}

// Инициализация счетчиков и минимальных координат
var (l, r, u, d) = (0, 0, 0, 0)
var minX = 1
var minY = 1

// Обработка команд с динамическим отслеживанием минимальных координат
for char in s {
    switch char {
    case "L":
        l += 1
        if minX - l < 1 {
            minX += 1
        }
        if r > 0 {
            r -= 1
        }
    case "R":
        r += 1
        if l > 0 {
            l -= 1
        }
    case "U":
        u += 1
        if minY - u < 1 {
            minY += 1
        }
        if d > 0 {
            d -= 1
        }
    case "D":
        d += 1
        if u > 0 {
            u -= 1
        }
    default:
        break
    }
}

// Проверка существования допустимой позиции и вывод результата
if minX > m || minY > n {
    print("No")
} else {
    print("(\(minX),\(minY))")
}