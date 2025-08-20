// Функция для подсчета стержней с кольцами всех трех цветов
fun countPoints(rings: String): Int {
    // Класс, представляющий стержень с флагами наличия цветов
    class Rod {
        var hasR = false
        var hasB = false
        var hasW = false
    }

    // Инициализируем массив из 10 стержней
    val rods = Array(10) { Rod() }

    // Проходим по входной строке шагом 2 (цвет и позиция)
    for (i in 0 until rings.length step 2) {
        // Проверка на выход за границы строки для пары
        if (i + 1 >= rings.length) break

        val color = rings[i]
        val posChar = rings[i + 1]

        // Проверка, что позиция - это цифра от 0 до 9
        if (!posChar.isDigit()) continue
        val pos = posChar.toString().toInt()
        if (pos !in 0..9) continue

        // Обновляем флаги стержня в зависимости от цвета
        when (color) {
            'R' -> rods[pos].hasR = true
            'B' -> rods[pos].hasB = true
            'W' -> rods[pos].hasW = true
            // Игнорируем недопустимые цвета
            else -> continue
        }
    }

    // Подсчитываем стержни с тремя цветами
    return rods.count { it.hasR && it.hasB && it.hasW }
}

fun main() {
    println("Input:")
    val input = readLine() ?: "" // Чтение входной строки
    println(countPoints(input))
}