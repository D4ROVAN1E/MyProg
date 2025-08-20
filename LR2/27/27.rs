use std::io::{self, BufRead};

/// Проверяет, является ли дата корректной
fn is_correct_date(day: u32, month: u32) -> bool {
    day >= 1 && day <= 31 && month >= 1 && month <= 12
}

fn main() {
    let stdin = io::stdin();
    let mut input = String::new();
    stdin.lock().read_line(&mut input).expect("Ошибка при чтении строки");
    
    let tokens: Vec<&str> = input.trim().split_whitespace().collect();
    let nums: usize = tokens[0].parse().expect("Ошибка при парсинге количества чисел");
    
    for i in 1..=nums {
        if i >= tokens.len() {
            break;
        }
        
        let token = tokens[i];
        let mut valid_input = true;
        
        // Проверка, что все символы - цифры
        for c in token.chars() {
            if !c.is_ascii_digit() {
                valid_input = false;
                break;
            }
        }
        
        if !valid_input || token.len() < 2 {
            print!("NO");
            continue;
        }
        
        let number: u32 = token.parse().unwrap();
        let mut is_valid_date = false;
        
        if number >= 11 && number < 100 {
            let day = number / 10;
            let month = number % 10;
            is_valid_date = is_correct_date(day, month);
        } else if number >= 100 && number < 1000 {
            let day = number / 10;
            let month = number % 10;
            is_valid_date = is_correct_date(day, month);
            
            if !is_valid_date {
                let day = number / 100;
                let month = number % 100;
                is_valid_date = is_correct_date(day, month);
            }
        } else if number >= 1000 && number <= 3112 {
            let day = number / 100;
            let month = number % 100;
            is_valid_date = is_correct_date(day, month);
        }
        
        print!("{}", if is_valid_date { "YES" } else { "NO" });
    }
}