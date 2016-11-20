#include "LCB.h"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#define BASE 100000000 //система счисления
#define SIGNS 8
#define MIN_LENGTH_FOR_KARATSUBA 2 //числа короче умножаются квадратичным алгоритмом
typedef long long digit; //взят только для разрядов числа

//#define unsigned long long 'ull'

using namespace std;

//тип для длинных чисел
struct bignum {
  digit *values; //массив с цифрами числа записанными в обратном порядке
  long long length, floatpoint = 0; //длинна числа
};

void Dump(bignum a){
    cout << a.values << "\n";
    cout << a.length << "\n" << a.floatpoint << "\n";
};

void bigLog(string a, string s , bignum b){
    cout << "log___" << a << "___" << s << "! ";
    for (unsigned int v=0; v<b.length;v++){
        if ((v==b.floatpoint) && (b.floatpoint!=0)) cout << ".";
        cout << b.values[v] << " ";
    }
    cout << "\n";
}

void bigNul(bignum a){
    for (unsigned int v=0; v<a.length;v++) a.values[v] = 0;
}

bignum StrToBig(string s){
    bignum c;//-результат

    //Поиск точки
    //c.floatpoint = 0 - дроби нет, иначе указывает, сколько цифр стоит после точки
    for(long long i = s.length()-1; i >= 0; i--){
        if ((s[i]=='.') || (s[i]==',')) c.floatpoint = s.length()-1-i;
    }

    //Задаем знак
    int sign;//Принимает значения +-1
    int signplace = 0;//signplace - 1, если есть знак, иначе 0

    if(s[0] == '-'){
            sign = -1;
            signplace=1;
        }
        else{
            sign = 1;
            if(s[0] == '+'){
                signplace=1;
            }
        }

    //Ищем длину bignum'a
    unsigned long long k,buf;
    buf = s.length()-signplace; //buf - кол-во цифр
    if (c.floatpoint>0) buf = buf - c.floatpoint - 1; //buf - кол-во цифр в целой части
    //cout << buf << "_\n";
    k = (buf)/SIGNS;
    if (buf % SIGNS > 0) k++;
    k += (c.floatpoint)/SIGNS;
    if (c.floatpoint % SIGNS > 0) k++;
    c.length = k;

    //cout << "H" << c.length << "\n";

    //Обнуляем
    c.values = new digit[c.length];
    bigNul(c);
    /*
    unsigned long long count = 0;
    for(unsigned long long i = 0; i < c.length; i++){
        c.values[i] = 0;
    }
    */

    long long pow,backofstr,t = s.length()-1;// pow - степень 10, t - указатель на посл. цифру целой части
                                            //backofstr - указатель на посл. цифру
    backofstr = t;
    if (c.floatpoint != 0) t=t-c.floatpoint-1;

    //Заполняем дробную часть
    k = SIGNS - c.floatpoint % SIGNS; //k кол-во недостающих цифр для полного заполнения первой пачки
    if (k==SIGNS) k = 0;//на случай если кол-во дробных цифр делится на SIGNS без остатка

    pow = 1;
    for(long long i = 1; i <= k; i++) pow = pow * 10;
    unsigned long long current = 0;
    for(long long i = backofstr; i > t+1; i--){
        c.values[current] = c.values[current] + (s[i] - '0') * pow;
        pow = pow * 10;
        if((backofstr-i+1+k) % SIGNS == 0){
            //переход на новую "пачку" чисел
            //c.values[current] = c.values[current] * sign;
            current++;
            pow = 1;
        }
    }

    //Заполняем целую часть
    pow = 1;
    for(long long i = t; i >= signplace; i--){
        c.values[current] = c.values[current] + (s[i] - '0') * pow;
        pow = pow * 10;
        if((t-i+1) % SIGNS == 0){
            //переход на новую "пачку" чисел
            //c.values[current] = c.values[current] * sign;
            current++;
            pow = 1;
        }
    }

    //Добавляем знак в число
    for(long long i = 0; i < c.length; i++){
        c.values[i] = c.values[i] * sign;
    }

    //!!!Теперь c.floatpoint - номер "пачки" с которой начинается целая часть
    if (c.floatpoint % SIGNS == 0) c.floatpoint = c.floatpoint/SIGNS;
    else c.floatpoint = c.floatpoint/SIGNS + 1;


    //cout << "STF float=" << c.floatpoint << "\n";
    bigLog("StrToBig","End",c);
    //system("pause");
    return c;
}

string BigToStr(bignum a){
    string res;//res1 - конечный результат

    //Пишем знак, если надо
    int signplace = 0;
    for (long long i = 0; i <= a.length-1; i++)
        if (a.values[i]<0){
            signplace = 1;
            break;
        }

    //cout << a.length << "L\n";

    //Находим длину строки без лишних 0 впереди
    long long k = a.length-1;//k - указатель на последнюю пачку
    unsigned long long LengthOfRes = a.floatpoint * SIGNS + 1;//искомая длина строки
    int trigger=0;
    while ((k >= a.floatpoint) && (trigger==0)){
        if (a.values[k]!=0){
            //cout << a.values[k] << " " << k << "!!!\n";
            trigger = 3;
            LengthOfRes = SIGNS * k;
            long long i = 1;
            long long buf = a.values[k];
            while ((i<=SIGNS) && (trigger == 3)){
                LengthOfRes++;
                if (buf == 0){
                    trigger = 1;
                    LengthOfRes--;
                }
                buf = buf / 10;
                i++;
            }
        }
        k--;
    }

    //cout << "K" << LengthOfRes << "\n";

    LengthOfRes += signplace;
    if (a.floatpoint>0) LengthOfRes++;

    //cout << "P" << LengthOfRes << "\n";

    //Находим кол-во не значащих 0 в дроби
    k = 0;//k - указатель на первую пачку
    unsigned long long NullsInFloat = a.floatpoint * SIGNS;//искомое кол-во нулей
    trigger=0;
    while ((k < a.floatpoint) && (trigger==0)){
        if (a.values[k]!=0){
            trigger = 3;
            NullsInFloat = SIGNS * k;
            long long i = 1;
            long long buf = a.values[k];
            while ((i<=SIGNS) && (trigger == 3)){
                //cout << a.values[k] << " " << NullsInFloat << "\n";
                NullsInFloat++;
                if (buf%10 != 0){
                    trigger = 1;
                    NullsInFloat--;
                }
                buf = buf / 10;
                i++;
            }
        }
        k++;
    }

    //Задаем длину строки
    k=LengthOfRes - NullsInFloat;
    if ( (NullsInFloat==a.floatpoint*SIGNS) && (a.floatpoint!=0) )k--;
    res.resize(k);

    /*
    cout << "FLT" << a.floatpoint << "\n";
    cout << "LGT" << k << "\n";
    cout << "NlS" << NullsInFloat << "\n";
    cout << "LOR" << LengthOfRes << "\n";
    //*/

    //Вводим знак, если надо
    //res1.resize(res.length());
    if (signplace == 1) res[0]='-';

    //k = res.length();// k - указатель на место в строке

    //заполняем res дробью
    //((a.floatpoint>0) &&
    //избавляемся от лишних 0
    //k = 0;
    if (NullsInFloat!=a.floatpoint*SIGNS){
        long long StartOfFloat = NullsInFloat / SIGNS; //номер пачки с которой начинаем считывать дробь
        for(long long i = 1; i <= NullsInFloat % SIGNS; i++)
                a.values[StartOfFloat] = a.values[StartOfFloat]/10;
        //cout << "MIN" << a.values[StartOfFloat] << "\n";
        //k - указатель на место в строке
        long long buf = res.length() + NullsInFloat;
        for (long long i=StartOfFloat; i<a.floatpoint; i++){
            a.values[i]=abs(a.values[i]);
            do{
                k--;
                res[k]=a.values[i] % 10 + '0';
                a.values[i] = a.values[i] / 10;
            } while ((buf - k) % SIGNS != 0);
        }
        k--;
        res[k]='.';
    }

    //cout << NullsInFloat << "n " << k << "\n";
    //cout << res << "\n";

    //заполняем целую часть
    long long buf = res.length();
    if (NullsInFloat!=a.floatpoint*SIGNS){
            buf += NullsInFloat;
            buf--;
    }
    for (long long i=a.floatpoint;(i<a.length) && (k>signplace); i++){
        a.values[i]=abs(a.values[i]);
        do{
            k--;
            res[k]=a.values[i] % 10 + '0';
            a.values[i] = a.values[i] / 10;
        } while ((k>signplace) && ((buf - k) % SIGNS != 0) );
    }

    //system("pause");
    return res;
}

unsigned long long GetBigLength(bignum l){
    unsigned long long NewLength = 0;
    for (unsigned long long i = 0; i<l.length; i++){
        if(l.values[i]!=0) NewLength = i+1;
    }
    if (NewLength==0) NewLength++;
    return NewLength;
}

//Нормализация числа
void normalize(bignum l) {
    unsigned long long Newlength = 0;
    Newlength = GetBigLength(l);

    int sign;
    if(l.values[Newlength-1]<0) sign=-1;
    else sign=1;
    if (sign<0){
        for (unsigned long long i = 0; i <=l.length-1; i++){
            l.values[i] = l.values[i] * sign;
        }
    }

    for (unsigned long long i = 0; i < l.length - 1; i++) {
        if (l.values[i] >= BASE) { //если число больше максимального, то организовавается перенос
            digit carryover = l.values[i] / BASE;
            l.values[i + 1] += carryover;
            l.values[i] -= carryover * BASE;
        } else if (l.values[i] < 0) { //если меньше - заем
            digit carryover = (l.values[i] + 1) / BASE - 1;
            l.values[i + 1] += carryover;
            l.values[i] -= carryover * BASE;
        }
    }

    if (sign<0){
            for (unsigned long long i = 0; i <= l.length-1; i++){
                l.values[i] = l.values[i] * sign;
            }
    }
}

//Аналог memcpy в Windows для bignum
//bignum_memcpy(куда, что, откуда начинать)
//void bignum_memcpy(bignum &a, bignum &b, unsigned long long lengthA,unsigned long long lengthB)
void bignum_memcpy(bignum &a, bignum &b, unsigned long long Start){
    for (unsigned long long i=0; (i < b.length) && (Start < a.length); i++){
        a.values[Start]=b.values[i];
        Start++;
    }
}

//Сложение, a>b, если они целые
bignum Add(bignum a, bignum b) {
    bignum c;
    if ( (a.floatpoint==0) && (b.floatpoint==0) ){
        c.length = a.length + 1;
        c.values = new digit[c.length];
        bigNul(c);

        c.values[a.length] = 0;
        for (unsigned long long i = b.length; i < a.length; i++) c.values[i] = a.values[i];

        for (unsigned long long i = 0; i < b.length; ++i) c.values[i] = a.values[i] + b.values[i];
    }
    else{
        c.floatpoint = max(a.floatpoint,b.floatpoint);
        long long x,y;
        x=a.length-a.floatpoint;
        y=b.length-b.floatpoint;
        c.length = c.floatpoint + max(x,y) + 1;
        c.values = new digit[c.length];
        bigNul(c);

        //Дробь
        if (a.floatpoint > b.floatpoint){
            for (long long i=0; i < c.floatpoint; i++) c.values[i]=a.values[i];
            long long buf = c.floatpoint-b.floatpoint;
            for (long long i=0; i < c.floatpoint; i++) c.values[buf+i] += b.values[i];
        }
        else{
            for (long long i=0; i < c.floatpoint; i++) c.values[i]=b.values[i];
            long long buf = c.floatpoint-a.floatpoint;
            for (long long i=0; i < c.floatpoint; i++) c.values[buf+i] += a.values[i];
        }

        //Целое
        if (x > y){
            for (long long i=0; i < x; i++) c.values[c.floatpoint+i]=a.values[a.floatpoint+i];
            for (long long i=0; i < y; i++) c.values[c.floatpoint+i]+=b.values[b.floatpoint+i];
        }
        else{
            for (long long i=0; i < y; i++) c.values[c.floatpoint+i]=b.values[b.floatpoint+i];
            for (long long i=0; i < x; i++) c.values[c.floatpoint+i]+=a.values[a.floatpoint+i];
        }
    }
    normalize (c);
    return c;
}

//Вычитание
bignum &Sub(bignum &a, bignum b) {
    for (unsigned long long i = 0; i < b.length; ++i)
        a.values[i] -= b.values[i];
    return a;
}

//3 Ф-ции для работы с отр произв/делением
//Показывает знак bignum'a, выводит +1/-1
int GetSign(bignum a){
    int sign = 1;
    if (a.values[a.length-1] < 0) sign = -1;
    return sign;
}

//Кидает знак в частное/произведение
void SetSign(bignum a, int sign){
    for (unsigned long long i = 0; i<a.length; i++){
        a.values[i] = a.values[i] * sign;
    }
}

//Abs() для bignum
void bignum_Abs(bignum a){
    for (unsigned long long i = 0; i<a.length; i++){
        a.values[i] = abs(a.values[i]);
    }
}

//Умножение методом Каратсубы
bignum karatsuba(bignum a, bignum b) {
    bignum product; //результирующее произведение
    product.length = a.length + b.length;
    product.values = new digit[product.length];

    bigNul(product);

    cout << "START MUL\n";
    bigLog("Mul","StartA",a);
    bigLog("Mul","StartB",b);

    if (a.length < MIN_LENGTH_FOR_KARATSUBA) { //если число короче то применять наивное умножение
        //memset(product.values, 0, sizeof(digit) * product.length);
        if (a.length > 0){
            for (long long i = 0; i < b.length; ++i){
                product.values[i] = a.values[0] * b.values[i];
            }
        }
    } else { //умножение методом Карацубы

        bignum a_part1; //младшая часть числа a
        a_part1.values = a.values;
        a_part1.length = (a.length + 1) / 2;

        //a_part1.length - Одна из длин, которая понадобится нам
        //в неизменном виде. Она будет служить необходима для записи числа в результат
        long long ForPrintBase = a_part1.length;//Не измененная копия

        bignum a_part2; //старшая часть числа a
        a_part2.values = a.values + a_part1.length;
        a_part2.length = a.length / 2;

        bignum b_part1; //младшая часть числа b
        b_part1.values = b.values;
        if (a_part1.length <= b.length) b_part1.length = a_part1.length;
        else b_part1.length = b.length;

        bignum b_part2; //старшая часть числа b
        b_part2.values = b.values + b_part1.length;
        if (a_part1.length <= b.length) b_part2.length = b.length - b_part1.length;
        else b_part2.length = 0;


        bignum sum_of_a_parts = Add(a_part1, a_part2); //cумма частей числа a
        bignum sum_of_b_parts = Add(b_part1, b_part2); //cумма частей числа b
        //bigLog("Mul","SumA",sum_of_a_parts);
        //bigLog("Mul","SumB",sum_of_b_parts);
        sum_of_a_parts.length=GetBigLength(sum_of_a_parts);
        sum_of_b_parts.length=GetBigLength(sum_of_b_parts);

        bigLog("Mul","SumA",sum_of_a_parts);
        bigLog("Mul","SumB",sum_of_b_parts);


        bignum product_of_sums_of_parts = karatsuba(sum_of_a_parts, sum_of_b_parts);

        cout << "___________________________________________\n";

        // произведение сумм частей
        if (a_part1.length>0) a_part1.length=GetBigLength(a_part1);
        if (b_part1.length>0) b_part1.length=GetBigLength(b_part1);
        bignum product_of_first_parts = karatsuba(a_part1, b_part1); //младший член
        long long ForPrintSecond = ForPrintBase + product_of_first_parts.length;
        bignum product_of_second_parts;

        if (a_part2.length>0) a_part2.length=GetBigLength(a_part2);
        if (b_part2.length>0) b_part2.length=GetBigLength(b_part2);
        product_of_second_parts = karatsuba(a_part2, b_part2);
        //bigLog("ErrOne","",product_of_second_parts);

        //cout << product_of_sums_of_parts.length << product_of_first_parts.length << product_of_second_parts.length << "\n";
        bignum sum_of_middle_terms = Sub(Sub(product_of_sums_of_parts, product_of_first_parts), product_of_second_parts);
        //bigLog("ErrTwo","",product_of_second_parts);
        normalize(sum_of_middle_terms);
        sum_of_middle_terms.length = GetBigLength(sum_of_middle_terms);

        bigLog("Mul","MID",sum_of_middle_terms);

        //нахождение суммы средних членов

        /*
        * Суммирование многочлена
        */

        cout << "ForPrintSecond" << ForPrintSecond << "\n";
        cout << "ForPrintBase" << ForPrintBase << "\n";

        //memcpy(product.values, product_of_first_parts.values,product_of_first_parts.length * sizeof(digit));
        //memcpy(product.values + 2*ForPrintBase,product_of_second_parts.values, product_of_second_parts.length * sizeof(digit));

        bignum_memcpy(product, product_of_first_parts, 0);
        bignum_memcpy(product, product_of_second_parts, ForPrintBase+ForPrintBase);

        //bigLog("ErrTre","",product_of_second_parts);
        bigLog("Mul","BefMid", product);

        for (unsigned long long i = 0; i < sum_of_middle_terms.length; ++i){
            cout << i << "\n";
            product.values[ForPrintBase + i] += sum_of_middle_terms.values[i];
            //bigLog("Mulli", "Situation", product);
        }

        //cout << [] product_of_second_parts.values;
        //bigLog("Error","",product_of_second_parts);
        //Dump(product_of_second_parts);

        /*
        * Зачистка
        */
        delete [] sum_of_a_parts.values;
        cout << "A";
        delete [] sum_of_b_parts.values;
        cout << "a";
        delete [] product_of_sums_of_parts.values;
        cout << "b";
        delete [] product_of_first_parts.values;
        cout << "c";
        delete [] product_of_second_parts.values;
        cout << "d";
    }

    cout << "u\n";

    normalize(product); //конечная нормализация числа

    cout << "ENDKAR\n";
    bigLog("MulEND","InA",a);
    bigLog("MulEND","InB",b);
    bigLog("MulEND","PRODUCT",product);
    cout << "\n";

    return product;
}

//Экспортируемые из Dll ф-ции
string DllAdd(string a,string b){
    bignum x,y,res;
    if (a.length() < b.length()){
        x=StrToBig(b);
        y=StrToBig(a);
    }
    else{
        x=StrToBig(a);
        y=StrToBig(b);
    }
    res = Add(x,y);

    return BigToStr(res);
}

string DllSub(string a,string b){
    bignum x,y,res;
    x=StrToBig(a);
    y=StrToBig(b);

    for (unsigned long long i = 0; i <=y.length-1; i++){
            y.values[i] = y.values[i] * (-1);
        }
    if (y.length>x.length){
        res=y;
        y=x;
        x=res;
    }

    res = Add(x,y);

    return BigToStr(res);
}

string DllMul(string a,string b){
    bignum x,y,res;
    unsigned long long Float;
    x=StrToBig(a);
    y=StrToBig(b);

    if (y.length>x.length){
        res=y;
        y=x;
        x=res;
    }

    int sign;
    sign = GetSign(x) * GetSign(y);
    bignum_Abs(x);
    bignum_Abs(y);

    Float = x.floatpoint + y.floatpoint;
    x.floatpoint=0;
    y.floatpoint=0;

    res = karatsuba(x,y);

    res.floatpoint = Float;

    SetSign(res, sign);

    return BigToStr(res);
}

string Butaf (string a,string b){
    bignum x;
    x=StrToBig(a);
    return BigToStr(x);
}
extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
