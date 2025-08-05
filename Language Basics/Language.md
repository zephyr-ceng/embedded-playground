## Language Node

### 32个关键字

- 数据类型：`auto char short int long float double  struct enum void union`  

- 修饰类： `signed unsigned static const volatile extern ` 

- 语句控制类：`if else for do while  goto switch case default continue break `

- 其他：`register typedef sizeof`

- 特别说明

  - `register`: 尽可能的将变量存于CPU内部寄存器，即不能通过内存寻址访问，所以不能使用取值运算符

  - `sizeof`: 是关键字，并非函数

  - `static`: 静态局部变量，在函数体里面定义的，就只能在这个函数里用了，同一个文档中的其他函数也用不了。由于被static 修饰的变量总是存在内存的静态区，所以即使这个函数运行结束，这个静态变量的值还是不会被销毁，函数下次使用时仍然能用到这个值

  - `switch case`: case后面使用整型或者字符类型（本质也是整型）

  - `extern`: 主要用于声明外部变量，初始化意味着申请内存，即定义；

    ```C
    // global.c
    int value = 20;
    
    // global.h
    extern int value;
    
    // .c
    #include "global.h"
    value++;
    ```

    

  - `struct`: 打包相关参数，CPU处理函数参数大于4个会降低效率。

  - `union`: 用法和`struct`使用相似，主要用于压缩空间，在同一时间不被同时用到的数据可以使用`union`,空间大小为最大长度的数据成员（此外还需考虑大端 【 高位字节存储在低地址 】 和小端【高位字节存储在高地址的影响】）

    ```C
    int CheckSystem()
    {
        union check{
            int i;
            char ch;
        } c;
        c.i = 1;
        printf()
        return (c.ch==1);
    }
    
    // main.c
    printf("System is %s-endian\n", checkSystem() ? "little" : "big"); // 判定首地址的值，大端模式时首地址为0；小端为1；
    ```

  - `enum`: 定义一组整型常量，常量用大写，常用于状态机，数组映射，通信协议命令码

    ​	

    ​	**const：**有类型检查、受作用域限制、安全性高。适合参数配置、常量变量。

    ​	**enum：**一组相关常量，自动递增，适合状态机、协议命令。

  - `typedef`: 类型别名，有类型检查，有作用域，

  - `#define：` 文本替换，最轻量、预处理替换、无类型检查。适合硬件寄存器、条件编译。

  

  ### 指针和数组

  - 基本指针
  - 空指针
  - 野指针
  - 指针和一维数组
  - 指针和二维数组
  - 指针和函数

  ​	

### 编码规则

### 使用技巧





