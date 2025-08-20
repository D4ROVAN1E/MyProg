// Функция для проверки, является ли строка целым числом
function isInteger(str: string): boolean {
  if (typeof str !== 'string') {
    return false;
  }
  if (str.trim() === '') {
    return false;
  }
  for (let i = 0; i < str.length; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return false;
    }
  }
  return true;
}

// Функция для проверки корректности даты
function isValidDate(day: number, month: number): boolean {
  return day >= 1 && day <= 31 && month >= 1 && month <= 12;
}

// Функция для обработки входной строки
function processInput(line: string): string {
  // Разбиваем введенную строку на отдельные числа
  const tokens = line.split(/\s+/);
  
  // Первое число - количество последующих чисел
  const count = parseInt(tokens[0], 10);
  if (isNaN(count)) {
    return 'Invalid input';
  }

  let result = '';

  // Обрабатываем каждое число, начиная со второго
  for (let i = 1; i <= count && i < tokens.length; i++) {
    const token = tokens[i];
    const number = parseInt(token, 10);

    if (isNaN(number)) {
      result += 'NO';
      continue;
    }

    let isCorrectDate = false;

    if (number >= 11 && number < 100) {
      const day = Math.floor(number / 10);
      const month = number % 10;
      isCorrectDate = isValidDate(day, month);
    } else if (number >= 100 && number < 1000) {
      let day = Math.floor(number / 10);
      let month = number % 10;
      isCorrectDate = isValidDate(day, month);
      
      if (!isCorrectDate) {
        day = Math.floor(number / 100);
        month = number % 100;
        isCorrectDate = isValidDate(day, month);
      }
    } else if (number >= 1000 && number <= 3112) {
      const day = Math.floor(number / 100);
      const month = number % 100;
      isCorrectDate = isValidDate(day, month);
    }

    result += isCorrectDate ? 'YES' : 'NO';
  }

  return result;
}

// Функция для получения ввода от пользователя
function getUserInput(): void {
    const input = prompt("Введите количество чисел и сами числа через пробел:");
    
    if (input === null) {
        console.log("Ввод был отменен пользователем");
        return;
    }

    const result = processInput(input);
    console.log(`Результат: ${result}`);
}

// Запускаем программу
getUserInput();