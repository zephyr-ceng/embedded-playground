# C语言补充

## 枚举

1. 枚举的概念：用于定义一组具有离散值的常量，它可以让数据更简洁，更易读

2. `enum Day{MON=1, TUE, WED, THU, FRI, SAT, SUN} `, 后面的默认加一

3. ```C
   // 直接定义变量，省略定义，可直接在.h文件中定义，导入该文件的都可以直接使用WeekDay定义枚举变量（范围为enum的定义范围）
   typedef enum{
         MON=1, TUE, WED, THU, FRI, SAT, SUN
   } WEEKDAY; // Day相当于别名
   
   WEEKDAY day = TUE; // 使用时把别名当新类型即可
   ```



## 结构体

1. 结构体概念：方便统一管理变量

2. `struct Person{char name[20],int age, bool gender,}`

3. ```C
   // C99的stidio.h中定义了布尔变量
   typedef struct {
   	char name[20],
   	int age,
   	bool gender,
   } PERSION;
   
   PERSION stu = {"James Weaver",25, True}; // 初始化变量
   ```

   

