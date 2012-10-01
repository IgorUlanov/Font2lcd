#include "parametr.h"
#include <QColor>
#include <QFontMetrics>
#include <QPainter>
#include <QImage>
#include <QTextCodec>
#include <Qstring>
#include <QList>
#include <QFile>
#include <QTextStream>

Parametr::Parametr(QFont fn)
{
    symbol=0x38;
    pen=Qt::black;
    brush=Qt::white;
    fnt=fn;
    fnt.setPointSize(12);
    Load();
    Calc();
    line_bottom=y;
    line_top=0;
}

void Parametr::Calc()
{
    QFontMetrics fm(fnt);
    int a=0;
    a-=fm.leftBearing(symbol);
    a+=fm.width(symbol);
    a-=fm.rightBearing(symbol);
    int b=fm.ascent()+fm.descent()+1;
    if ((a>0)&&(b>0))
    {
        x=a;
        y=b;
        zero_y=fm.ascent();
        zero_x=-fm.leftBearing(symbol);
        k=(width-2)/a;
        if (k>(height-2)/b) k=(height-2)/b;
    }
}

void Parametr::Load(void)
{
    // считывание таблицы symbol
    QFile f_sym("symbol.csv");
    f_sym.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream sym(&f_sym);
    sym.setCodec(QTextCodec::codecForName("UTF-8"));
    int i=0;
    while (!sym.atEnd())
     {
         QString str = sym.readLine();
         QList<QString> list=str.split(QRegExp("\\b(;)\\b"));
         for (int n=0;n<list.size();n++) utf[i++]=list.value(n).toUInt(0,10);
     }
     f_sym.close();
}
