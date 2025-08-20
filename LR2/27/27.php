<?php
// Чтение всей строки из стандартного ввода
$input = trim(fgets(STDIN));

// Разделение входной строки на массив чисел по пробелам
$tokens = explode(' ', $input);

// Первый элемент - количество чисел
$nums = intval($tokens[0]);

for ($i = 1; $i <= $nums; $i++) {
    $token = $tokens[$i];
    $validInput = true;
    
    // Проверка, что все символы - цифры
    for ($j = 0; $j < strlen($token); $j++) {
        $symbol = ord($token[$j]) - ord('0');
        if ($symbol < 0 || $symbol > 9) {
            $validInput = false;
            break;
        }
    }
    
    // Если входная строка не прошла проверку или содержит менее двух цифр
    if (!$validInput || strlen($token) < 2) {
        echo "NO";
        continue;
    }
    
    $isCorrectDate = false;
    $number = intval($token);
    
    if ($number >= 11 && $number < 100) {
        $day = intval($number / 10);
        $month = $number % 10;
        $isCorrectDate = ($day >= 1 && $day <= 31 && $month >= 1 && $month <= 12);
    } else if ($number >= 100 && $number < 1000) {
        $day = intval($number / 10);
        $month = $number % 10;
        $isCorrectDate = ($day >= 1 && $day <= 31 && $month >= 1 && $month <= 12);
        if (!$isCorrectDate) {
            $day = intval($number / 100);
            $month = $number % 100;
            $isCorrectDate = ($day >= 1 && $day <= 31 && $month >= 1 && $month <= 12);
        }
    } else if ($number >= 1000 && $number <= 3112) {
        $day = intval($number / 100);
        $month = $number % 100;
        $isCorrectDate = ($day >= 1 && $day <= 31 && $month >= 1 && $month <= 12);
    }
    
    echo $isCorrectDate ? "YES" : "NO";
}
?>
