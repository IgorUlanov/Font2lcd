#ifndef PARAMETR_H
#define PARAMETR_H
#include <QColor>
#include <QFont>

class Parametr
{
private:
    void Symbol(int,int);
    void Load(void); // функция считывания из файла "symbol.csv" таблицы symbol
public:
    Parametr(QFont);
    quint16 utf[256]; // таблица соответствия кодов символов utf8 - cp1251
    void Save(void);
    int symbol; // выбранный символ
    QColor pen,brush; // цвета шрифта для конвертации
    QFont fnt; // шрифт для конвертации
    static const int height=363; // высота виджета для вывода символа
    static const int width=363; // ширина виджета для вывода символа
    int k,x,y; // коэфициент умножения при выводе символа на экран, и изначальная высота и ширина символа
    int zero_y,zero_x; // начальная точка для вывода символа
    void Calc(void);
//    int max_top,max_bottom; //
    int line_top,line_bottom;
};

#endif // PARAMETR_H
