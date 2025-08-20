import java.util.Scanner;

public class Main {

    public static boolean correctDate(int day, int month) {
        return day >= 1 && day <= 31 && month >= 1 && month <= 12;
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        int nums = scanner.nextInt();
        
        for (int i = 0; i < nums; i++) {
            String token = scanner.next();
            boolean validInput = true;
            
            // Проверка, состоит ли входная строка только из цифр
            for (int j = 0; j < token.length(); j++) {
                if (!Character.isDigit(token.charAt(j))) {
                    validInput = false;
                    break;
                }
            }
            
            if (!validInput || token.length() < 2) {
                System.out.print("NO");
                continue;
            }
            
            boolean isCorrectDate = false;
            int number = Integer.parseInt(token);
            
            if (number >= 11 && number < 100) {
                int day = number / 10;
                int month = number % 10;
                isCorrectDate = correctDate(day, month);
            } else if (number >= 100 && number < 1000) {
                int day = number / 10;
                int month = number % 10;
                isCorrectDate = correctDate(day, month);
                if (!isCorrectDate) {
                    day = number / 100;
                    month = number % 100;
                    isCorrectDate = correctDate(day, month);
                }
            } else if (number >= 1000 && number <= 3112) {
                int day = number / 100;
                int month = number % 100;
                isCorrectDate = correctDate(day, month);
            }
            
            System.out.print(isCorrectDate ? "YES" : "NO");
        }
        
        scanner.close();
    }
}