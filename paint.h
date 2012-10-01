#ifndef PAINT_H
#define PAINT_H

#include <QWidget>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>

class Paint : public QWidget
{
    Q_OBJECT
public:
    explicit Paint(QWidget *parent = 0);
private:
    void Symbol(void);
    QImage img;
signals:
    
public slots:
    void paintEvent(QPaintEvent *);
};

#endif // PAINT_H
