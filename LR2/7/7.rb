# Функция для подсчета стержней с кольцами всех трех цветов
def count_points(rings)
    # Инициализируем массив из 10 хэшей, каждый представляет стержень
    # Каждый хэш содержит флаги для цветов R, B, W (по умолчанию false)
    rods = Array.new(10) { { r: false, b: false, w: false } }
  
    # Проходим по строке с шагом 2 (каждые два символа - пара цвет-позиция)
    (0...rings.length).step(2) do |i|
      # Проверяем, что не выходим за пределы строки
      next if i + 1 >= rings.length
  
      color_char = rings[i].downcase # Приводим к нижнему регистру для удобства
      position_char = rings[i + 1]
  
      # Пропускаем недопустимые цвета (только R, B, W)
      next unless ['r', 'b', 'w'].include?(color_char)
  
      # Пропускаем недопустимые позиции (только цифры 0-9)
      next unless position_char =~ /\d/ # Проверка на цифру
      position = position_char.to_i
  
      # Обновляем соответствующий флаг в хэше стержня
      rods[position][color_char.to_sym] = true
    end
  
    # Считаем количество стержней, у которых все три цвета присутствуют
    rods.count { |rod| rod[:r] && rod[:b] && rod[:w] }
  end
  
  # Основная программа
  puts "Введите строку с кольцами:"
  input = gets.chomp
  
  # Защита от пустого ввода
  if input.empty?
    puts "Ошибка: введена пустая строка."
    exit
  end
  
  # Проверка на минимальную длину (хотя бы одна пара символов)
  if input.length < 2
    puts "Ошибка: строка слишком короткая."
    exit
  end
  
  # Вызываем функцию и выводим результат
  puts "Результат: #{count_points(input)}"