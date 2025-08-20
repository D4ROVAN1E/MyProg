import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int N = 0, M = 0;
        String S = "";

        try {
            // Чтение N и M из ввода
            N = scanner.nextInt();
            M = scanner.nextInt();
            // Чтение строки S (команды)
            S = scanner.next();
        } catch (Exception e) {
            // Если ввод не корректен (не числа или нет данных)
            System.out.println("Invalid input");
            scanner.close();
            return;
        }

        // Проверка, что размеры доски корректны (N и M >=1)
        if (N < 1 || M < 1) {
            System.out.println("Invalid input");
            scanner.close();
            return;
        }

        // Проверка символов в строке S на допустимость
        for (char c : S.toCharArray()) {
            if (c != 'L' && c != 'R' && c != 'U' && c != 'D') {
                System.out.println("Invalid input");
                scanner.close();
                return;
            }
        }

        int min_x = 1, min_y = 1;
        int L = 0, R = 0, U = 0, D = 0;

        // Обработка каждой команды с динамическим обновлением координат
        for (char c : S.toCharArray()) {
            switch (c) {
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
        if (min_x > M || min_y > N) {
            System.out.println("No");
        } else {
            // Вывод минимальных координат
            System.out.println("(" + min_x + "," + min_y + ")");
        }

        scanner.close();
    }
}