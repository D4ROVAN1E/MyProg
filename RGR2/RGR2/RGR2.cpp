/*Шифр Бифида, Шифр «железнодорожной изгороди», Аффинный шифр
70 % 16 + 1 - номер столбца от кода буквы
70 / 16 + 1 - номер строки от кода буквы
CP866*/
#include "Bifid.h"

using namespace std;

int main()
{
    setlocale(LC_ALL, "ru");

    cout << "Введите текст для шифрования: ";
    string message;
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
