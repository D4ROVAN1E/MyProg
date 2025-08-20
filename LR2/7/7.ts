// Функция countPoints принимает строку rings и возвращает число стержней,
// на которых присутствуют кольца всех трёх цветов: красного (R), синего (B) и белого (W).
function countPoints(rings: string): number {
    // Определяем тип Rod (стержень) с булевыми флагами для каждого цвета.
    type Rod = {
        hasR: boolean; 
        hasB: boolean; 
        hasW: boolean; 
    };

    // Создаём массив rods длины 10, где каждый элемент инициализируется объектом Rod,
    // у которого изначально все флаги установлены в false.
    let rods: Rod[] = Array(10)
        .fill(null)
        .map(() => ({ hasR: false, hasB: false, hasW: false }));

    // Обрабатываем строку rings, проходя по ней с шагом 2, так как каждый
    // символ цвета сопровождается символом позиции.
    for (let i = 0; i < rings.length; i += 2) {
        // Проверка защиты от дурака: если строка имеет нечётную длину, последний символ будет пропущен.
        if (i + 1 >= rings.length) break;

        // Извлекаем символ цвета и символ позиции
        const color = rings[i];
        const posChar = rings[i + 1];

        // Защита от дурака: проверяем, что posChar является корректным числовым символом (от '0' до '9').
        if (posChar < '0' || posChar > '9') {
            continue; 
        }

        // Преобразуем символьное представление позиции в число.
        const pos = parseInt(posChar);

        // Выбираем действие по типу цвета с помощью конструкции switch.
        // Обновляем соответствующий флаг в объекте rods[pos].
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

    // Переменная для подсчёта стержней, на которых присутствуют все три цвета.
    let count = 0;
    rods.forEach(rod => {
        if (rod.hasR && rod.hasB && rod.hasW) {
            count++;
        }
    });

    return count;
}

// Функция для получения ввода от пользователя
function getUserInput(): void {
    const input = prompt("Введите строку с кольцами:");
    
    if (input === null) {
        console.log("Ввод был отменен пользователем");
        return;
    }

    const result = countPoints(input);
    console.log(`Результат для строки "${input}": ${result}`);
}

// Запускаем программу
getUserInput();
