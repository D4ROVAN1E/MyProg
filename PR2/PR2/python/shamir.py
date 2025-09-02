from modulo_comparison import decomp_of_degree, gcd
from extended_euclidean import extended_euclidean

def _text_transform(cyphertext, key, p):
    """
    Вспомогательная функция для шифрования/дешифрования каждого символа.
    """
    new_text = []
    for symb in cyphertext:
        x1 = decomp_of_degree(symb, key, p)
        new_text.append(x1)
    return new_text

def encrypt(cA, cB, p, message):
    """Функция для шифрования сообщения по схеме Шамира."""
    if gcd(cA, p) != 1 or gcd(cB, p) != 1:
        raise ValueError("Все ключи должны быть взаимнопростыми с p")
    if not (256 < p < 4294967296):
        raise ValueError("Простое число должно быть больше 256 и меньше чем 2^32")

    # Преобразуем message (bytes) в список чисел
    cyphertext = list(message)

    cyphertext = _text_transform(cyphertext, cA, p)
    cyphertext = _text_transform(cyphertext, cB, p)

    return cyphertext

def decrypt(cA, cB, p, cyphertext):
    """Функция для расшифровки сообщения по схеме Шамира."""
    if not (256 < p < 4294967296):
         raise ValueError("Простое число должно быть больше 256 и меньше чем 2^32")

    # Абонент A вычисляет секретное число DA, такое что (CA * DA) mod (P-1) = 1
    # ExtendedEuclidean возвращает x в ax+by=1. Если x<0, приводим к положительному
    dA_int = extended_euclidean(cA, p - 1)
    dA = (dA_int + (p - 1)) % (p - 1)


    # Абонент B вычисляет секретное число DB, такое что (CB * DB) mod (P-1) = 1
    dB_int = extended_euclidean(cB, p - 1)
    dB = (dB_int + (p - 1)) % (p - 1)

    cyphertext = _text_transform(cyphertext, dA, p)
    cyphertext = _text_transform(cyphertext, dB, p)

    # Преобразуем список чисел обратно в байты
    message = bytes(cyphertext)
    
    return message