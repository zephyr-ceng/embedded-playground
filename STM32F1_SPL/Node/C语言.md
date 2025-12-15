# C语言补充

## 32个关键字

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
    int value = 20; // 初始化后才分配内存空间
    
    // global.h
    extern int value; // 此处未分配内存空间
    
    // .c
    #include "global.h"
    value++;
    ```

    

  - `struct`: 打包相关参数，CPU处理函数参数大于4个会降低效率。

    结构体概念：方便统一管理变量`struct Person{char name[20],int age, bool gender,}`

    ```c
    // C99的stidio.h中定义了布尔变量
    typedef struct {
    	char name[20],
    	int age,
    	bool gender,
    } PERSION;
    
    PERSION stu = {"James Weaver",25, True}; // 初始化变量
    ```

    

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

    - 枚举的概念：用于定义一组具有离散值的常量，它可以让数据更简洁，更易读
    - `enum Day{MON=1, TUE, WED, THU, FRI, SAT, SUN} `, 后面的默认加一

    ```c
    // 直接定义变量，省略定义，可直接在.h文件中定义，导入该文件的都可以直接使用WeekDay定义枚举变量（范围为enum的定义范围）
    typedef enum{
          MON=1, TUE, WED, THU, FRI, SAT, SUN
    } WEEKDAY; // Day相当于别名
    
    WEEKDAY day = TUE; // 使用时把别名当新类型即
    ```

    

  - `const`：有类型检查、受作用域限制、安全性高。适合参数配置、常量变量。**修饰谁，谁不可变**

  - `typedef`: 类型别名，有类型检查，有作用域，

  - `#define：` 文本替换，最轻量、预处理替换、无类型检查。适合硬件寄存器、条件编译。

  - `volatile` 易变的，会被程式所改变，所以禁止优化处理

    ```C
    int flag =0;
    // 可能会被优化为死循环
    while(flag==0){
        // code
    }
    ```

## 指针和数组

- 基本指针

  - 概念：指针是存储内存地址的变量，其本质就是保存某块内存单元起始地址的变量

  - 例如：`int *p = &a;` 表示指针 `p` 指向整型变量 `a` 的内存地址；通过解引用操作（如`*p`），可以访问该地址处存储的值。指针还可以指向其他指针、数组、函数或动态分配的内存

    

- 数组

  - 一维数组：**相同类型**的元素组成的**有序**集合，这些元素在内存中**连续存储**，并通过**索引**（下标）来访问

  - 二维数组：数组的数组，例如 `int arr[M][N]` 定义了一个包含 M 个长度为 N 的一维数组的数组。它在内存中分配了连续的 M×N 个元素

  - 动态数组：设置行列数后自动分配内存

    ```C
    int rows = 3, cols = 4;
    int **arr = malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++)
        arr[i] = malloc(cols * sizeof(int));
    
    func(arr, rows, cols);
    ```

    

- 指针和数组

  - 数组指针：

    ```C
    // 一维数组指针
    int one_Array = {1,2,3,4,5}; // arr类型为int[4] 会退化为指向一个元素
    int *p_one = &one_Array[0]; // 也可以写为int *p = arr;为了避免歧义
    
    // 二维数组指针
    int two_Array[3][4] = {{6,7,8,9},{10,11,12,13},{14,15,16,17}}; // two_Array 会退化为指向一行元素
    int *p_two = &two_Array[0][0]; // 平铺表示法,p+1表示移动一个int,获取到下一个元素
    int (*p_two)[4] = &two_Array[0];  // 按行表示法，p2+1 表示移动4个int,获取到下一行元素
    
    // 其他
    int (*p)[5] = &one_Array; // 一维表示法
    int (*p)[3][4] = &two_Array; // 二维表示法
    
    //注意事项
    int arr = {3,4,5}
    int *p = &arr; // 类型不匹配，
    int *p = arr[0]; // 非法，相当于常量赋值给了指针
    ```

    

  - 指针数组：**存放指针的数组,`int *pArr[3]`表示存放`int`类型的3个指针的数组

    ```c
    int a=10, b=20, c=30;
    int *ptrArr[3];     // 定义一个包含3个int型指针的数组
    ptrArr[0] = &a;
    ptrArr[1] = &b;
    ptrArr[2] = &c;
    for(int i=0; i<3; i++){
        printf("%d ", *(ptrArr[i])); // 通过指针数组访问各变量的值:contentReference[oaicite:28]{index=28}
    }
    ```

    

  - 数组与指针也存在明显区别：**内存布局不同**——数组在内存中占据一段连续的字节空间`sizeof(arr)`，而指针仅占固定大小的存储单元（例如32位系统为4字节）`sizeof(p)`；**可赋值性不同**——指针变量是普通变量，可以被重新赋值指向不同地址；而数组名在语义上不是变量，不能被赋值，只能在定义时指定地址。**使用场景不同**——数组名用于定义内存块并通过索引访问元素，指针用于动态访问和操作内存。

    

  - **指针的注意事项：野指针，空指针，内存溢出，非法指针**

    - `void *p` 表示不确定类型的指针，可以存放任何类型的指针，**所以不能直接解引用，需转换为具体类型的指针**【类型转换需注意大小】

    - 空指针：**空指针**（`NULL`）是被显式赋值为 `NULL` 的指针，不指向任何有效地址

    - 野指针：**野指针**（Dangling Pointer）指向不确定或已释放的内存地址，对未初始化指针解引用或对已 `free` 后的内存继续访问都会产生野指针

    - 非法指针：指指向没有访问权限或已经释放掉内存的指针，如局部变量地址外部使用

      

  - **二维数组在函数中的传递(核心也是指针的退化)**

    ```c
    // 固定列(列数固定)传递 void func(int [][4],int rows); 调用 func(arr,3)
    void func(int (*arr)[4],int rows){
         for (int i = 0; i < rows; i++)
            for (int j = 0; j < 4; j++)
                printf("%d ", arr[i][j]);
    }
    
    // 平铺传递
    void func(int *arr, int rows, int cols) {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                printf("%d ", arr[i*cols+j]); // 或者*(arr+i*cols+j)
    } // 调用 func(&arr[0][0],3,4);
    
    // 指向指针
    void func(int **arr, int rows, int cols);
    ```

    

## 指针和函数

- 函数指针：是指向函数的指针变量。函数本身存在地址，因此可以将函数的地址赋给相应类型的指针变量。

  ```c
  int max(int x, int y) { return x>y ? x : y; }
  int (*fp)(int,int) = max;  // 定义函数指针 fp 指向 max 函数
  int result = fp(3, 5);     // 通过函数指针调用 max，结果为 5:contentReference[oaicite:35]{index=35}  
  
  // 回调函数即函数指针
  // 嵌入式写法
  typedef void  (*callback_t)(int *arr); // .h 定义函数格式
  
  static callback_t callback_cb = NULL; // .c 定义全局变量指针
  // 注册回调函数
  void RegisterCallback(callback_t cb){
      callback_cb = cb;
  }
  // 调用注册函数
  if(callback_cb != NULL)
      callback_cb(array);
  
  // 由用户提供回调函数
  void function_Name(int *arr){
      print("%d",array[0]);
  }
  RegisterCallback(function_Name); // 注册
  
  
  
  ```

  

- 指针函数：返回指针的函数`int* func(int x)`

​	

## 编码规则



## 使用技巧





