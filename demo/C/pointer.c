// 选择性编译

#include <stdio.h>

#define DEBUGS ;

int main(int argc, char const *argv[])
{
#ifdef DEBUG
    printf("已经定义过");
#else
    printf("未定义");
#endif // DEBUG
    return 0;
}
