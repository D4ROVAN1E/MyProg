#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <cstdint> 

using namespace std;

const int MAX_SPEED = 200; // Безопасный верхний предел для скорости
const int INF = 1e9;       // Значение, используемое как "бесконечность"

//Находит минимальное количество ходов для достижения следующей контрольной точки.
map<int, int> find_next_states(int start_pos, int end_pos, const map<int, int>& current_states) {
    queue<pair<int, int>> q;
    vector<vector<int>> min_moves(end_pos + 1, vector<int>(MAX_SPEED, -1));

    // Заполняем очередь начальными состояниями для текущего сегмента
    // ИСПРАВЛЕНО: Замена структурированной привязки на стандартный доступ к паре
    for (auto const& state_pair : current_states) {
        int speed = state_pair.first;
        int moves = state_pair.second;
        if (start_pos <= end_pos && speed < MAX_SPEED) {
            q.push({ start_pos, speed });
            min_moves[start_pos][speed] = moves;
        }
    }

    // Выполняем BFS
    while (!q.empty()) {
        pair<int, int> current_q_item = q.front();
        q.pop();
        int pos = current_q_item.first;
        int speed = current_q_item.second;

        int current_total_moves = min_moves[pos][speed];

        // --- Обработка изменения скорости и перемещения ---
        if (speed == 0) {
            int new_speed = 1;
            int new_pos = pos + new_speed;
            if (new_pos <= end_pos && min_moves[new_pos][new_speed] == -1) {
                min_moves[new_pos][new_speed] = current_total_moves + 1;
                q.push({ new_pos, new_speed });
            }
        }
        else {
            for (int dv = -1; dv <= 1; ++dv) {
                int new_speed = speed + dv;
                if (new_speed > 0 && new_speed < MAX_SPEED) {
                    int new_pos = pos + new_speed;
                    if (new_pos <= end_pos && min_moves[new_pos][new_speed] == -1) {
                        min_moves[new_pos][new_speed] = current_total_moves + 1;
                        q.push({ new_pos, new_speed });
                    }
                    else if (new_pos <= end_pos) {
                        min_moves[new_pos][new_speed] = min(current_total_moves + 1, min_moves[new_pos][new_speed]);
                        q.push({ new_pos, new_speed });
                    }
                }
            }
        }
    }

    map<int, int> next_states;
    for (int final_speed = 1; final_speed < MAX_SPEED; ++final_speed) {
        if (min_moves[end_pos][final_speed] != -1) {
            next_states[final_speed] = min_moves[end_pos][final_speed];
        }
    }
    return next_states;
}

int main() {
    int k;
    cin >> k;

    vector<int> checkpoints(k);
    for (int i = 0; i < k; ++i) {
        cin >> checkpoints[i];
    }

    map<int, int> states;
    states[0] = 0;

    int current_pos = 1;

    for (int target_pos : checkpoints) {
        if (current_pos == target_pos) {
            continue;
        }
        states = find_next_states(current_pos, target_pos, states);
        current_pos = target_pos;
    }

    int min_total_moves = INF;
    if (states.empty()) {
        min_total_moves = (k > 0) ? INF : 0;
    }
    else {
        // ИСПРАВЛЕНО: Замена структурированной привязки на стандартный доступ к паре
        for (auto const& state_pair : states) {
            min_total_moves = min(min_total_moves, state_pair.second);
        }
    }

    // Особый случай, если k=0, ответ 0.
    if (k == 0) {
        min_total_moves = 0;
    }


    cout << min_total_moves << endl;

    return 0;
}