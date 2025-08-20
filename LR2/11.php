<?php
// Чтение входных данных из стандартного ввода
$input = trim(fgets(STDIN));
$parts = explode(' ', $input);

// Проверка наличия трёх элементов (N, M, S)
if (count($parts) < 3) {
    echo "Invalid input\n";
    exit;
}

list($nStr, $mStr, $s) = $parts;

// Проверка, что N и M являются целыми числами
if (!ctype_digit($nStr) || !ctype_digit($mStr)) {
    echo "Invalid input\n";
    exit;
}

$n = intval($nStr);
$m = intval($mStr);

// Проверка допустимости размеров доски
if ($n < 1 || $m < 1) {
    echo "Invalid input\n";
    exit;
}

// Проверка допустимости символов в строке команд
foreach (str_split($s) as $c) {
    if (!in_array($c, ['L', 'R', 'U', 'D'])) {
        echo "Invalid input\n";
        exit;
    }
}

// Подсчёт количества каждой команды
$l = $r = $u = $d = 0;
$min_x = 1;
$min_y = 1;

foreach (str_split($s) as $c) {
    switch ($c) {
        case 'L': 
            $l++;
            if ($min_x - $l < 1) $min_x++;
            if ($r > 0) $r--;
            break;
        case 'R': 
            $r++;
            if ($l > 0) $l--;
            break;
        case 'U': 
            $u++;
            if ($min_y - $u < 1) $min_y++;
            if ($d > 0) $d--;
            break;
        case 'D': 
            $d++;
            if ($u > 0) $u--;
            break;
    }
}

// Проверка существования допустимой начальной позиции
if ($min_x > $m || $min_y > $n) {
    echo "No\n";
} else {
    echo "($min_x,$min_y)\n";
}
?>