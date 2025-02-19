#include <iostream>
#include <cstring>
using namespace std;


enum month {
    yiyue = 1,
    eryue = 2
};

int main(int argc, char const *argv[])
{
    month tem = month(2);
    switch (tem)
    {
    case 1:
        cout << tem << endl;
        break;
    
    default:
        break;
    }
    return 0;
}
