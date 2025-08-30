/*Шифр Бифида, Шифр «железнодорожной изгороди», Аффинный шифр
70 % 16 + 1 - номер столбца от кода буквы
70 / 16 + 1 - номер строки от кода буквы
CP866*/
#include <fstream>
#include <iterator>
#include "Bifid.h"
#include "Affine.h"
#include "Railway fence.h"

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

enum class MenuChoice {
    Exit = 0,
    EncryptFile = 1,
    DecryptFile = 2,
    Bifid = 3,
    Affine = 4,
    Railway_fence = 5
};

pair<int, int> menu() {
    pair<int, int> choise;

    cout << "\nВыберите действие:\n";
    cout << "1) Зашифровать файл\n";
    cout << "2) Расшифровать файл\n";
    cout << "0) Выход\n";
    cout << "Ваш выбор: ";
    cin >> choise.first;

    if (choise.first) {
        cout << "\nВыберите шифрование:\n";
        cout << "3) Шифр Бифида\n";
        cout << "4) Аффинный шифр\n";
        cout << "5) Шифр \"Железнодорожной изгороди\"\n";
        cout << "Ваш выбор: ";
        cin >> choise.second;
    }

    return choise;
}

int main()
{
    setlocale(LC_ALL, "ru");


    try {

        while (true) {

            pair<int, int> choise_raw = menu();
            MenuChoice choise = static_cast<MenuChoice>(choise_raw.first);
            MenuChoice cypher = static_cast<MenuChoice>(choise_raw.second);

            switch (choise) {

            case MenuChoice::EncryptFile: {
                string in_filename, out_filename;
                // Очищаем буфер ввода перед использованием getline
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Введите полный путь до файла для шифрования: ";
                getline(cin, in_filename);

                cout << "Введите полный путь для сохранения результата: ";
                getline(cin, out_filename);

                // Открываем входной файл в бинарном режиме
                ifstream input_file(in_filename, ios::binary);
                if (!input_file.is_open()) {
                    throw "Не удалось открыть входной файл. Проверьте путь.";
                }

                // Читаем все байты из файла в вектор
                vector<uint8_t> file_data(
                    (istreambuf_iterator<char>(input_file)),
                    istreambuf_iterator<char>()
                );
                input_file.close();

                // Открываем выходной файл в бинарном режиме
                ofstream output_file(out_filename, ios::binary);
                if (!output_file.is_open()) {
                    throw "Не удалось создать выходной файл.";
                }

                vector<uint8_t> cypherdata;
                // Шифруем данные
                switch (cypher) {

                case MenuChoice::Bifid: {
                    cypherdata = bifid_encrypt(file_data);
                    break;
                }
                case MenuChoice::Affine: {
                    int a, b;
                    const int m = 256;

                    cout << "Введите ключ 'a' (число, взаимно простое с 256): ";
                    cin >> a;

                    // Проверка ключа 'a' с помощью вашей функции Gcd
                    if (Gcd(a, m) != 1) {
                        throw "Ошибка: ключ 'a' должен быть взаимно простым с 256.\n";
                    }

                    cout << "Введите ключ 'b': ";
                    cin >> b;

                    cypherdata = affine_encrypt(file_data, a, b);
                    break;
                }
                case MenuChoice::Railway_fence: {
                    uint32_t key;

                    cout << "Введите ключ (количество рельсов): ";
                    cin >> key;

                    // Проверка на корректность ввода ключа
                    if (cin.fail() || key <= 0) {
                        throw "Ошибка: Ключ должен быть положительным целым числом.\n";
                    }

                    cypherdata = railFence_encrypt(file_data, key);
                    break;
                }

                }

                // Записываем зашифрованные данные (каждое число uint8_t)
                for (const auto& block : cypherdata) {
                    output_file.write(reinterpret_cast<const char*>(&block), sizeof(uint8_t));
                }
                output_file.close();

                cout << "Файл '" << in_filename << "' успешно зашифрован в '" << out_filename << "'.\n";
                break;
            }
            
            case MenuChoice::DecryptFile: {
                string in_filename, out_filename;
                // Очищаем буфер ввода перед использованием getline
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Введите полный путь до зашифрованного файла: ";
                getline(cin, in_filename);

                cout << "Введите полный путь для сохранения расшифрованного результата: ";
                getline(cin, out_filename);

                // Открываем зашифрованный файл в бинарном режиме
                ifstream input_file(in_filename, ios::binary);
                if (!input_file.is_open()) {
                    throw "Не удалось открыть входной файл. Проверьте путь.";
                }

                // Читаем данные блоками
                vector<uint8_t> cypherdata;
                uint32_t block;
                while (input_file.read(reinterpret_cast<char*>(&block), sizeof(uint8_t))) {
                    cypherdata.push_back(block);
                }
                input_file.close();

                // Открываем выходной файл в бинарном режиме
                ofstream output_file(out_filename, ios::binary);
                if (!output_file.is_open()) {
                    throw "Не удалось создать выходной файл.";
                }

                // Расшифровываем данные
                vector<uint8_t> decrypted_data;
                switch (cypher) {

                case MenuChoice::Bifid: {
                    decrypted_data = bifid_decrypt(cypherdata);
                    break;
                }
                case MenuChoice::Affine: {
                    int a, b;
                    const int m = 256;

                    cout << "Введите ключ 'a' (число, взаимно простое с 256): ";
                    cin >> a;

                    // Проверка ключа 'a' с помощью вашей функции Gcd
                    if (Gcd(a, m) != 1) {
                        throw "Ошибка: ключ 'a' должен быть взаимно простым с 256.\n";
                    }

                    cout << "Введите ключ 'b': ";
                    cin >> b;

                    decrypted_data = affine_decrypt(cypherdata, a, b);
                    break;
                }
                case MenuChoice::Railway_fence: {
                    uint32_t key;

                    cout << "Введите ключ (количество рельсов): ";
                    cin >> key;

                    // Проверка на корректность ввода ключа
                    if (cin.fail() || key <= 0) {
                        throw "Ошибка: Ключ должен быть положительным целым числом.\n";
                    }

                    decrypted_data = railFence_decrypt(cypherdata, key);
                    break;
                }

                }

                // Записываем расшифрованные байты в файл
                output_file.write(reinterpret_cast<const char*>(decrypted_data.data()), decrypted_data.size());
                output_file.close();

                cout << "Файл '" << in_filename << "' успешно расшифрован в '" << out_filename << "'.\n";
                break;
            }

            case MenuChoice::Exit:
                return 0;

            default:
                cout << endl << endl;
                break;
            }

        }
        
    }
    catch (const char* s) {
        cerr << "Произошла ошибка: " << s << endl;
    }
    catch (const exception& e) {
        cerr << "Произошла стандартная ошибка: " << e.what() << endl;
    }

    return 0;
}
