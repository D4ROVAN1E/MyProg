import java.util.Scanner; // Импортируем класс Scanner для чтения данных с консоли

// Класс Rod представляет стержень, на который надеваются кольца
class Rod {
    // Флаги наличия кольца определённого цвета на данном стержне
    boolean hasR = false; 
    boolean hasB = false; 
    boolean hasW = false; 
}

public class Main {
    
    public static int countPoints(String rings) {
        // Создаем массив из 10 объектов Rod, соответствующих стержням с номерами от 0 до 9
        Rod[] rods = new Rod[10];
        for (int i = 0; i < 10; i++) {
            rods[i] = new Rod(); // Инициализируем каждый стержень новым объектом Rod
        }
        
        // проверяем, что длина строки является четным числом,
        // так как каждая пара символов описывает одно кольцо.
        if (rings == null || rings.length() % 2 != 0) {
            System.out.println("Ошибка ввода: строка должна быть не null и иметь четную длину.");
            return 0; 
        }
        
        // Итерация по строке с шагом 2, так как каждая пара символов – это описание кольца.
        for (int i = 0; i < rings.length(); i += 2) {
            // Извлекаем символ, обозначающий цвет кольца.
            char color = rings.charAt(i);
            // Извлекаем символ, обозначающий номер стержня.
            char posChar = rings.charAt(i + 1);
            
            // Проверяем корректность символа позиции: он должен быть цифрой от '0' до '9'.
            if (posChar < '0' || posChar > '9') {
                continue;
            }
            
            // Преобразуем символ в числовое значение индекса стержня
            int pos = posChar - '0';
            
            // В зависимости от цвета обновляем флаги для соответствующего стержня:
            switch (color) {
                case 'R': 
                    rods[pos].hasR = true; 
                    break;
                case 'B':
                    rods[pos].hasB = true; 
                    break;
                case 'W':
                    rods[pos].hasW = true;
                    break;
                default:
                    break;
            }
        }
        
        // Считаем количество стержней, на которых присутствуют кольца всех трех цветов.
        int count = 0;
        for (int i = 0; i < rods.length; i++) {
            if (rods[i].hasR && rods[i].hasB && rods[i].hasW) {
                count++; 
            }
        }
        
        return count; // Возвращаем итоговое количество стержней с кольцами всех трех цветов
    }
    
    public static void main(String[] args) {
        // Создаем объект Scanner для чтения ввода с консоли
        Scanner scanner = new Scanner(System.in);
        
        // Запрашиваем у пользователя ввод строки, описывающей кольца
        System.out.println("Введите строку колец:");
        String input = scanner.nextLine(); // Читаем строку целиком
        
        // Вызываем метод countPoints для подсчета количества стержней, удовлетворяющих условию
        int result = countPoints(input);
        
        // Выводим результат на консоль
        System.out.println("Количество стержней с кольцами всех трех цветов: " + result);
        
        // Закрываем Scanner, чтобы освободить ресурсы
        scanner.close();
    }
}
