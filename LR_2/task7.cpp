#include <iostream>
#include "lfu.hpp"

int main() {
    int cap, Q;
    cin >> cap >> Q;

    LFUCache<int> cache(cap);

    string command;
    int key, value;

    for (int i = 0; i < Q; i++) {
        cin >> command;
        if (command == "SET") {
            cin >> key >> value;
            cache.set(key, value);
        }
        else if (command == "GET") {
            cin >> key;
            int result = cache.get(key);
            cout << result << " ";
        }
    }
    cout << endl; 

    return 0;
}