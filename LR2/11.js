function findStartPosition(N, M, S) {
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

  // 4. Инициализируем минимальные координаты и счетчики движений
  let min_x = 1;
  let min_y = 1;
  let L = 0, R = 0, U = 0, D = 0;

  // 5. Проходим по каждому символу в строке команд S
  for (let i = 0; i < S.length; i++) {
    const move = S[i];
    
    switch(move) {
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

  // 6. Проверяем, помещается ли путь на доску
  if (min_x > M || min_y > N) {
    return "No";
  } else {
    return `(${min_x},${min_y})`;
  }
}

// Функция для ввода данных от пользователя
function getUserInput() {
  const readline = require('readline').createInterface({
      input: process.stdin,
      output: process.stdout
  });

  return new Promise((resolve) => {
      readline.question('Введите N и M через пробел: ', (dimensions) => {
          const [N, M] = dimensions.split(' ').map(Number);
          readline.question('Введите S: ', (S) => {
              readline.close();
              resolve({ N, M, S });
          });
      });
  });
}

// Основная функция для запуска программы
async function main() {
  try {
      const { N, M, S } = await getUserInput();
      const result = findStartPosition(N, M, S);
      console.log(`Результат: ${result}`);
  } catch (error) {
      console.error('Произошла ошибка:', error.message);
  }
}

// Запуск программы
main();