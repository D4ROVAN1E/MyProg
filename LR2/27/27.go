package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"unicode"
)

func correctDate(day, month int) bool {
	return day >= 1 && day <= 31 && month >= 1 && month <= 12
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Split(bufio.ScanWords)

	// Считываем количество чисел
	scanner.Scan()
	nums, _ := strconv.Atoi(scanner.Text())

	for i := 0; i < nums; i++ {
		scanner.Scan()
		token := scanner.Text()

		valid := true
		// Проверяем, все ли символы в токене являются цифрами
		for _, c := range token {
			if !unicode.IsDigit(c) {
				valid = false
				break
			}
		}
		if !valid || len(token) < 2 {
			fmt.Print("NO")
			continue
		}

		number, _ := strconv.Atoi(token)
		isCorrect := false

		if number >= 11 && number < 100 {
			day := number / 10
			month := number % 10
			isCorrect = correctDate(day, month)
		} else if number >= 100 && number < 1000 {
			day := number / 10
			month := number % 10
			isCorrect = correctDate(day, month)
			if !isCorrect {
				day = number / 100
				month = number % 100
				isCorrect = correctDate(day, month)
			}
		} else if number >= 1000 && number <= 3112 {
			day := number / 100
			month := number % 100
			isCorrect = correctDate(day, month)
		}

		if isCorrect {
			fmt.Print("YES")
		} else {
			fmt.Print("NO")
		}
	}
}
