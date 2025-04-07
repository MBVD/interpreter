int main() {
    int x = 1;
    int y = x ? 1 : 2 ? 2 : 1 < x ? 2 : 1 < 3;
    for (int i = 0; i<3; i=i+1){}
    x +++ y;
    ++x + y;
}