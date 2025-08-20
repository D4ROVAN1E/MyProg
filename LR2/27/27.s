.section .data
    input_buffer: .space 256      # Буфер для ввода
    yes_msg: .ascii "YES\n"       # Сообщение "YES" с переводом строки
    no_msg: .ascii "NO\n"         # Сообщение "NO" с переводом строки
    
.section .text
.globl _start

_start:
    # Основной цикл чтения строк
read_line:
    # Читаем строку в буфер
    movq $0, %rax                 # sys_read
    movq $0, %rdi                 # stdin
    movq $input_buffer, %rsi      # буфер
    movq $256, %rdx              # максимальный размер
    syscall
    
    # Проверяем конец ввода
    cmpq $1, %rax                # если прочитано <= 1 символ (только \n)
    jle exit                     # завершаем программу
    
    # Инициализируем указатель на текущую позицию в буфере
    movq $input_buffer, %r12     # сохраняем указатель в регистре r12
    
read_next_number:
    # Обрабатываем число
    movq $0, %rcx               # счетчик цифр
    movq $0, %r8                # накопитель числа
    movq %r12, %rsi            # текущая позиция в строке
    
count_digits:
    movb (%rsi), %al
    cmpb $10, %al               # проверяем на \n
    je check_and_process
    cmpb $32, %al              # проверяем на пробел
    je check_and_process
    cmpb $0, %al               # проверяем на конец строки
    je check_and_process
    
    subb $48, %al              # конвертируем ASCII в число
    cmpb $0, %al
    jl invalid_digit
    cmpb $9, %al
    jg invalid_digit
    
    # Добавляем цифру к числу
    imulq $10, %r8
    movzbq %al, %rax
    addq %rax, %r8
    
    incq %rcx                  # увеличиваем счетчик цифр
    incq %rsi                  # следующий символ
    jmp count_digits

check_and_process:
    # Если не было цифр, пропускаем обработку
    cmpq $0, %rcx
    je skip_process
    
    # Сохраняем следующую позицию
    movq %rsi, %r12
    
    # Проверяем возможность разделения на день/месяц
    cmpq $1, %rcx              # если одна цифра
    jle print_no
    
    # Перебираем все возможные разделения
    movq %r8, %r9              # копируем число
    movq $10, %r10             # начальный делитель
    
try_split:
    movq %r9, %rax
    movq $0, %rdx
    divq %r10                  # делим на степень 10
    
    # rax = первая часть (день)
    # rdx = вторая часть (месяц)
    
    # Проверяем день (1-31)
    cmpq $0, %rax
    jle next_split
    cmpq $31, %rax
    jg next_split
    
    # Проверяем месяц (1-12)
    cmpq $0, %rdx
    jle next_split
    cmpq $12, %rdx
    jg next_split
    
    # Если дошли сюда - нашли правильное разделение
    jmp print_yes
    
next_split:
    imulq $10, %r10
    cmpq %r9, %r10
    jl try_split               # Проверяем все возможные разделения
    
    # Если не нашли подходящего разделения
    jmp print_no
    
skip_process:
    incq %r12                  # пропускаем разделитель
    
continue_reading:
    # Проверяем, есть ли еще числа в строке
    movb (%r12), %al
    cmpb $10, %al              # если новая строка
    je read_line              # читаем следующую строку
    cmpb $0, %al               # если конец строки
    je read_line              # читаем следующую строку
    jmp read_next_number       # обрабатываем следующее число
    
print_no:
    movq $1, %rax              # sys_write
    movq $1, %rdi              # stdout
    movq $no_msg, %rsi         # "NO\n"
    movq $3, %rdx              # длина (включая \n)
    syscall
    incq %r12                  # пропускаем разделитель
    jmp continue_reading
    
print_yes:
    movq $1, %rax              # sys_write
    movq $1, %rdi              # stdout
    movq $yes_msg, %rsi        # "YES\n"
    movq $4, %rdx              # длина (включая \n)
    syscall
    incq %r12                  # пропускаем разделитель
    jmp continue_reading
    
invalid_digit:
    movq %rsi, %r12           # сохраняем позицию после некорректного числа
    jmp print_no
    
exit:
    movq $60, %rax             # sys_exit
    xorq %rdi, %rdi            # код возврата 0
    syscall