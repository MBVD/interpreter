#include <iostream>
int main() {
    int x = 1;
    int y;
    y ? x : y = x ? 1 : 2 ? 2 : 1 < x ? 2 : 1 < 3;
    // for (int i = 0; i < y < 3; i=i+1){}
    x +++ y;
    ++x + y;
    // std::cout<<(1 < 2 > 3);
    x = 1;
    y = 2;
    std::cout<< x ? 2 : 1 < 20; // сначала сделал тернарный потом сделал сравнение
}