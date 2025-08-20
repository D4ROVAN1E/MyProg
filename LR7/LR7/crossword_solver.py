def get_overlap_length(s1, s2):
    """Функция для нахождения перекрытия между двумя словами"""
    max_length = 0
    min_len = min(len(s1), len(s2))
    
    # Ищем перекрытия длиной от 2 символов
    for i in range(2, min_len + 1):
        if s1[-i:] == s2[:i]:
            max_length = i
    
    return max_length

def solve_crossword(current_chain, available_words, best_chain):
    """Рекурсивная функция для построения линейного кроссворда"""
    if len(current_chain) > len(best_chain[0]):
        best_chain[0] = current_chain.copy()
    
    for i in range(len(available_words)):
        next_word = available_words[i]
        can_add = False
        
        if not current_chain:
            can_add = True
        else:
            # Проверяем перекрытие с последним словом в цепочке
            last_word = current_chain[-1]
            can_add = get_overlap_length(last_word, next_word) > 0
        
        if can_add:
            # Добавляем слово в текущую цепочку
            current_chain.append(next_word)
            # Удаляем слово из доступных, чтобы не использовать его повторно
            word_to_restore = available_words.pop(i)
            
            solve_crossword(current_chain, available_words, best_chain)
            
            # Возвращаем слово обратно (backtracking)
            available_words.insert(i, word_to_restore)
            current_chain.pop()

def main():
    print("=== Линейный кроссворд ===")
    print("Введите слова через пробел (например, ЛОГИКА КАСКАД АДРЕС):")
    
    line = input().strip()
    words = line.split()
    
    if not words:
        print("Нет слов для построения кроссворда.")
        return
    
    current_chain = []
    best_chain = [[]]  # Используем список в списке для передачи по ссылке
    
    solve_crossword(current_chain, words, best_chain)
    
    if len(best_chain[0]) < 2:
        print("Не удалось построить цепочку слов.")
    else:
        print("Найденная цепочка:")
        print(" -> ".join(best_chain[0]))

if __name__ == "__main__":
    main()