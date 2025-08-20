# Считываем ввод и разделяем на части
input = gets.chomp.split

# Проверяем, что введено три элемента: N, M и S
if input.size < 3
  puts "Invalid input"
  exit
end

n_str, m_str, s = input[0], input[1], input[2]

begin
  # Преобразуем N и M в целые числа
  n = Integer(n_str)
  m = Integer(m_str)
rescue ArgumentError
  # Если преобразование не удалось, выводим ошибку
  puts "Invalid input"
  exit
end

# Проверяем, что размеры доски корректны
if n < 1 || m < 1
  puts "Invalid input"
  exit
end

# Проверяем, что строка S содержит только допустимые символы
unless s.chars.all? { |c| %w[L R U D].include?(c) }
  puts "Invalid input"
  exit
end

# Инициализируем счетчики и минимальные координаты
l = 0
r = 0
u = 0
d = 0
min_x = 1
min_y = 1

# Обрабатываем каждую команду
s.each_char do |c|
  case c
  when 'L'
    l += 1
    min_x += 1 if min_x - l < 1
    r -= 1 if r > 0
  when 'R'
    r += 1
    l -= 1 if l > 0
  when 'U'
    u += 1
    min_y += 1 if min_y - u < 1
    d -= 1 if d > 0
  when 'D'
    d += 1
    u -= 1 if u > 0
  end
end

# Проверяем существование допустимой позиции
if min_x > m || min_y > n
  puts "No"
else
  # Выводим минимальные координаты
  puts "(#{min_x},#{min_y})"
end