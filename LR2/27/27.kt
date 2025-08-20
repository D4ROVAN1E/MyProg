fun main() {
    // Читаем входные данные и разбиваем их на отдельные токены по пробелам
    val input = readLine()?.split(" ") ?: emptyList()
    
    // Первый элемент - количество чисел
    val n = input.firstOrNull()?.toIntOrNull() ?: 0
    
    // Обрабатываем каждое число, начиная со второго элемента
    input.drop(1).take(n).forEach { token ->
        processToken(token)
    }
}

fun processToken(token: String) {
    // Проверяем, что токен состоит только из цифр
    if (!token.matches(Regex("^\\d+$"))) {
        println("Invalid input")
        return
    }

    val number = token.toInt()
    var isCorrectDate = false

    when {
        number in 11..99 -> {
            val day = number / 10
            val month = number % 10
            isCorrectDate = correctDate(day, month)
        }
        number in 100..999 -> {
            // Проверяем два варианта разделения
            val day1 = number / 10
            val month1 = number % 10
            isCorrectDate = correctDate(day1, month1)
            
            if (!isCorrectDate) {
                val day2 = number / 100
                val month2 = number % 100
                isCorrectDate = correctDate(day2, month2)
            }
        }
        number in 1000..3112 -> {
            val day = number / 100
            val month = number % 100
            isCorrectDate = correctDate(day, month)
        }
    }

    println(if (isCorrectDate) "YES" else "NO")
}

fun correctDate(day: Int, month: Int): Boolean {
    return day in 1..31 && month in 1..12
}