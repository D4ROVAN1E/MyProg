function findStartPosition(N: number, M: number, S: string): string {
    // 1. Проверяем, что N и M являются числами и больше или равны 1.
    if (typeof N !== 'number' || typeof M !== 'number' || isNaN(N) || isNaN(M) || N < 1 || M < 1) {
      return "Ошибка: Размеры доски N и M должны быть числами, большими или равными 1.";
    }
  
    // 2. Проверяем, что S является строкой.
    if (typeof S !== 'string') {
      return "Ошибка: Последовательность ходов S должна быть строкой.";
    }
  
    // 3. Проверяем, что строка S содержит только допустимые символы ('L', 'R', 'U', 'D').
    if (!/^[LRUD]*$/.test(S)) {
      return "Ошибка: Строка команд S может содержать только символы 'L', 'R', 'U', 'D'.";
    }

    let min_x = 1, min_y = 1;
    let L = 0, R = 0, U = 0, D = 0;

    for (const c of S) {
        switch(c) {
            case 'L':
                L++;
                if (min_x - L < 1) min_x++;
                if (R > 0) R--;
                break;
            case 'R':
                R++;
                if (L > 0) L--;
                break;
            case 'U':
                U++;
                if (min_y - U < 1) min_y++;
                if (D > 0) D--;
                break;
            case 'D':
                D++;
                if (U > 0) U--;
                break;
        }
    }

    if (min_x > M || min_y > N) {
        return "No";
    } else {
        return `(${min_x},${min_y})`;
    }
}

// Функция для получения ввода от пользователя
function getUserInput(): void {
    const N = Number(prompt("Введите высоту доски (N):"));
    const M = Number(prompt("Введите ширину доски (M):"));
    const S = prompt("Введите последовательность ходов (только символы L, R, U, D):");

    if (N === null || M === null || S === null) {
        console.log("Ввод был отменен пользователем");
        return;
    }

    const result = findStartPosition(N, M, S);
    console.log(`Результат для N=${N}, M=${M}, S="${S}": ${result}`);
}

// Запускаем программу
getUserInput();