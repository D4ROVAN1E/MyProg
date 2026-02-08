#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

// Настройки подключения к БД
const string CONNECTION_STRING = "dbname=librarydb user=postgres password=postgres host=localhost port=5432";

class LibraryManager {
private:
    string conn_str;

public:
    LibraryManager(string conn) : conn_str(conn) {}

    // Создание структуры БД (5 таблиц)
    void createTables() {
        try {
            pqxx::connection C(conn_str);
            pqxx::work W(C);

            cout << "[INFO] Создание таблиц..." << endl;

            // Жанры
            W.exec("CREATE TABLE IF NOT EXISTS Genres ("
                   "id SERIAL PRIMARY KEY, "
                   "name VARCHAR(100) NOT NULL UNIQUE);");

            // Авторы
            W.exec("CREATE TABLE IF NOT EXISTS Authors ("
                   "id SERIAL PRIMARY KEY, "
                   "first_name VARCHAR(100), "
                   "last_name VARCHAR(100) NOT NULL, "
                   "birth_year INT);");

            // Читатели
            W.exec("CREATE TABLE IF NOT EXISTS Readers ("
                   "id SERIAL PRIMARY KEY, "
                   "full_name VARCHAR(150) NOT NULL, "
                   "email VARCHAR(100) UNIQUE, "
                   "registration_date DATE DEFAULT CURRENT_DATE);");

            // Книги (Связь с Авторами и Жанрами)
            W.exec("CREATE TABLE IF NOT EXISTS Books ("
                   "id SERIAL PRIMARY KEY, "
                   "title VARCHAR(200) NOT NULL, "
                   "author_id INT REFERENCES Authors(id), "
                   "genre_id INT REFERENCES Genres(id), "
                   "publish_year INT, "
                   "total_copies INT DEFAULT 1);");

            // Выдачи книг (Связь Книг и Читателей)
            W.exec("CREATE TABLE IF NOT EXISTS Loans ("
                   "id SERIAL PRIMARY KEY, "
                   "book_id INT REFERENCES Books(id), "
                   "reader_id INT REFERENCES Readers(id), "
                   "loan_date DATE DEFAULT CURRENT_DATE, "
                   "return_date DATE);"); // NULL означает, что книга еще не возвращена

            W.commit();
            cout << "[OK] Структура БД успешно создана." << endl;
        } catch (const exception &e) {
            cerr << "[ERROR] Ошибка создания таблиц: " << e.what() << endl;
        }
    }

    // Заполнение тестовыми данными
    void seedData() {
        try {
            pqxx::connection C(conn_str);
            pqxx::work W(C);

            // Проверка, пуста ли БД, чтобы не дублировать данные
            pqxx::result R = W.exec("SELECT count(*) FROM Genres");
            if (R[0][0].as<int>() > 0) {
                cout << "[INFO] Данные уже существуют. Пропуск наполнения." << endl;
                return;
            }

            cout << "[INFO] Наполнение базы тестовыми данными..." << endl;

            // Вставка Жанров
            W.exec("INSERT INTO Genres (name) VALUES "
                   "('Фантастика'), ('Детектив'), ('Классика'), ('Научпоп');");

            // Вставка Авторов
            W.exec("INSERT INTO Authors (first_name, last_name, birth_year) VALUES "
                   "('Айзек', 'Азимов', 1920), "
                   "('Федор', 'Достоевский', 1821), "
                   "('Артур', 'Конан Дойл', 1859), "
                   "('Стивен', 'Хокинг', 1942);");

            // Вставка Книг
            // Предполагаем ID: 1-Азимов, 2-Достоевский, 3-Дойл, 4-Хокинг
            // Жанры: 1-Фант, 2-Детектив, 3-Класс, 4-Научпоп
            W.exec("INSERT INTO Books (title, author_id, genre_id, publish_year, total_copies) VALUES "
                   "('Я, Робот', 1, 1, 1950, 5), "
                   "('Основание', 1, 1, 1951, 3), "
                   "('Преступление и наказание', 2, 3, 1866, 10), "
                   "('Идиот', 2, 3, 1869, 4), "
                   "('Записки о Шерлоке Холмсе', 3, 2, 1892, 6), "
                   "('Краткая история времени', 4, 4, 1988, 2);");

            // Вставка Читателей
            W.exec("INSERT INTO Readers (full_name, email) VALUES "
                   "('Иван Иванов', 'ivan@mail.com'), "
                   "('Петр Петров', 'petr@test.ru'), "
                   "('Мария Сидорова', 'maria@corp.com');");

            // Вставка Выдач (Loans)
            W.exec("INSERT INTO Loans (book_id, reader_id, loan_date, return_date) VALUES "
                   "(1, 1, '2026-10-01', '2026-10-15'), " // -- Вернул
                   "(3, 1, '2026-10-20', NULL), "         // -- Не вернул
                   "(5, 2, '2026-10-21', NULL), "         // -- Не вернул
                   "(6, 3, '2026-09-01', '2026-09-10');"); // -- Вернул

            W.commit();
            cout << "[OK] Данные успешно добавлены." << endl;

        } catch (const exception &e) {
            cerr << "[ERROR] Ошибка наполнения данными: " << e.what() << endl;
        }
    }

    // Выполнение 10 запросов
    void runDemonstration() {
        try {
            pqxx::connection C(conn_str);

            cout << "\n ДЕМОНСТРАЦИЯ 10 ЗАПРОСОВ \n" << endl;

            // 1: Обновление данных (UPDATE)
            // Цель: Изменить email читателя.
            cout << "1. Обновление email читателя:" << endl;
            {
                pqxx::work W(C); // Открываем новую транзакцию для изменения
                W.exec("UPDATE Readers SET email = 'new_email@test.com' WHERE id = 1;");
                W.commit();
                cout << " Email пользователя с ID 1 обновлен." << endl;
            } 

            // Используем nontransaction для SELECT запросов для быстродействия, 
            // но work для сложных операций
            pqxx::nontransaction N(C); 

            // 2: Простая выборка (SELECT)
            // Цель: Получить список всех книг.
            cout << "2. Список всех книг:" << endl;
            pqxx::result r2 = N.exec("SELECT title, publish_year FROM Books ORDER BY title;");
            for (auto row : r2) {
                cout << " - " << row[0].c_str() << " (" << row[1].c_str() << ")" << endl;
            }

            // 3: Объединение таблиц (JOIN)
            // Цель: Вывести книги с именами авторов.
            cout << "\n3. Книги и их авторы:" << endl;
            pqxx::result r3 = N.exec(
                "SELECT b.title, a.last_name "
                "FROM Books b "
                "JOIN Authors a ON b.author_id = a.id;");
            for (auto row : r3) {
                cout << " - " << row[0].c_str() << " -- Автор: " << row[1].c_str() << endl;
            }

            // 4: Фильтрация (WHERE)
            // Цель: Найти книги, изданные после 1900 года.
            cout << "\n4. Книги новее 1900 года:" << endl;
            pqxx::result r4 = N.exec("SELECT title, publish_year FROM Books WHERE publish_year > 1900;");
            for (auto row : r4) {
                cout << " - " << row[0].c_str() << " [" << row[1].c_str() << "]" << endl;
            }

            // 5: Агрегатная функция (COUNT)
            // Цель: Посчитать общее количество книг в библиотеке.
            cout << "\n5. Общее кол-во книг в базе:" << endl;
            pqxx::result r5 = N.exec("SELECT COUNT(*) FROM Books;");
            cout << " Всего книг: " << r5[0][0].c_str() << endl;

            // 6: Группировка и Агрегация (GROUP BY)
            // Цель: Посчитать количество книг каждого жанра.
            cout << "\n6. Количество книг по жанрам:" << endl;
            pqxx::result r6 = N.exec(
                "SELECT g.name, COUNT(b.id) "
                "FROM Genres g "
                "JOIN Books b ON g.id = b.genre_id "
                "GROUP BY g.name;");
            for (auto row : r6) {
                cout << " Жанр: " << row[0].c_str() << ", Книг: " << row[1].c_str() << endl;
            }

            // 7: Условие на группу (HAVING)
            // Цель: Показать авторов, у которых в библиотеке более 1 книги.
            cout << "\n7. Авторы, у которых > 1 книги:" << endl;
            pqxx::result r7 = N.exec(
                "SELECT a.last_name, COUNT(b.id) "
                "FROM Authors a "
                "JOIN Books b ON a.id = b.author_id "
                "GROUP BY a.last_name "
                "HAVING COUNT(b.id) > 1;");
            for (auto row : r7) {
                cout << " Автор: " << row[0].c_str() << " (Книг: " << row[1].c_str() << ")" << endl;
            }

            // 8: Сложный тройной JOIN
            // Цель: Показать текущие активные выдачи (кто взял, какую книгу).
            cout << "\n8. Должники (книги на руках):" << endl;
            pqxx::result r8 = N.exec(
                "SELECT r.full_name, b.title, l.loan_date "
                "FROM Loans l "
                "JOIN Readers r ON l.reader_id = r.id "
                "JOIN Books b ON l.book_id = b.id "
                "WHERE l.return_date IS NULL;");
            if (r8.empty()) cout << "Должников нет." << endl;
            for (auto row : r8) {
                cout << " Читатель: " << row[0].c_str() 
                          << " взял '" << row[1].c_str() << "' (" << row[2].c_str() << ")" << endl;
            }

            // 9: Под(Subquery)
            // Цель: Найти книгу с самым ранним годом издания.
            cout << "\n9. Самая старая книга:" << endl;
            pqxx::result r9 = N.exec(
                "SELECT title, publish_year FROM Books "
                "WHERE publish_year = (SELECT MIN(publish_year) FROM Books);");
            cout << " Книга: " << r9[0][0].c_str() << ", Год: " << r9[0][1].c_str() << endl;

            // 10: Вычисляемое поле и сортировка
            // Цель: Показать книги и сколько лет прошло с момента издания.
            cout << "\n10. Возраст книги в годах:" << endl;
            pqxx::result r10 = N.exec(
                "SELECT title, (EXTRACT(YEAR FROM CURRENT_DATE) - publish_year) as age "
                "FROM Books "
                "ORDER BY age DESC LIMIT 3;"); // Топ 3 старых
            for (auto row : r10) {
                cout << " '" << row[0].c_str() << "' - " << row[1].c_str() << " лет назад." << endl;
            }

        } catch (const exception &e) {
            cerr << "[ERROR] Ошибка выполнения запросов: " << e.what() << endl;
        }
    }
};

int main() {
    // Установка локали для корректного вывода кириллицы
    setlocale(LC_ALL, "");

    cout << " Запуск библиотечной системы " << endl;

    // Создаем экземпляр менеджера и передаем строку подключения
    LibraryManager lib(CONNECTION_STRING);

    // 1. Создание БД
    lib.createTables();

    // 2. Наполнение данными
    lib.seedData();

    // 3. Демонстрация запросов
    lib.runDemonstration();

    cout << "\n Работа программы завершена " << endl;
    return 0;
}