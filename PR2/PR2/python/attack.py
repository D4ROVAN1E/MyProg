from modulo_comparison import gcd, decomp_of_degree
from extended_euclidean import extended_euclidean

def meet_in_the_middle_attack(p, M, C, key_limit):
    """
    Функция, реализующая атаку "встреча посередине".
    M и C - это числовые представления первого символа.
    """
    print("--- Запуск атаки 'Встреча посередине' ---")
    print("Атакуем первый символ сообщения...")
    print(f"Символ открытого текста (M): {M}")
    print(f"Символ шифротекста (C): {C}")
    print(f"Поиск ключей в диапазоне до: {key_limit}\n")

    intermediate_values = {}  # Аналог std::map

    # Сторона Алисы. Шифруем M всеми возможными ключами gA и сохраняем результаты.
    print(f"Фаза 1: Вычисление и сохранение M^gA для gA от 2 до {key_limit}...")
    for gA in range(2, key_limit + 1):
        if gcd(gA, p - 1) == 1:  # Ключ должен иметь обратный элемент
            intermediate = decomp_of_degree(M, gA, p)
            intermediate_values[intermediate] = gA
    
    print(f"Фаза 1 завершена. Сохранено {len(intermediate_values)} промежуточных значений.\n")

    # Сторона Боба. Расшифровываем C всеми возможными ключами gB и ищем совпадение.
    print("Фаза 2: Вычисление C^d_gB и поиск совпадения...")
    for gB in range(2, key_limit + 1):
        if gcd(gB, p - 1) != 1:
            continue  # gB должен иметь обратный элемент

        # Находим d_gB, такое что gB * d_gB = 1 mod (p-1)
        d_gB_int = extended_euclidean(gB, p - 1)
        d_gB = (d_gB_int + (p - 1)) % (p - 1)

        decrypted_intermediate = decomp_of_degree(C, d_gB, p)

        # Проверяем, есть ли результат в сохраненных значениях из Шага 1
        if decrypted_intermediate in intermediate_values:
            found_cA = intermediate_values[decrypted_intermediate]
            found_cB = gB

            print("--- АТАКА УСПЕШНА! ---")
            print("Найдено совпадение!")
            print(f"Промежуточное значение: {decrypted_intermediate}")
            print("Найдены потенциальные ключи:")
            print(f"  cA = {found_cA}")
            print(f"  cB = {found_cB}")
            print("--------------------------")
            return

    print("--- АТАКА ПРОВАЛЕНА ---")
    print(f"Не удалось найти ключи в заданном диапазоне поиска ({key_limit}).")
    print("---------------------")