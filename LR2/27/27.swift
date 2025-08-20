import Foundation // Импорт Foundation для работы с функциями ввода-вывода

func correctDate(day: Int, month: Int) -> Bool {
    return day >= 1 && day <= 31 && month >= 1 && month <= 12
}

func main() {
    // Чтение количества чисел
    guard let inputLine = readLine() else { return }
    let tokens = inputLine.components(separatedBy: .whitespaces)
    
    // Первый токен - количество чисел
    guard let count = Int(tokens[0]) else { return }
    
    // Обработка каждого числа
    for i in 1...count {
        guard i < tokens.count else { break }
        let token = tokens[i]
        
        // Проверка на корректность: все символы — цифры
        guard token.allSatisfy({ $0.isNumber }) else {
            print("NO", terminator: "")
            continue
        }
        
        var isCorrectDate = false
        
        if let number = Int(token) {
            if number >= 11 && number < 100 {
                let day = number / 10
                let month = number % 10
                isCorrectDate = correctDate(day: day, month: month)
            } else if number >= 100 && number < 1000 {
                var day = number / 10
                var month = number % 10
                isCorrectDate = correctDate(day: day, month: month)
                
                if !isCorrectDate {
                    day = number / 100
                    month = number % 100
                    isCorrectDate = correctDate(day: day, month: month)
                }
            } else if number >= 1000 && number <= 3112 {
                let day = number / 100
                let month = number % 100
                isCorrectDate = correctDate(day: day, month: month)
            }
        }
        
        print(isCorrectDate ? "YES" : "NO", terminator: "")
    }
}

main() // Запуск основной функции