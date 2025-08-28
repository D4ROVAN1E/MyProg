/*Шифр Бифида, Шифр «железнодорожной изгороди», Аффинный шифр
70 % 16 + 1 - номер столбца от кода буквы
70 / 16 + 1 - номер строки от кода буквы
CP866*/
#include "Bifid.h"
#include "Affine.h"

using namespace std;

// Перегрузка оператора для красивого вывода вектора uint8_t (как строку)
ostream& operator<<(ostream& os, const vector<uint8_t>& vec) {
    for (uint8_t element : vec) {
        os << static_cast<char>(element);
    }
    return os;
}

template <typename T>
ostream& operator<<(ostream& os, vector<T> vec) {
    for (T element : vec) {
        os << element << " ";
    }
    os << "\n";
    return os;
}

int main()
{
    setlocale(LC_ALL, "ru");


    while (true) {

        int choise;
        cin >> choise;
        switch (choise) {

            case 1: {

                cout << "Введите текст для шифрования: ";
                string message;
                cin.ignore(1);
                getline(cin, message);

                if (message.empty()) {
                    cout << "Введена пустая строка. Завершение программы." << endl;
                    return 0;
                }

                cout << "\nОригинальный текст: " << message << endl;

                string encrypted_message = bifid_encrypt(message);
                cout << "Зашифрованный текст: " << encrypted_message << endl;

                string decrypted_message = bifid_decrypt(encrypted_message);
                cout << "Расшифрованный текст: " << decrypted_message << endl;

                cout << "\nПроверка: " << (message == decrypted_message ? "Успешно!" : "Ошибка!") << endl;
            }

            case 2: {
                string text;
                int32_t a, b;
                const int32_t m = 256;

                cout << "Введите текст для шифрования: ";
                cin.ignore(1);
                getline(cin, text);

                cout << "Введите ключ 'a' (число, взаимно простое с 256): ";
                cin >> a;

                // Проверка ключа 'a' с помощью вашей функции Gcd
                if (Gcd(a, m) != 1) {
                    cout << "Ошибка: ключ 'a' должен быть взаимно простым с 256." << endl;
                    return 1;
                }

                cout << "Введите ключ 'b': ";
                cin >> b;


                vector<uint8_t> encrypted_text = encrypt(text, a, b);
                cout << "\nЗашифрованный текст: " << encrypted_text << endl;

                string decrypted_text = decrypt(encrypted_text, a, b);
                cout << "Расшифрованный текст: " << decrypted_text << endl;
            }

            case 0:
                return 0;

            default:
                cout << endl << endl;
        }

    }

    return 0;
}
