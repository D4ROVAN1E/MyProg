#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Функция для нахождения длины перекрытия между двумя словами
bool getOverlapLength(const string& s1, const string& s2) {
    int maxLength = 0;
    for (int i = 3; i < min(s1.length(), s2.length()); ++i) {
        if (s1.substr(s1.length() - i) == s2.substr(0, i)) {
            maxLength = i;
        }
    }
    return maxLength > 1;
}

// Рекурсивная функция для построения линейного кроссворда
void solveCrossword(vector<string>& currentChain, vector<string>& availableWords, vector<string>& bestChain) {
    if (currentChain.size() > bestChain.size()) {
        bestChain = currentChain;
    }

    for (size_t i = 0; i < availableWords.size(); ++i) {
        string nextWord = availableWords[i];
        bool canAdd = false;

        if (currentChain.empty()) {
            canAdd = true;
        } else {
            // Проверяем перекрытие с последним словом в цепочке
            string lastWord = currentChain.back();
            canAdd = (getOverlapLength(lastWord, nextWord));
        }

        if (canAdd) {
            // Добавляем слово в текущую цепочку
            currentChain.push_back(nextWord);
            // Удаляем слово из доступных, чтобы не использовать его повторно
            availableWords.erase(availableWords.begin() + i);

            solveCrossword(currentChain, availableWords, bestChain);

            // Возвращаем слово обратно (backtracking)
            availableWords.insert(availableWords.begin() + i, nextWord);
            currentChain.pop_back();
        }
    }
}

int main() {
    cout << "=== Линейный кроссворд ===" << endl;
    cout << "Введите слова через пробел (например, ЛОГИКА КАСКАД АДРЕС):" << endl;

    string line;
    getline(cin, line);
    istringstream is(line);
    vector<string> words;
    while (is >> line) {
        words.push_back(line);
    }

    if (words.empty()) {
        cout << "Нет слов для построения кроссворда." << endl;
        return 0;
    }

    vector<string> currentChain;
    vector<string> bestChain;

    solveCrossword(currentChain, words, bestChain);

    if (bestChain.size() < 2) {
        cout << "Не удалось построить цепочку слов." << endl;
    } else {
        cout << "Найденная цепочка:" << endl;
        for (size_t i = 0; i < bestChain.size(); ++i) {
            cout << bestChain[i];
            if (i < bestChain.size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl;
    }

    return 0;
}


