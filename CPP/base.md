## CPP  BASE

1. 输入输出: 

   ```CPP
   #include <iosteam>
   
   int main (){
       cout << "hello world!" <<endl; // 输出
       char var[50];
       cin >> &var; // 获取输入
       cerr << "error msg: "<< endl; // 
       clog << "error msg: "<< endl;
   }
   ```

   

2. 头文件：``#include <iosream>`

3. 运算符，控制语句，数组 和C相同

   - 动态分配数组：`int *p = new (nothrow) int [1000]; delete[] p` nothrow 是为了防止内存不足

4. 字符串

   ```C
   char arr[255] = {0}; // 字符串初始化
   memset(arr,0,sizeof(arr)); // 清空字符串
   strcpy(arr,"hello"); // 赋值
   strncpy(arr,"world",4); // 使用前清空字符串
   ```

   

5. 函数

   - 值传递：不改变实参

   - 指针传递：使用`*` 符号

   - 引用传递： 使用`&`变符号量别名，会改变实参，用于函数传参

   - 数组传递时需要传递数组长度，且为指针表示

     ```cpp
     #include <iostream>
     #include <climits>
     // using namespace std;
     
     void swap(int &a, int &b)
     {
         int temp = a;
         a = b;
         b = temp;
     }
     
     void swap2(int a, int b)
     {
         int temp = a;
         a = b;
         b = temp;
     }
     
     
     
     int main(int argc, char const *argv[])
     {
         int a = 10, b = 20;
         std::cout << "交换前" << a << "------" << b << std::endl;
         swap2(a, b);
         std::cout << "交换后" << a << "-----" << b << std::endl;
         return 0;
     }
     ```

6. 时间和日期 

   - `#include <ctime>`

   ```CPP
   #include <iostream>
   #include <climits>
   using namespace std;
   #include <ctime>
   
   int main(int argc, char const *argv[])
   {
       time_t now = time(0);
       char *dt = ctime(&now);          // 把 now 转换为字符串形式
       tm *gmtm = gmtime(&now);         // tm 结构
       char *dtime = asctime(gmtm);     // UTC时间
       std::cout << dt << std::endl;    // Tue Dec 12 15:08:29 2023
       std::cout << dtime << std::endl; // Tue Dec 12 15:08:29 2023
   
       // 输出 tm 结构的各个组成部分
       tm *ltm = localtime(&now);
       cout << "年: " << 1900 + ltm->tm_year << endl;
       cout << "月: " << 1 + ltm->tm_mon << endl;
       cout << "日: " << ltm->tm_mday << endl;
       // 时分秒
       cout << "时间: " << ltm->tm_hour << ":";
       cout << ltm->tm_min << ":";
       cout << ltm->tm_sec << endl;
   
       /*
       年: 2023
       月: 12
       日: 12
       时间: 23:8:29
        */
       return 0;
   }
   ```

7. 堆栈

   堆：自动回收

   栈：可以灵活管理，new申请，delete 释放内存

   ```CPP
   // new 申请内存
   // delete 释放内存
   main (){
   	int *p = new int(5);
   	cout << "*p=" << *p<< endl;
   	
   	*p = 8;
   	cout << "*p = "<< *p<< endl;
   	delete p; // 释放内存
   }
   ```

8. 指针

   - 空指针：0 or NULL 表示空指针，**空指针无法解引用**，内存释放后，指针要指向空

   - 野指针：指针无初始化（初始化为nullptr）；指针指向了动态分配的内存(置空)；指针指向作用域内的局部变量（不返回局部变量）；数组越界

9. 结构体  同C语言

   ```CPP
   #include <iostream>
   #include <cstring>
   
   using namespace std;
   void printBook(struct Books *book);
   
   struct Books
   {
       char title[50];
       char author[50];
       char subject[100];
       int book_id;
   };
   
   int main()
   {
       Books Book1; // 定义结构体类型 Books 的变量 Book1
       Books Book2; // 定义结构体类型 Books 的变量 Book2
   
       // Book1 详述
       strcpy(Book1.title, "C++ 教程");
       strcpy(Book1.author, "Runoob");
       strcpy(Book1.subject, "编程语言");
       Book1.book_id = 12345;
   
       // Book2 详述
       strcpy(Book2.title, "CSS 教程");
       strcpy(Book2.author, "Runoob");
       strcpy(Book2.subject, "前端技术");
       Book2.book_id = 12346;
   
       // 通过传 Book1 的地址来输出 Book1 信息
       printBook(&Book1);
   
       // 通过传 Book2 的地址来输出 Book2 信息
       printBook(&Book2);
   
       return 0;
   }
   // 该函数以结构指针作为参数
   void printBook(struct Books *book)
   {
       cout << "书标题  : " << book->title << endl;
       cout << "书作者 : " << book->author << endl;
       cout << "书类目 : " << book->subject << endl;
       cout << "书 ID : " << book->book_id << endl;
   }
   ```

   ```C
   // 结构体-动态分配内存
   #include <iostream>
   #include <cstring>
   using namespace std;
   
   
   struct st_t
   {
       int a;
       int *p;
   };
   
   int main(int argc, char const *argv[])
   {
       st_t stt;
       memset(&stt,0,sizeof(st_t));
       stt.p = new int[100];
   
       stt.a = 0; // 清空结构体成员
       memset(stt.p,0,100*sizeof(int)); // 释放结构体动态内存
       cout << sizeof(stt) << endl;
       cout << sizeof(stt.a) << endl;
       cout << sizeof(stt.p) << endl;
       return 0;
   }
   ```

   

10. 面向对象

   - 类基本定义

     ```C
     // base.h 类的声明
     #include <iostream>
     using namespace std;
     
     class base
     {
     private:
         int value;
     
     public:
         base();        // 不带参数的构造函数
         base(int a);   // 带参数的构造函数
         void show(); // 成员方法
         ~base();       // 析构函数，释放内存空间
     };
     ```

     ```CPP
     // base.cpp  类方法的实现
     #include "../header/base.h"
     
     // 构造函数
     base::base(){};
     base::base(int a)
     {
         value = a;
         std::cout << "初始化" << std::endl;
     };
     // 成员方法
     void base::show()
     {
         cout << "hello world" <<endl;
     }
     
     // 析构函数
     base::~base()
     {
         std::cout << "已销毁" << std::endl;
     }
     ```

     ```CPP
     // main 主函数
     #include "base.cpp"
     
     int main(int argc, char const *argv[])
     {
         base s(12);
         s.show();
         return 0;
     }
     ```

     