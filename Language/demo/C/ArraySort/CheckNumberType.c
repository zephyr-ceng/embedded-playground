#include "stdio.h"
#include "math.h"
/**
 * @brief  判断一个数是否为素数，素数：只能被1和本身整除的数
 * @param  num:整数
 * @retval 1:是素数 0:不是素数
 * */
int is_Prime(int num)
{
    int cnt = 0;
    if (num <= 1)
        return 0;
    for (int i = 2; i < num; i++)
    {
        if (num % i == 0)
            cnt++;
        return 0;
    }
    if (cnt == 0)
        return 1;
    else
        return 0;
}

/**
 * @brief  复利计算
 * @note   复利计算公式：A = P(1 + r/n)^(nt)
 * @param  principal:本金
 * @param  rate:年利率
 * @param  times:每年复利次数
 * @param  years:投资年限
 * @retval 最终金额A
 * */
double compound_Interest(double principal, double rate, int times, int years)
{
    return principal * pow((1 + rate / times), times * years);
}

// 交换变量值
int swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
    return 0;
}

// 不使用临时变量交换变量值
int swap_NoTemp(int *a, int *b)
{
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
    return 0;
}

// 将整数中的0转换为1
int convert_ZeroToOne(int *num)
{
    int temp = *num;
    int result = 0;
    int place = 0;
    while (temp > 0)
    {
        if (temp % 10 == 0)
            result = result + 1 * pow(10, place);
        else
            result = result + (temp % 10) * pow(10, place);
        place++;
        temp = temp / 10;
    }
}

// 二进制转十进制
int binaryToDecimal(int binary){
    int decimal = 0;
    int base = 1; //2^0
    while(binary !=0){
        decimal += (binary % 10) *base;
        base *=2;
        binary /=10;
    }
    return decimal;
}