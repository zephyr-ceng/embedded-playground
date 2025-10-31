#include "stdio.h"
#include "stdlib.h"
#include  "windows.h"

#define fileName "read.txt"

// 文件读取
char* read_File(const char *FailName)
{
    char buffer[256];
    FILE *file_p = fopen(fileName, "r+");
    if(file_p == NULL)
    {
        printf("文件打开错误\n");
        return 0;
    }
    while (fgets(buffer,sizeof(buffer),file_p) !=NULL)
    {
        printf("%s",buffer);
    }   
    fclose(file_p);
}

// 文件追加写入
char* Append_Write_File(const char *FailName)
{
    char buffer[50] = "追加写入文件测试内容\n";
    FILE *file_p = fopen(fileName, "a+");
    if(file_p == NULL)
    {
        printf("文件打开错误\n");
        return 0;
    }
    fputs(buffer,file_p);
    fclose(file_p);
}

// 文件覆盖写入
char* Cover_Write_File(const char *FailName)
{
    char buffer[50] = "覆盖写入文件\n";
    FILE *file_p = fopen(fileName, "w+");
    if(file_p == NULL)
    {
        printf("文件打开错误\n");
        return 0;
    }
    fputs(buffer,file_p);
    fclose(file_p);
}


int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    Cover_Write_File(fileName);
    Append_Write_File(fileName);
    read_File(fileName);
    return 0;
}