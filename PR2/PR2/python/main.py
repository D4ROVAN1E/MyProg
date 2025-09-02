import sys
import struct

# Импорт функций из других файлов проекта
from modulo_comparison import (
    get_modulo_comparison_input, wheel_test, theorem_conditions, 
    ferm, decomp_of_degree
)
from extended_euclidean import extended_euclidean
from shamir import encrypt, decrypt
from fraction import solve_diophantine
from attack import meet_in_the_middle_attack

def display_menu():
    """Отображает меню выбора действий."""
    print("\nВыберите действие:")
    print("1) Сравнение двух чисел с возведением в степень по модулю")
    print("2) Расширенный алгоритм Евклида для чисел c и d")
    print("3) Нахождение обратного числа по модулю")
    print("4) Шифрование и дешифрование текста алгоритмом Шамира")
    print("5) Решение уравнения a*275 + b*145 = 10")
    print("6) Демонстрация атаки 'Встреча посередине' на шифр Шамира")
    print("7) Зашифровать файл алгоритмом Шамира")
    print("8) Расшифровать файл алгоритмом Шамира")
    print("0) Выход")
    print("Ваш выбор: ", end="")

def main():
    """Основная функция программы с циклом меню."""
    while True:
        display_menu()
        try:
            choice = int(input())
        except ValueError:
            print("Ошибка: Введите число.", file=sys.stderr)
            continue

        try:
            if choice == 1:
                first_base, second_base, prime_num, first_degree, second_degree = get_modulo_comparison_input()
                
                print("\n--- Вычисление по теореме Ферма ---")
                if not wheel_test(prime_num):
                    raise ValueError("Модуль не является простым")
                if not theorem_conditions(first_base, prime_num) or not theorem_conditions(second_base, prime_num):
                    raise ValueError("Введённые данные не соответствуют условию теоремы Ферма")

                result1 = ferm(first_base, first_degree, prime_num)
                result2 = ferm(second_base, second_degree, prime_num)
                print(f"{first_base}^{first_degree} mod {prime_num} = {result1}")
                print(f"{second_base}^{second_degree} mod {prime_num} = {result2}")
                print(f"Результат: Числа {'равны' if result1 == result2 else 'не равны'} по модулю.")

                print("\n--- Вычисление по разложению степени ---")
                result1 = decomp_of_degree(first_base, first_degree, prime_num)
                result2 = decomp_of_degree(second_base, second_degree, prime_num)
                print(f"{first_base}^{first_degree} mod {prime_num} = {result1}")
                print(f"{second_base}^{second_degree} mod {prime_num} = {result2}")
                print(f"Результат: Числа {'равны' if result1 == result2 else 'не равны'} по модулю.")

            elif choice == 2:
                print("Введите два числа c и d для расширенного алгоритма Евклида: ", end="")
                c, d = map(int, input().split())
                print(f"Находим x и y для уравнения {c}x + {d}y = НОД({c}, {d})")
                x = extended_euclidean(c, d, output=True)
                print(f"Один из коэффициентов (x) = {x}")

            elif choice == 3:
                print("Введите число и модуль для нахождения обратного элемента: ", end="")
                num, mod = map(int, input().split())
                d_inv = extended_euclidean(num, mod, output=True)
                print(f"\nОбратный элемент d = {d_inv}")
                print(f"{num}^(-1) mod {mod} = {(d_inv % mod + mod) % mod}")
            
            elif choice == 4:
                p = 4091
                cA = 101
                cB = 103
                
                print("\n--- Легитимный протокол Шамира ---")
                msg_str = input("Введите сообщение для шифрования: ")
                msg = msg_str.encode('utf-8')
                
                print(f"Оригинальное сообщение: {msg_str}")
                print(f"Параметры: p={p}, cA={cA}, cB={cB}")

                cyphertext = encrypt(cA, cB, p, msg)
                # Вывод в виде списка чисел, как в C++
                print(f"Зашифрованное сообщение (шифротекст): {' '.join(map(str, cyphertext))}")

                decrypted_msg = decrypt(cA, cB, p, cyphertext)
                print(f"Расшифрованное сообщение: {decrypted_msg.decode('utf-8')}")

            elif choice == 5:
                x, y, c = 275, 145, 10
                print(f"Решение уравнения a*{x} + b*{y} = {c}")
                answer_x, answer_y = solve_diophantine(x, y, c)
                print(f"Одно из частных решений: x = {answer_x}, y = {answer_y}")
                print(f"Проверка: {x}*{answer_x} + {y}*{answer_y} = {x * answer_x + y * answer_y}")

            elif choice == 6:
                # --- Легитимный обмен ---
                p = 4091
                cA = 101
                cB = 103
                
                print("\n--- Легитимный протокол Шамира ---")
                msg_str = "This is a secret message."
                msg = msg_str.encode('utf-8')

                print(f"Оригинальное сообщение: {msg_str}")
                print(f"Параметры: p={p}, cA={cA}, cB={cB}\n")

                cyphertext = encrypt(cA, cB, p, msg)
                print(f"Зашифрованное сообщение (шифротекст): {' '.join(map(str, cyphertext))}")

                decrypted_msg = decrypt(cA, cB, p, cyphertext)
                print(f"Расшифрованное сообщение: {decrypted_msg.decode('utf-8')}\n")

                # --- Атака на основе известного открытого текста ---
                meet_in_the_middle_attack(p, msg[0], cyphertext[0], 200)

            elif choice == 7:
                in_filename = input("Введите полный путь до файла для шифрования: ")
                out_filename = input("Введите полный путь для сохранения результата: ")

                with open(in_filename, 'rb') as input_file:
                    file_data = input_file.read()

                p = 4091
                cA = 101
                cB = 103

                cypherdata = encrypt(cA, cB, p, file_data)
                
                with open(out_filename, 'wb') as output_file:
                    for block in cypherdata:
                        # Упаковываем каждое 32-битное целое число в 4 байта
                        output_file.write(struct.pack('<I', block))

                print(f"Файл '{in_filename}' успешно зашифрован в '{out_filename}'.")


            elif choice == 8:
                in_filename = input("Введите полный путь до зашифрованного файла: ")
                out_filename = input("Введите полный путь для сохранения расшифрованного результата: ")

                cypherdata = []
                with open(in_filename, 'rb') as input_file:
                    while True:
                        # Читаем 4 байта
                        chunk = input_file.read(4)
                        if not chunk:
                            break
                        # Распаковываем 4 байта в 32-битное целое число
                        block = struct.unpack('<I', chunk)[0]
                        cypherdata.append(block)

                p = 4091
                cA = 101
                cB = 103

                decrypted_data = decrypt(cA, cB, p, cypherdata)

                with open(out_filename, 'wb') as output_file:
                    output_file.write(decrypted_data)
                
                print(f"Файл '{in_filename}' успешно расшифрован в '{out_filename}'.")


            elif choice == 0:
                print("Выход из программы.")
                break
            else:
                print("Неверный выбор. Пожалуйста, попробуйте снова.")

        except (ValueError, FileNotFoundError, Exception) as e:
            print(f"Произошла ошибка: {e}", file=sys.stderr)

if __name__ == "__main__":
    main()