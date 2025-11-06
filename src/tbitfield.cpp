// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

TBitField::TBitField(int len)
{
    if (len <= 0)
        throw "Invalid bitfield length";

    // Устанавливаем длину битового поля
    BitLen = len;
    // Вычисляем количество элементов массива TELEM для хранения всех битов
    // sizeof(TELEM)*8 - количество битов в одном элементе TELEM
    //(100+32-1) / 32 = 4 для целочисл делен
    MemLen = (len + sizeof(TELEM) * 8 - 1) >>5;
    // Выделяем память под массив
    
    pMem = new TELEM[MemLen];

    // Инициализируем все биты нулями
    for (int i = 0; i < MemLen; i++)
        pMem[i] = 0;
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    // Копируем основные параметры
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    // Выделяем новую память
    pMem = new TELEM[MemLen];

    // Копируем данные из исходного объекта
    for (int i = 0; i < MemLen; i++)
        pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
// в каком pmem[?] хранится бит(этаж)
{
    if (n < 0 || n >= BitLen)
        throw "Bit index out of range";
    // Делим номер бита на количество битов в одном элементе TELEM
    return n >>5;
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{//(квартира)00..010
    if (n < 0 || n >= BitLen)
        throw "Bit index out of range";
    // Вычисляем позицию бита внутри элемента TELEM и сдвигаем единицу
    return 1 << (n & 31);
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >= BitLen)
        throw "Bit index out of range";

    int index = GetMemIndex(n);// Индекс элемента в массиве этаж
    TELEM mask = GetMemMask(n);// Маска для нужного бита кв
    pMem[index] |= mask; // Устанавливаем бит с помощью ИЛИ
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || n >= BitLen)
        throw "Bit index out of range";

    int index = GetMemIndex(n);
    TELEM mask = GetMemMask(n);
    pMem[index] &= ~mask; // Очищаем бит с помощью И с инвертированной маской
}

int TBitField::GetBit(const int n) const // получить значение бита(0 или 1)
{
    if (n < 0 || n >= BitLen)
        throw "Bit index out of range";

    int index = GetMemIndex(n);
    TELEM mask = GetMemMask(n);
    return (pMem[index] & mask) != 0;// Проверяем установлен ли бит
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    // Проверка на самоприсваивание
    if (this != &bf)
    {
        // Освобождаем старую память
        delete[] pMem;
        // Копируем параметры
        BitLen = bf.BitLen;
        MemLen = bf.MemLen;
        // Выделяем новую память
        pMem = new TELEM[MemLen];
        // Копируем данные
        for (int i = 0; i < MemLen; i++)
            pMem[i] = bf.pMem[i];
    }
    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    // Если длины разные - поля не равны
    if (BitLen != bf.BitLen)
        return 0;

    // Сравниваем все элементы массивов
    for (int i = 0; i < MemLen; i++)
        if (pMem[i] != bf.pMem[i])
            return 0;
    return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    return !(*this == bf);  // Просто инвертируем результат сравнения на равенство
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или", для накладывания маски
{
    // Выбираем максимальную длину из двух полей
    int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    TBitField res(maxLen);//создаем бит.поле макс длины

    // Копируем биты из текущего объекта
    for (int i = 0; i < MemLen; i++)
        res.pMem[i] |= pMem[i];

    // Копируем биты из второго объекта
    for (int i = 0; i < bf.MemLen; i++)
        res.pMem[i] |= bf.pMem[i];

    return res;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    // Выбираем максимальную длину из двух полей
    int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
    TBitField result(maxLen);//создаем бит.поле макс длины

    // Выполняем операцию И(перемнож) только для общей части массивов
    int minMemLen = (MemLen < bf.MemLen) ? MemLen : bf.MemLen;
    for (int i = 0; i < minMemLen; i++)
        result.pMem[i] = pMem[i] & bf.pMem[i];

    return result;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField result(BitLen);

    // Инвертируем все элементы массива(1 на 0 .0 на 1)
    for (int i = 0; i < MemLen; i++)
        result.pMem[i] = ~pMem[i];

    // Очищаем лишние биты в последнем элементе массива
    // Это нужно потому что при инверсии устанавливаются биты за пределами BitLen
    int extraBits = BitLen & 31;
    if (extraBits != 0)
    {
        // Создаем маску только для значимых битов
        TELEM mask = (1 << extraBits) - 1;
        result.pMem[MemLen - 1] &= mask;
    }

    return result;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    // Читаем биты последовательно
    for (int i = 0; i < bf.BitLen; i++)
    {
        int bit;
        istr >> bit;
        if (bit == 1)
            bf.SetBit(i);    // Устанавливаем бит
        else if (bit == 0)
            bf.ClrBit(i);    // Очищаем бит
        else
            break;           // Если ввод не 0 или 1 - завершаем
    }
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    // Выводим все биты подряд
    for (int i = 0; i < bf.BitLen; i++)
        ostr << bf.GetBit(i);
    return ostr;
}
