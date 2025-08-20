<?php
function countPoints($rings) {
    // Инициализация массива стержней. Каждый стержень имеет флаги для цветов R, B, W.
    $rods = array_fill(0, 10, ['R' => false, 'B' => false, 'W' => false]);
    $length = strlen($rings);
    
    // Обработка пар цвет-позиция с шагом 2 символа
    for ($i = 0; $i < $length; $i += 2) {
        // Проверка на нечётную длину строки (последний символ игнорируется)
        if ($i + 1 >= $length) {
            break;
        }
        
        $color = $rings[$i];      // Цвет кольца
        $posChar = $rings[$i + 1]; // Символ позиции
        
        // Пропуск недопустимых цветов (не R, B, W)
        if (!in_array($color, ['R', 'B', 'W'])) {
            continue;
        }
        
        // Пропуск недопустимых позиций (не цифра или не в диапазоне 0-9)
        if (!ctype_digit($posChar)) {
            continue;
        }
        $pos = intval($posChar);
        if ($pos < 0 || $pos > 9) {
            continue;
        }
        
        // Установка флага цвета для соответствующего стержня
        $rods[$pos][$color] = true;
    }
    
    // Подсчёт стержней с кольцами всех трёх цветов
    $count = 0;
    foreach ($rods as $rod) {
        if ($rod['R'] && $rod['B'] && $rod['W']) {
            $count++;
        }
    }
    
    return $count;
}

// Пример использования
echo "Введите строку: ";
$input = trim(fgets(STDIN));
echo countPoints($input) . "\n";
?>