class VotingSystem:
    def __init__(self, candidates, voters):
        self.num_candidates = candidates
        self.num_voters = voters
        self.participants = []
        self.ballots = []
    
    def input_candidates(self):
        print("Введите имена кандидатов:")
        for i in range(self.num_candidates):
            candidate = input(f"Кандидат {i+1}: ").strip()
            self.participants.append(candidate)
    
    def input_votes(self):
        print("Введите предпочтения избирателей (от лучшего к худшему):")
        for i in range(self.num_voters):
            print(f"Избиратель {i+1}:")
            ballot = []
            for j in range(self.num_candidates):
                vote = input(f"  Позиция {j+1}: ").strip()
                ballot.append(vote)
            self.ballots.append(ballot)
    
    def calculate_borda_winner(self):
        points = [0] * self.num_candidates
        
        for ballot in self.ballots:
            for position in range(self.num_candidates):
                candidate = ballot[position]
                try:
                    candidate_index = self.participants.index(candidate)
                    points[candidate_index] += (self.num_candidates - 1 - position)
                except ValueError:
                    print(f"Внимание: кандидат '{candidate}' не найден в списке")
        
        max_points = max(points)
        winner_index = points.index(max_points)
        
        print("\nМетод Борда:")
        for i in range(self.num_candidates):
            print(f"{self.participants[i]}: {points[i]} баллов")
        print(f"Победитель: {self.participants[winner_index]}")
        
        return self.participants[winner_index]
    
    def calculate_condorcet_winner(self):
        wins = [[0 for _ in range(self.num_candidates)] for _ in range(self.num_candidates)]
        
        for i in range(self.num_candidates):
            for j in range(self.num_candidates):
                if i == j:
                    continue
                
                wins_count = 0
                for ballot in self.ballots:
                    try:
                        pos_i = ballot.index(self.participants[i])
                        pos_j = ballot.index(self.participants[j])
                        
                        if pos_i < pos_j:
                            wins_count += 1
                    except ValueError:
                        print(f"Внимание: кандидат не найден в бюллетене")
                
                if wins_count > self.num_voters // 2:
                    wins[i][j] = 1
        
        for i in range(self.num_candidates):
            is_condorcet_winner = True
            for j in range(self.num_candidates):
                if i != j and wins[i][j] == 0:
                    is_condorcet_winner = False
                    break
            
            if is_condorcet_winner:
                print(f"\nМетод Кондорсе:\nПобедитель: {self.participants[i]}")
                return self.participants[i]
        
        print("\nМетод Кондорсе:\nПобедителя нет")
        return ""
    
    def run_election(self):
        self.input_candidates()
        self.input_votes()
        
        borda_winner = self.calculate_borda_winner()
        condorcet_winner = self.calculate_condorcet_winner()
        
        if condorcet_winner and borda_winner != condorcet_winner:
            print("\nМетоды дали разные результаты!")

def show_test_cases():
    print("\nПримеры тестирования:")
    print("1. Единогласие: все голосуют A B C → A побеждает везде")
    print("2. Парадокс: A B C, B C A, C A B → нет победителя Кондорсе")
    print("3. Разногласия: возможны разные победители в методах")

def show_method_analysis():
    print("\nСравнение методов:")
    print("Борда: всегда есть победитель, учитывает все предпочтения")
    print("Кондорсе: наиболее справедлив, но может не найти победителя")
    print("Разные результаты при циклических предпочтениях")

def main():
    print("Система голосования (методы Борда и Кондорсе)")
    print("1 - Голосование, 2 - Тесты, 3 - Анализ: ", end="")
    
    try:
        choice = int(input())
        
        if choice == 1:
            candidates = int(input("Количество кандидатов: "))
            voters = int(input("Количество избирателей: "))
            
            election = VotingSystem(candidates, voters)
            election.run_election()
        elif choice == 2:
            show_test_cases()
        elif choice == 3:
            show_method_analysis()
        else:
            print("Неверный выбор")
    except ValueError:
        print("Ошибка: введите число")

if __name__ == "__main__":
    main()