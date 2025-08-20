use std::io;

fn main() {
    // Чтение входных данных
    let mut input = String::new();
    io::stdin().read_line(&mut input).expect("Ошибка при чтении ввода");
    let parts: Vec<&str> = input.trim().split_whitespace().collect();

    // Проверка количества аргументов
    if parts.len() != 3 {
        println!("Invalid input");
        return;
    }

    // Парсинг N и M
    let n: i32 = match parts[0].parse() {
        Ok(num) => num,
        Err(_) => {
            println!("Invalid input");
            return;
        }
    };
    let m: i32 = match parts[1].parse() {
        Ok(num) => num,
        Err(_) => {
            println!("Invalid input");
            return;
        }
    };

    // Проверка корректности размеров доски
    if n < 1 || m < 1 {
        println!("Invalid input");
        return;
    }

    // Получение строки команд и проверка символов
    let s = parts[2];
    for c in s.chars() {
        match c {
            'L' | 'R' | 'U' | 'D' => (),
            _ => {
                println!("Invalid input");
                return;
            }
        }
    }

    let (mut l, mut r, mut u, mut d) = (0, 0, 0, 0);
    let (mut min_x, mut min_y) = (1, 1);

    // Обработка команд с новой логикой
    for c in s.chars() {
        match c {
            'L' => {
                l += 1;
                if min_x - l < 1 {
                    min_x += 1;
                }
                if r > 0 {
                    r -= 1;
                }
            }
            'R' => {
                r += 1;
                if l > 0 {
                    l -= 1;
                }
            }
            'U' => {
                u += 1;
                if min_y - u < 1 {
                    min_y += 1;
                }
                if d > 0 {
                    d -= 1;
                }
            }
            'D' => {
                d += 1;
                if u > 0 {
                    u -= 1;
                }
            }
            _ => unreachable!(),
        }
    }

    // Проверка существования решения и вывод результата
    if min_x > m || min_y > n {
        println!("No");
    } else {
        println!("({},{})", min_x, min_y);
    }
}