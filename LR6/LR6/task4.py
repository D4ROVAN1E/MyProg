from collections import deque

MAX_SPEED = 200  # Безопасный верхний предел для скорости
INF = 10**9      # Значение, используемое как "бесконечность"

def find_next_states(start_pos, end_pos, current_states):
    """Находит минимальное количество ходов для достижения следующей контрольной точки."""
    q = deque()
    min_moves = [[-1] * MAX_SPEED for _ in range(end_pos + 1)]

    # Заполняем очередь начальными состояниями для текущего сегмента
    for speed, moves in current_states.items():
        if start_pos <= end_pos and speed < MAX_SPEED:
            q.append((start_pos, speed))
            min_moves[start_pos][speed] = moves

    # Выполняем BFS
    while q:
        pos, speed = q.popleft()
        
        current_total_moves = min_moves[pos][speed]

        # Обработка изменения скорости и перемещения
        if speed == 0:
            new_speed = 1
            new_pos = pos + new_speed
            if new_pos <= end_pos and min_moves[new_pos][new_speed] == -1:
                min_moves[new_pos][new_speed] = current_total_moves + 1
                q.append((new_pos, new_speed))
        else:
            for dv in range(-1, 2):  # от -1 до 1 включительно
                new_speed = speed + dv
                if new_speed > 0 and new_speed < MAX_SPEED:
                    new_pos = pos + new_speed
                    if new_pos <= end_pos and min_moves[new_pos][new_speed] == -1:
                        min_moves[new_pos][new_speed] = current_total_moves + 1
                        q.append((new_pos, new_speed))
                    elif new_pos <= end_pos:
                        min_moves[new_pos][new_speed] = min(current_total_moves + 1, min_moves[new_pos][new_speed])
                        q.append((new_pos, new_speed))

    next_states = {}
    for final_speed in range(1, MAX_SPEED):
        if min_moves[end_pos][final_speed] != -1:
            next_states[final_speed] = min_moves[end_pos][final_speed]
    
    return next_states

def main():
    k = int(input())
    
    checkpoints = []
    for _ in range(k):
        checkpoints.append(int(input()))

    states = {0: 0}
    current_pos = 1

    for target_pos in checkpoints:
        if current_pos == target_pos:
            continue
        states = find_next_states(current_pos, target_pos, states)
        current_pos = target_pos

    if not states:
        min_total_moves = INF if k > 0 else 0
    else:
        min_total_moves = min(states.values())

    # Особый случай, если k=0, ответ 0.
    if k == 0:
        min_total_moves = 0

    print(min_total_moves)

if __name__ == "__main__":
    main()