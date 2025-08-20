// Подключаем модуль 'readline' для организации работы с консолью
const readline = require('readline');

function correctDate(day, month) {
    return day >= 1 && day <= 31 && month >= 1 && month <= 12;
}

function checkDate(token) {
    // Проверяем, что токен состоит только из цифр
    if (!/^\d+$/.test(token) || token.length < 2) {
        return "NO";
    }
    
    let number = parseInt(token, 10);
    let isCorrectDate = false;
    
    if (number >= 11 && number < 100) {
        let day = Math.floor(number / 10);
        let month = number % 10;
        isCorrectDate = correctDate(day, month);
    } else if (number >= 100 && number < 1000) {
        let day = Math.floor(number / 10);
        let month = number % 10;
        isCorrectDate = correctDate(day, month);
        if (!isCorrectDate) {
            day = Math.floor(number / 100);
            month = number % 100;
            isCorrectDate = correctDate(day, month);
        }
    } else if (number >= 1000 && number <= 3112) {
        let day = Math.floor(number / 100);
        let month = number % 100;
        isCorrectDate = correctDate(day, month);
    }
    
    return isCorrectDate ? "YES" : "NO";
}

async function main() {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    try {
        const input = await new Promise(resolve => {
            rl.question('Введите числа через пробел: ', resolve);
        });
        
        const tokens = input.trim().split(/\s+/);
        const nums = parseInt(tokens[0], 10);
        
        if (isNaN(nums) || tokens.length < nums + 1) {
            console.log("Invalid input");
            rl.close();
            return;
        }
        
        let result = '';
        for (let i = 1; i <= nums; i++) {
            result += checkDate(tokens[i]);
        }
        console.log(result);
        
    } catch (error) {
        console.error('Произошла ошибка:', error.message);
    } finally {
        rl.close();
    }
}

main();
