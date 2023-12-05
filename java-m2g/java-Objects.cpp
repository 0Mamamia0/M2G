//
// Created by Admin on 2023/12/4.
//

#include <atomic>
namespace objects {
    std::atomic<int> num;


    void increase() {
        num ++;
    }

    void decrease() {
        num --;
    }

    int count() {
        return num.load();
    }
}



