#include <iostream>
#include <stdlib.h>
#include <typeinfo>

class gorinych{
    public:
    unsigned short head_count = 1;

    int cut_all_heads(){
        int choped = head_count;
        head_count *= 2;
        return choped;
    }

    bool alive(){
        if(head_count == 0) return 0;
        return 1;
    }
};

int main(){
    gorinych zmei;

    while(zmei.alive()){
        std::cout << zmei.cut_all_heads() << "golov otrubleno!\n";
    }

    std::cout << "zmei umer(\n";

    return 0;
}