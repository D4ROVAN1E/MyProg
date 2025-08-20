#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>

using namespace std;

class VotingSystem {
private:
    vector<string> participants;
    vector<vector<string>> ballots;
    int numCandidates;
    int numVoters;

public:
    VotingSystem(int candidates, int voters) : numCandidates(candidates), numVoters(voters) {
        participants.resize(candidates);
        ballots.resize(voters);
    }

    void inputCandidates() {
        cout << "Введите имена кандидатов:\n";
        for (int i = 0; i < numCandidates; i++) {
            cin >> participants[i];
        }
    }

    void inputVotes() {
        cout << "Введите предпочтения избирателей (от лучшего к худшему):\n";
        for (int i = 0; i < numVoters; i++) {
            ballots[i].resize(numCandidates);
            for (int j = 0; j < numCandidates; j++) {
                cin >> ballots[i][j];
            }
        }
    }

    string calculateBordaWinner() {
        vector<int> points(numCandidates, 0);

        for (const auto& ballot : ballots) {
            for (int position = 0; position < numCandidates; position++) {
                string candidate = ballot[position];
                auto it = find(participants.begin(), participants.end(), candidate);
                int candidateIndex = distance(participants.begin(), it);
                points[candidateIndex] += (numCandidates - 1 - position);
            }
        }

        int maxPoints = *max_element(points.begin(), points.end());
        int winnerIndex = distance(points.begin(),
            find(points.begin(), points.end(), maxPoints));

        cout << "\nМетод Борда:\n";
        for (int i = 0; i < numCandidates; i++) {
            cout << participants[i] << ": " << points[i] << " баллов\n";
        }
        cout << "Победитель: " << participants[winnerIndex] << "\n";

        return participants[winnerIndex];
    }

    string calculateCondorcetWinner() {
        vector<vector<int>> wins(numCandidates, vector<int>(numCandidates, 0));

        for (int i = 0; i < numCandidates; i++) {
            for (int j = 0; j < numCandidates; j++) {
                if (i == j) continue;

                int winsCount = 0;
                for (const auto& ballot : ballots) {
                    int posI = distance(ballot.begin(),
                        find(ballot.begin(), ballot.end(), participants[i]));
                    int posJ = distance(ballot.begin(),
                        find(ballot.begin(), ballot.end(), participants[j]));

                    if (posI < posJ) {
                        winsCount++;
                    }
                }

                if (winsCount > numVoters / 2) {
                    wins[i][j] = 1;
                }
            }
        }

        for (int i = 0; i < numCandidates; i++) {
            bool isCondorcetWinner = true;
            for (int j = 0; j < numCandidates; j++) {
                if (i != j && wins[i][j] == 0) {
                    isCondorcetWinner = false;
                }
            }

            if (isCondorcetWinner) {
                cout << "\nМетод Кондорсе:\nПобедитель: " << participants[i] << "\n";
                return participants[i];
            }
        }

        cout << "\nМетод Кондорсе:\nПобедителя нет\n";
        return "";
    }

    void runElection() {
        inputCandidates();
        inputVotes();

        string bordaWinner = calculateBordaWinner();
        string condorcetWinner = calculateCondorcetWinner();

        if (!condorcetWinner.empty() && bordaWinner != condorcetWinner) {
            cout << "\nМетоды дали разные результаты!\n";
        }
    }
};

void showTestCases() {
    cout << "\nПримеры тестирования:\n";
    cout << "1. Единогласие: все голосуют A B C → A побеждает везде\n";
    cout << "2. Парадокс: A B C, B C A, C A B → нет победителя Кондорсе\n";
    cout << "3. Разногласия: возможны разные победители в методах\n";
}

void showMethodAnalysis() {
    cout << "\nСравнение методов:\n";
    cout << "Борда: всегда есть победитель, учитывает все предпочтения\n";
    cout << "Кондорсе: наиболее справедлив, но может не найти победителя\n";
    cout << "Разные результаты при циклических предпочтениях\n";
}

int main() {
    setlocale(LC_ALL, "ru");
    cout << "Система голосования (методы Борда и Кондорсе)\n";
    cout << "1 - Голосование, 2 - Тесты, 3 - Анализ: ";

    int choice;
    cin >> choice;

    switch (choice) {
    case 1: {
        int candidates, voters;
        cout << "Количество кандидатов: ";
        cin >> candidates;
        cout << "Количество избирателей: ";
        cin >> voters;

        VotingSystem election(candidates, voters);
        election.runElection();
        break;
    }
    case 2:
        showTestCases();
        break;
    case 3:
        showMethodAnalysis();
        break;
    default:
        cout << "Неверный выбор\n";
    }

    return 0;
}