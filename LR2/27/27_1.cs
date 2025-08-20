using System;
using System.Linq;

class Program
{
    static bool IsCorrectDate(int day, int month)
    {
        return day >= 1 && day <= 31 && month >= 1 && month <= 12;
    }

    static void Main()
    {
        string[] input = Console.ReadLine().Split(' ', StringSplitOptions.RemoveEmptyEntries);
        int n = int.Parse(input[0]);
        
        for (int i = 1; i <= n; i++)
        {
            string token = input[i];
            bool validInput = true;
            
            // Проверяем, что все символы являются цифрами
            foreach (char c in token)
            {
                if (!char.IsDigit(c))
                {
                    validInput = false;
                    break;
                }
            }
            
            if (!validInput || token.Length < 2)
            {
                Console.Write("NO");
                continue;
            }
            
            bool isCorrectDate = false;
            int number = int.Parse(token);
            
            if (number >= 11 && number < 100)
            {
                int day = number / 10;
                int month = number % 10;
                isCorrectDate = IsCorrectDate(day, month);
            }
            else if (number >= 100 && number < 1000)
            {
                int day = number / 10;
                int month = number % 10;
                isCorrectDate = IsCorrectDate(day, month);
                
                if (!isCorrectDate)
                {
                    day = number / 100;
                    month = number % 100;
                    isCorrectDate = IsCorrectDate(day, month);
                }
            }
            else if (number >= 1000 && number <= 3112)
            {
                int day = number / 100;
                int month = number % 100;
                isCorrectDate = IsCorrectDate(day, month);
            }
            
            Console.Write(isCorrectDate ? "YES" : "NO");
        }
        Console.WriteLine();
    }
}
