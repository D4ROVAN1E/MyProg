using System;

class Program
{
    // Структура для представления стержня с флагами цветов
    struct Rod
    {
        public bool HasR;
        public bool HasB;
        public bool HasW;
    }

    static int CountPoints(string rings)
    {
        // Инициализация 10 стержней
        Rod[] rods = new Rod[10];
        
        // Проверка на пустую строку
        if (string.IsNullOrEmpty(rings))
            return 0;

        // Обработка пар цвет-позиция
        for (int i = 0; i < rings.Length; i += 2)
        {
            // Пропуск последнего символа, если длина нечётная
            if (i + 1 >= rings.Length)
                break;

            char color = rings[i];
            char posChar = rings[i + 1];

            // Проверка, что позиция - цифра от 0 до 9
            if (!char.IsDigit(posChar))
                continue;

            int pos = posChar - '0';
            if (pos < 0 || pos > 9)
                continue;

            // Обновление флагов цвета для стержня
            switch (color)
            {
                case 'R':
                    rods[pos].HasR = true;
                    break;
                case 'B':
                    rods[pos].HasB = true;
                    break;
                case 'W':
                    rods[pos].HasW = true;
                    break;
                // Игнорирование недопустимых цветов
                default:
                    break;
            }
        }

        // Подсчёт стержней с тремя цветами
        int count = 0;
        foreach (Rod rod in rods)
        {
            if (rod.HasR && rod.HasB && rod.HasW)
                count++;
        }
        return count;
    }

    static void Main()
    {
        Console.WriteLine("Input:");
        string input = Console.ReadLine(); // Чтение и очистка ввода
        if (input == null)
        {
            Console.WriteLine(0);
            return;
        }
        Console.WriteLine(CountPoints(input));
    }
}