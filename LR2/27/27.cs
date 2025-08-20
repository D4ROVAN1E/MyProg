using System;

class Program
{
    static bool CorrectDate(int day, int month)
    {
        return day >= 1 && day <= 31 && month >= 1 && month <= 12;
    }

    static void Main()
    {
        string input = Console.ReadLine();
        string[] tokens = input.Split(new[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
        
        if (tokens.Length == 0) return;
        
        // Проверяем, что первый токен является числом
        bool isFirstNumber = int.TryParse(tokens[0], out int nums);
        if (!isFirstNumber)
        {
            Console.WriteLine("NO");
            return;
        }
        
        for (int i = 1; i <= nums && i < tokens.Length; i++)
        {
            string token = tokens[i];
            bool validInput = true;
            
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
                Console.WriteLine("NO");
                continue;
            }
            
            bool isCorrectDate = false;
            int number = int.Parse(token);
            
            if (number >= 11 && number < 100)
            {
                int day = number / 10;
                int month = number % 10;
                isCorrectDate = CorrectDate(day, month);
            }
            else if (number >= 100 && number < 1000)
            {
                int day = number / 10;
                int month = number % 10;
                isCorrectDate = CorrectDate(day, month);
                if (!isCorrectDate)
                {
                    day = number / 100;
                    month = number % 100;
                    isCorrectDate = CorrectDate(day, month);
                }
            }
            else if (number >= 1000 && number <= 3112)
            {
                int day = number / 100;
                int month = number % 100;
                isCorrectDate = CorrectDate(day, month);
            }

            Console.WriteLine(isCorrectDate ? "YES" : "NO");
        }
    }
}