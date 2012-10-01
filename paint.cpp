#include "paint.h"
#include "parametr.h"
#include <QColorGroup>
#include <QDebug>

extern Parametr* par;

Paint::Paint(QWidget *parent) :
    QWidget(parent)
{
    img=QImage(par->width,par->height,QImage::Format_RGB16);
}

void Paint::Symbol(void)
{
    QPainter paint;
    par->Calc();
    QImage symbol=QImage(par->x,par->y,QImage::Format_RGB16);
    paint.begin(&symbol);
    paint.initFrom(this);
    paint.setFont(par->fnt);
    paint.setBrush(QBrush(par->brush,Qt::SolidPattern));
    paint.setPen(QPen(par->pen,1,Qt::SolidLine,Qt::FlatCap));
    paint.fillRect(0,0,par->x,par->y,QBrush(par->brush,Qt::SolidPattern));
    paint.setRenderHint(QPainter::TextAntialiasing,true);
    paint.drawText(par->zero_x,par->zero_y,QString(par->symbol));
    img.fill(this->palette().window().color());
    img=symbol.scaled(par->x*par->k,par->y*par->k);
    paint.end();
}

void Paint::paintEvent(QPaintEvent*)
{
    QPainter paint;
    Symbol();
    paint.begin(this);
    paint.drawRect(0,0,par->x*par->k+1,par->y*par->k+2);
    paint.drawImage(1,1,img);
    paint.setPen(Qt::red);
    paint.drawLine(1,par->line_top*par->k+1,par->x*par->k+1,par->line_top*par->k+1);
    paint.drawLine(1,par->line_bottom*par->k+1,par->x*par->k+1,par->line_bottom*par->k+1);
    paint.end();
}
