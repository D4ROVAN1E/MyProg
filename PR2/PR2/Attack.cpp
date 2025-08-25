#include "Attack.h"



// Функция, реализующая атаку "встреча посередине"
void meet_in_the_middle_attack(const uint32_t p, const uint8_t M, const uint32_t C, const uint32_t key_limit) {
    cout << "--- Запуск атаки 'Встреча посередине' ---" << endl;
    cout << "Атакуем первый символ сообщения..." << endl;
    cout << "Символ открытого текста (M): " << static_cast<uint32_t>(M) << endl;
    cout << "Символ шифротекста (C): " << C << endl;
    cout << "Поиск ключей в диапазоне до: " << key_limit << endl << endl;

    map<uint32_t, uint32_t> intermediate_values;

    // Шаг 1: Сторона Алисы. Шифруем M всеми возможными ключами gA и сохраняем результаты.
    cout << "Фаза 1: Вычисление и сохранение M^gA для gA от 2 до " << key_limit << "..." << endl;
    for (uint32_t gA = 2; gA <= key_limit; ++gA) {
        if (Gcd(gA, p - 1) == 1) { // Ключ должен иметь обратный элемент
            uint32_t intermediate = DecompOfDegree(M, gA, p);
            intermediate_values[intermediate] = gA;
        }
    }
    cout << "Фаза 1 завершена. Сохранено " << intermediate_values.size() << " промежуточных значений." << endl << endl;

    // Шаг 2: Сторона Боба. Расшифровываем C всеми возможными ключами gB и ищем совпадение.
    cout << "Фаза 2: Вычисление C^d_gB и поиск совпадения..." << endl;
    for (uint32_t gB = 2; gB <= key_limit; ++gB) {
        if (Gcd(gB, p - 1) != 1) {
            continue; // gB должен иметь мультипликативный обратный элемент
        }

        // Находим d_gB, такое что gB * d_gB = 1 mod (p-1)
        int32_t d_gB_int = ExtendedEuclidean(gB, p - 1, p - 1, false);
        uint32_t d_gB = (d_gB_int % static_cast<int32_t>(p - 1) + (p - 1)) % (p - 1);

        uint32_t decrypted_intermediate = DecompOfDegree(C, d_gB, p);

        // Проверяем, есть ли результат в сохраненных значениях из Шага 1
        if (intermediate_values.count(decrypted_intermediate)) {
            uint32_t found_cA = intermediate_values[decrypted_intermediate];
            uint32_t found_cB = gB;

            cout << "--- АТАКА УСПЕШНА! ---" << endl;
            cout << "Найдено совпадение!" << endl;
            cout << "Промежуточное значение: " << decrypted_intermediate << endl;
            cout << "Найдены потенциальные ключи:" << endl;
            cout << "  cA = " << found_cA << endl;
            cout << "  cB = " << found_cB << endl;
            cout << "--------------------------" << endl;
            return;
        }
    }

    cout << "--- АТАКА ПРОВАЛЕНА ---" << endl;
    cout << "Не удалось найти ключи в заданном диапазоне поиска (" << key_limit << ")." << endl;
    cout << "---------------------" << endl;
}