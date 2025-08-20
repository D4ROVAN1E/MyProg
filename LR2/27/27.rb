# Чтение входной строки и разделение на токены по пробелам
tokens = gets.chomp.split

# Первый токен - количество чисел для проверки
nums = tokens[0].to_i

# Обработка каждого токена из введенной строки
(1..nums).each do |i|
  token = tokens[i]
  
  # Проверка на корректность входных данных:
  # 1. Токен состоит только из цифр (регулярное выражение)
  # 2. Длина токена не менее 2 символов
  if token !~ /^\d+$/ || token.size < 2
    print 'NO'
    next # Переходим к следующему токену
  end

  number = token.to_i
  valid_date = false

  if number >= 11 && number < 100
    day = number / 10
    month = number % 10
    valid_date = (1..31).cover?(day) && (1..12).cover?(month)
  elsif number >= 100 && number < 1000
    # Первый вариант разделения
    day = number / 10
    month = number % 10
    valid_date = (1..31).cover?(day) && (1..12).cover?(month)
    
    # Второй вариант разделения, если первый не подошел
    unless valid_date
      day = number / 100
      month = number % 100
      valid_date = (1..31).cover?(day) && (1..12).cover?(month)
    end
  elsif number >= 1000 && number <= 3112
    day = number / 100
    month = number % 100
    valid_date = (1..31).cover?(day) && (1..12).cover?(month)
  end

  # Вывод результата для текущего токена
  print valid_date ? 'YES' : 'NO'
end

puts # Перевод строки после всех выводов