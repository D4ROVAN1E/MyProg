#include <iostream>
#include<string>
using namespace std;

int main(){
    string test;
    getline(cin, test);
    cout << test << endl;
    if (test[0] == 'П')
        cout << "Норм\n";
    else
        cout << "Не норм\n";
    return 0;
}
