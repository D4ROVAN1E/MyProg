// Функция countPoints принимает строку rings, описывающую кольца, надетые на стержни
function countPoints(rings) {
    // Проверка входного параметра: если rings не является строкой, выбрасываем ошибку.
    if (typeof rings !== 'string') {
        throw new Error("Input must be a string");
    }

    // Создаем массив из 10 объектов, соответствующих 10 стержням, от 0 до 9.
    // Каждый объект хранит булевы флаги: hasR, hasB, hasW – по умолчанию все false.
    const rods = Array.from({ length: 10 }, () => ({ hasR: false, hasB: false, hasW: false }));

    // Проходим по строке rings с шагом 2, так как каждая пара символов описывает одно кольцо.
    for (let i = 0; i < rings.length; i += 2) {
        // Если остался один символ (нечетная длина строки), прерываем цикл.
        if (i + 1 >= rings.length) break;

        // Первый символ пары обозначает цвет кольца.
        const color = rings[i];
        // Второй символ пары – позиция стержня, на который надето кольцо.
        const posChar = rings[i + 1];

        // Проверяем, что позиция является допустимым символом от '0' до '9'.
        // Если символ выходит за пределы допустимого диапазона, пропускаем текущую итерацию.
        if (posChar < '0' || posChar > '9') continue;

        // Преобразуем символ в число: '0' становится 0, '9' становится 9 и т.д.
        const pos = parseInt(posChar, 10);

        // В зависимости от цвета кольца обновляем соответствующий флаг в объекте стержня.
        // Если цвет не соответствует 'R', 'B' или 'W', то он игнорируется.
        switch (color) {
            case 'R':
                rods[pos].hasR = true;
                break;
            case 'B':
                rods[pos].hasB = true;
                break;
            case 'W':
                rods[pos].hasW = true;
                break;
            default:
                break;
        }
    }

    // Счетчик стержней, на которых есть кольца всех трех цветов.
    let count = 0;
    // Перебираем все стержни и проверяем: если все флаги (hasR, hasB, hasW) установлены, увеличиваем count.
    rods.forEach(rod => {
        if (rod.hasR && rod.hasB && rod.hasW) {
            count++;
        }
    });

    // Возвращаем итоговое количество стержней, удовлетворяющих условию.
    return count;
}

// Функция для ввода данных от пользователя
function getUserInput() {
    const readline = require('readline').createInterface({
        input: process.stdin,
        output: process.stdout
    });

    return new Promise((resolve) => {
        readline.question('Введите последовательность колец: ', (input) => {
            readline.close();
            resolve(input);
        });
    });
}

// Основная функция для запуска программы
async function main() {
    try {
        const input = await getUserInput();
        const result = countPoints(input);
        console.log(`Количество стержней с кольцами всех трех цветов: ${result}`);
    } catch (error) {
        console.error('Произошла ошибка:', error.message);
    }
}

// Запуск программы
main();
