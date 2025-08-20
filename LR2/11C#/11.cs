using System;

class Program
{
    static void Main()
    {
        // Чтение и разбиение входных данных
        string[] input = Console.ReadLine().Split();

        // Проверка наличия трёх компонентов: N, M и S
        if (input.Length != 3)
        {
            Console.WriteLine("Invalid input");
            return;
        }

        // Парсинг N и M с проверкой на корректность
        int N, M;
        if (!int.TryParse(input[0], out N) || !int.TryParse(input[1], out M))
        {
            Console.WriteLine("Invalid input");
            return;
        }

        string S = input[2];

        // Проверка допустимости размеров доски
        if (N < 1 || M < 1 || S == "")
        {
            Console.WriteLine("Invalid input");
            return;
        }

        // Проверка корректности символов в строке S
        foreach (char c in S)
        {
            if (c != 'L' && c != 'R' && c != 'U' && c != 'D')
            {
                Console.WriteLine("Invalid input");
                return;
            }
        }

        int min_x = 1, min_y = 1;
        int L = 0, R = 0, U = 0, D = 0;

        // Подсчёт количества команд каждого типа с новой логикой
        foreach (char c in S)
        {
            switch (c)
            {
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

        // Проверка существования допустимой начальной позиции
        if (min_x > M || min_y > N)
        {
            Console.WriteLine("No");
        }
        else
        {
            Console.WriteLine($"({min_x},{min_y})");
        }
    }
}