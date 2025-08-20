fun main() {
    // Чтение входной строки и разделение на части
    val input = readlnOrNull()?.split(" ") ?: listOf()
    // Проверка на наличие трёх элементов (N, M, S)
    if (input.size != 3) {
        println("Invalid input")
        return
    }

    // Парсинг N и M с проверкой на целые числа
    val n = input[0].toIntOrNull()
    val m = input[1].toIntOrNull()
    val s = input[2]

    // Проверка корректности N и M (должны быть ≥ 1) и непустой строки S
    if (n == null || m == null || n < 1 || m < 1 || s == "") {
        println("Invalid input")
        return
    }

    // Проверка символов в строке S (только L, R, U, D)
    for (c in s) {
        if (c !in setOf('L', 'R', 'U', 'D')) {
            println("Invalid input")
            return
        }
    }

    var minX = 1
    var minY = 1
    var l = 0
    var r = 0
    var u = 0
    var d = 0

    // Обработка команд с отслеживанием минимальных координат
    for (c in s) {
        when (c) {
            'L' -> {
                l++
                if (minX - l < 1) minX++
                if (r > 0) r--
            }
            'R' -> {
                r++
                if (l > 0) l--
            }
            'U' -> {
                u++
                if (minY - u < 1) minY++
                if (d > 0) d--
            }
            'D' -> {
                d++
                if (u > 0) u--
            }
        }
    }

    // Проверка существования допустимой начальной позиции
    if (minX > m || minY > n) {
        println("No")
    } else {
        // Вывод минимальных координат из допустимого диапазона
        println("($minX,$minY)")
    }
}