#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QString>
#include <QFont>
#include <QFontDialog>
#include <QFontMetrics>
#include <QTextStream>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_table_cellClicked(int row, int column);
    void on_push_pen_clicked();
    void on_push_brush_clicked();
    void on_push_save_clicked();
    void on_table_customContextMenuRequested(const QPoint &pos);
    void fontUpdate(const QFont &);
    void nouseSymbols(void);
    void useSymbols(void);
    void on_push_font_clicked();
    void on_sliderBottom_valueChanged(int value);
    void on_sliderTop_valueChanged(int value);
    void on_actionSave_triggered();
    void on_actionChange_triggered();
    void on_actionConfigure_triggered();
    void on_actionExit_triggered();
    void on_actionPen_triggered();
    void on_actionBrush_triggered();

private:
    Ui::MainWindow *ui;
    void fill_table(void);
    QTextStream adr,buf,str;
    void Symbol(int,int); // запись одного символа
    int bias;
    QFontDialog* fnt_d;
    void checkBorderline(void);
    void Show_font(void); // вывод информации о шрифте
    QString home_path;
};

#endif // MAINWINDOW_H
