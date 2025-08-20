// Определение структуры для хранения информации о цветах на стержне
struct Rod {
    var hasR = false 
    var hasB = false 
    var hasW = false 
}

// Функция для подсчета стержней с кольцами всех трех цветов
func countPoints(_ rings: String) -> Int {
    // Инициализация массива из 10 стержней
    var rods = Array(repeating: Rod(), count: 10)
    let characters = Array(rings) // Преобразование строки в массив символов
    let length = characters.count
    
    // Обработка пар символов 
    for i in stride(from: 0, to: length, by: 2) {
        // Проверка на выход за пределы массива (нечетная длина строки)
        guard i + 1 < length else { break }
        
        let color = characters[i]    
        let positionChar = characters[i + 1] 
        
        // Проверка, что позиция является цифрой
        guard positionChar.isNumber else { continue }
        let position = Int(String(positionChar))! // Преобразование в Int
        
        // Проверка допустимости позиции (0...9)
        guard position >= 0 && position < 10 else { continue }
        
        // Обновление флагов стержня в зависимости от цвета
        switch color {
        case "R": rods[position].hasR = true
        case "B": rods[position].hasB = true
        case "W": rods[position].hasW = true
        default: continue // Игнорирование недопустимых цветов
        }
    }
    
    // Подсчет стержней, где есть все три цвета
    return rods.filter { $0.hasR && $0.hasB && $0.hasW }.count
}

// Основная программа
print("Input: ")
if let input = readLine() { // Чтение входной строки
    print(countPoints(input)) // Вывод результата
}