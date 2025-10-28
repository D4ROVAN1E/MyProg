#include "dh.hpp"
#include <sstream>

//Вспомогательная рекурсивная функция для подсчёта подчинённых с использованием мемоизации (subordinateCounts) и детекции циклов (visiting)
uint32_t countSubordinates(
    const string& person,
    DoubleHash<Array<string>>& subordinatesMap,
    DoubleHash<uint32_t>& subordinateCounts,
    DoubleHash<bool>& visiting)
{
    //Если результат для 'person' уже есть в 'subordinateCounts', возвращаем его, не делая лишних вычислений.
    uint32_t* cachedCount = subordinateCounts.find(person);
    if (cachedCount != nullptr) {
        return *cachedCount;
    }

    //Детекция цикла (если мы уже в процессе подсчёта этого человека)
    //Это предотвращает бесконечную рекурсию, если A -> B, а B -> A.
    if (visiting.find(person) != nullptr) {
        cout << "Обнаружен цикл для: " << person << ". Подсчёт для этой ветки прерван." << endl;
        return 0; //Не считаем подчинённых в циклической ветке
    }

    //Отмечаем, что мы начали подсчёт для этого человека
    visiting.insert(person, true);

    //Ищем прямых подчинённых
    Array<string>* directSubs = subordinatesMap.find(person);

    //Базовый случай: нет прямых подчинённых
    if (directSubs == nullptr) {
        visiting.remove(person);      //Завершили подсчёт (убираем метку)
        subordinateCounts.insert(person, 0); //Мемоизация: 0 подчинённых
        return 0;
    }

    //Рекурсивный случай: считаем подчинённых
    uint32_t totalCount = 0;
    for (uint32_t i = 0; i < directSubs->size; i++) {
        string sub = MGET_BY_IND(*directSubs, i);

        //Игнорируем прямую петлю (напр., "E" -> "E")
        if (sub == person) {
            continue;
        }

        //Считаем 1 (за самого прямого подчинённого) 
        //+ всех его подчинённых 
        totalCount += 1 + countSubordinates(sub, subordinatesMap, subordinateCounts, visiting);
    }

    //Завершаем подсчёт, убираем метку и сохраняем результат в 'subordinateCounts'
    visiting.remove(person);
    subordinateCounts.insert(person, totalCount);
    return totalCount;
}

int main() {
    setlocale(LC_ALL, "ru");
    //Исходные данные (сотрудник, менеджер)
    Array<string> employees;
    Array<string> managers;

    /*MPUSH_BACK(employees, string("A")); MPUSH_BACK(managers, string("B"));
    MPUSH_BACK(employees, string("C")); MPUSH_BACK(managers, string("B"));
    MPUSH_BACK(employees, string("D")); MPUSH_BACK(managers, string("E"));
    MPUSH_BACK(employees, string("B")); MPUSH_BACK(managers, string("E"));
    MPUSH_BACK(employees, string("E")); MPUSH_BACK(managers, string("E"));*/

	string line;
	getline(cin, line);
	istringstream  pers(line);
    bool change = false;
    while (pers >> line) {
        if (!change) {
            MPUSH_BACK(employees, line);
			change = true;
        }
        else {
            MPUSH_BACK(managers, line);
			change = false;
        }
    }
    if (employees.size != managers.size) {
        cout << "Ошибка: количество сотрудников и менеджеров не совпадает!" << endl;
        return 1;
	}

    //Карта для хранения списков прямых подчинённых: Менеджер -> Array<Сотрудник>
    DoubleHash<Array<string>> subordinatesMap;

    //Хэш-таблица для хранения уникальных имён (используется как "set")
    DoubleHash<bool> seenPeople;

    //Массив всех уникальных людей для итогового вывода
    Array<string> allPeople;

    //Заполняем subordinatesMap и allPeople
    for (uint32_t i = 0; i < employees.size; i++) {
        string emp = MGET_BY_IND(employees, i);
        string mgr = MGET_BY_IND(managers, i);

        //Добавляем в карту подчинённых
        Array<string>* subsList = subordinatesMap.find(mgr);
        if (subsList == nullptr) {
            //Если менеджера ещё нет в карте, создаём новый список
            Array<string> newSubsList;
            MPUSH_BACK(newSubsList, emp);
            subordinatesMap.insert(mgr, newSubsList);
        }
        else {
            //Если есть, просто добавляем сотрудника в существующий список
            //Так как find() вернул указатель, мы меняем данные прямо в хэш-таблице
            MPUSH_BACK(*subsList, emp);
        }

        //Добавляем сотрудника в список всех людей (если его там нет)
        if (seenPeople.find(emp) == nullptr) {
            seenPeople.insert(emp, true);
            MPUSH_BACK(allPeople, emp);
        }
        //Добавляем менеджера в список всех людей (если его там нет)
        if (seenPeople.find(mgr) == nullptr) {
            seenPeople.insert(mgr, true);
            MPUSH_BACK(allPeople, mgr);
        }
    }

    //Создаём структуры для подсчёта

    //Карта для мемоизации (хранения) результатов: Человек -> Кол-во подчинённых
    DoubleHash<uint32_t> subordinateCounts;

    //Хэш-таблица для детекции циклов во время рекурсии
    DoubleHash<bool> visiting;

    //Запускаем подсчёт для каждого человека
    //Мы должны вызвать функцию для каждого, чтобы гарантировать,
    //что все, включая тех, кто не является менеджером (A, C, D),
    //получат запись (с 0) в 'subordinateCounts'.
    for (uint32_t i = 0; i < allPeople.size; i++) {
        string person = MGET_BY_IND(allPeople, i);
        //Рекурсивная функция сама заполнит 'subordinateCounts'
        countSubordinates(person, subordinatesMap, subordinateCounts, visiting);
    }

    //Выводим результат
    cout << "Результат: ";
    for (uint32_t i = 0; i < allPeople.size; i++) {
        string person = MGET_BY_IND(allPeople, i);

        //Находим подсчитанное значение в карте
        uint32_t* count = subordinateCounts.find(person);

        if (count != nullptr) {
            cout << person << "-" << *count;
        }

        if (i < allPeople.size - 1) {
            cout << ", ";
        }
    }
    cout << endl;

    return 0;
}