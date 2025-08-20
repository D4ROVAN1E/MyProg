def predict_the_winner(nums):
    cache = {}
    
    def solve(left, right):
        if left == right:
            return nums[left]
        
        if (left, right) in cache:
            return cache[(left, right)]
        
        take_left = nums[left] - solve(left + 1, right)
        take_right = nums[right] - solve(left, right - 1)
        
        cache[(left, right)] = max(take_left, take_right)
        return cache[(left, right)]
    
    return solve(0, len(nums) - 1) >= 0

def main():
    n = int(input("Введите размер массива: "))
    
    print("Введите элементы массива: ", end="")
    nums = list(map(int, input().split()))
    
    if predict_the_winner(nums):
        print("true")
    else:
        print("false")

if __name__ == "__main__":
    main()