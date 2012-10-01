#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QTextCodec>
#include <QFrame>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include "parametr.h"
#include "qdir.h"
#include <QSettings>

extern Parametr* par;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fnt_d=new QFontDialog(parent);
    connect(fnt_d, SIGNAL(currentFontChanged(const QFont &)),
                 this, SLOT(fontUpdate(const QFont &)));
    ui->frame_progress->setVisible(false);
    QFont fnt=par->fnt;
    fnt.setPointSize(8);
    ui->table->setFont(QFont(fnt));
    fill_table();
    ui->sliderBottom->setRange(1-(par->y),0);
    ui->sliderTop->setRange(0,par->y-1);
    Show_font();
    ui->label_xy->setText(tr("character width=%1%4font height=%2%4height to record=%3")
            .arg(par->x)
            .arg(par->y)
            .arg(par->line_bottom-par->line_top)
            .arg("\n"));
    QSettings settings(QSettings::UserScope, "Microsoft", "Windows");
    settings.beginGroup("CurrentVersion/Explorer/Shell Folders");
    home_path=settings.value("Personal").toString()+QDir::separator();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fill_table(void)
{
    for (int y=0;y<16;y++)
        for (int x=0;x<16;x++)
        {
            int n=y*16+x;
            QTableWidgetItem *newitem = new QTableWidgetItem(QChar(par->utf[n]));
            newitem->setTextAlignment(Qt::AlignCenter);
            newitem->setStatusTip("yes");
            newitem->setForeground(Qt::black);
            ui->table->setItem(y,x,newitem);
        }
}

void MainWindow::on_table_cellClicked(int row, int column)
{
    par->symbol=par->utf[row*16+column];
    par->Calc();
    ui->label_xy->setText(tr("character width=%1%4font height=%2%4height to record=%3")
            .arg(par->x)
            .arg(par->y)
            .arg(par->line_bottom-par->line_top)
            .arg("\n"));
    ui->widget->update();
}

void MainWindow::on_push_pen_clicked()
{
    QColor c=QColorDialog::getColor(par->pen, this);
    if (c.isValid()) par->pen = c;
}

void MainWindow::on_push_brush_clicked()
{
    QColor c=QColorDialog::getColor(par->brush, this);
    if (c.isValid()) par->brush = c;
}

void MainWindow::Symbol(int r,int c)
{
    // запись одного символа
    par->symbol=par->utf[r*16+c];
    par->Calc();
    adr << QString("\t0x%1,0x%2, // 0x%3 \"%4\"\n")
           .arg(bias,4,16,QLatin1Char('0'))
           .arg(par->x,4,16,QLatin1Char('0'))
           .arg(r*16+c,2,16,QLatin1Char('0'))
           .arg(QChar(par->symbol));
    QPainter paint;
    QImage img=QImage(par->x,par->y,QImage::Format_RGB16);
    paint.begin(&img);
    paint.setFont(par->fnt);
    paint.setBrush(QBrush(par->brush,Qt::SolidPattern));
    paint.setPen(QPen(par->pen,1,Qt::SolidLine,Qt::FlatCap));
    paint.fillRect(0,0,par->x,par->y,QBrush(par->brush,Qt::SolidPattern));
    paint.setRenderHint(QPainter::TextAntialiasing,true);
    paint.drawText(par->zero_x,par->zero_y,QString(par->symbol));
    buf << QString("\n\t// symbol 0x%1 \"%2\" ").arg(r*16+c,2,16,QLatin1Char('0')).arg(QChar(par->symbol));
    // ограничение по высоте
    for (int a=0;a<par->x;a++)
    {
        for (int b=0;b<(par->line_bottom-par->line_top);b++)
            {
                quint16 c=img.pixel(a,b+par->line_top);
                if (((a*(par->line_bottom-par->line_top)+b)%8)==0) buf << "\n\t";
                buf << QString("0x%1,").arg(c,4,16,QLatin1Char('0'));
            }
        bias++;
        }
    paint.end();
}

void MainWindow::on_push_save_clicked()
{
    // создается таблица адресации символов и сам массив символов.
    // создаются в разных файлах. Общее название - имя шрифта+размер.
    // таблица - adr_общее название, массив buf_общее название.
    ui->frame_progress->setVisible(true);
    ui->progressBar->setValue(0);
    QString all=par->fnt.rawName().replace(' ','_')+"_"+QString::number(par->line_bottom-par->line_top);
    ui->label_file->setText(tr("save files: %1").arg(home_path+"xxx_"+all+".c"));
    QFile f_adr(home_path+"adr_"+all+".c");
    QFile f_buf(home_path+"buf_"+all+".c");
    QFile f_str(home_path+"setting.txt");
    f_adr.open(QIODevice::WriteOnly | QIODevice::Text);
    f_buf.open(QIODevice::WriteOnly | QIODevice::Text);
    f_str.open(QIODevice::ReadOnly | QIODevice::Text);
    adr.setDevice(&f_adr);
    buf.setDevice(&f_buf);
    str.setDevice(&f_str);
    adr.setCodec(QTextCodec::codecForName("UTF-8"));
    buf.setCodec(QTextCodec::codecForName("UTF-8"));
    str.setCodec(QTextCodec::codecForName("UTF-8"));
    while (!str.atEnd())
    {
        QString s=str.readLine();
        if (s=="") break;
        s.replace(QString("%name%"),all);
        s.replace(QString("%size%"),QString::number(par->line_bottom-par->line_top));
        adr << s << "\n";
    }
    while (!str.atEnd())
    {
        QString s=str.readLine();
        if (s=="") break;
        s.replace(QString("%name%"),all);
        s.replace(QString("%size%"),QString::number(par->line_bottom-par->line_top));
        buf << s << "\n";
    }
    bias=0;
    int back=par->symbol;
    // последовательно считывается таблица. Для выбранных символов формируется массив,
    // для не выбранных записывается 0 в таблицу адресов.
    for (int r=0;r<16;r++) for (int c=0;c<16;c++)
    {
        ui->progressBar->setValue(r*16+c);
        if (ui->table->item(r,c)->statusTip()=="yes") Symbol(r,c);
        else adr << QString("\t0x%1,0x0000, // 0x%2\n")
                    .arg(bias,4,16,QLatin1Char('0'))
                    .arg(r*16+c,2,16,QLatin1Char('0'));
    }
    buf << "\n\t};\n";
    adr << "\n\t}; // the size of an array of characters = " << (bias*par->line_bottom-par->line_top*2)/1024 << "Kb \n";
    f_adr.close();
    f_buf.close();
    f_str.close();
    ui->frame_progress->setVisible(false);
    par->symbol=back;
    ui->widget->update();
}

void MainWindow::on_table_customContextMenuRequested(const QPoint &pos)
{
    QAction* actionUse = new QAction(tr("use simbol(s)"), this);
    QAction* actionNoUse = new QAction(tr("no use symbol(s)"), this);
    connect(actionUse, SIGNAL(triggered()), this, SLOT(useSymbols()));
    connect(actionNoUse, SIGNAL(triggered()), this, SLOT(nouseSymbols()));
    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionUse);
    contextMenu->addAction(actionNoUse);
    contextMenu->exec(QCursor::pos());
}

void MainWindow::fontUpdate(const QFont & font)
{
    par->fnt=font;
    QFont fnt=font;
    fnt.setPointSize(8);
    ui->table->setFont(fnt);
    par->Calc();
    ui->label_xy->setText(tr("character width=%1%4font height=%2%4height to record=%3")
            .arg(par->x)
            .arg(par->y)
            .arg(par->line_bottom-par->line_top)
            .arg("\n"));
    par->line_top=0;
    par->line_bottom=par->y;
    ui->sliderBottom->setRange(1-par->y,0);
    ui->sliderTop->setRange(0,par->y-1);
    ui->sliderBottom->setValue(0);
    ui->sliderTop->setValue(0);
    checkBorderline();
    ui->widget->update();
}

void MainWindow::useSymbols(void)
{
    for (int r=0;r<ui->table->rowCount();r++)
        for (int c=0;c<ui->table->columnCount();c++)
        {
            if(ui->table->item(r,c)->isSelected())
            {
                // использовать символы
                ui->table->item(r,c)->setStatusTip("yes");
                ui->table->item(r,c)->setForeground(Qt::black);
             }
        }
    Show_font();
}

void MainWindow::nouseSymbols(void)
{
    for (int r=0;r<ui->table->rowCount();r++)
        for (int c=0;c<ui->table->columnCount();c++)
        {
            if(ui->table->item(r,c)->isSelected())
            {
                // не использовать символы
                ui->table->item(r,c)->setStatusTip("no");
                ui->table->item(r,c)->setForeground(Qt::lightGray);
             }
        }
    Show_font();
}

void  MainWindow::Show_font(void)
{ // вывод информации о шрифте
    QFontMetrics fm(par->fnt);
    QString str="'"+par->fnt.family()+"' "+QString::number(par->fnt.pointSize());
    if (par->fnt.bold()) str+=" bold";
    if (par->fnt.italic()) str+=" italic";
    int n=0;
    for (int r=0;r<ui->table->rowCount();r++)
        for (int c=0;c<ui->table->columnCount();c++){
        if (ui->table->item(r,c)->statusTip()=="yes"){
            int s=par->utf[r*16+c];
            n+=(fm.width(s)-fm.leftBearing(s)-fm.rightBearing(s));
        }
    }
    n=(n*(par->line_bottom-par->line_top))/1024;
    str+=tr("  size array = ")+QString::number(n)+tr(" kB");
    ui->label_font->setText(str);
}

void MainWindow::on_push_font_clicked()
{
    fnt_d->setCurrentFont(par->fnt);
    fnt_d->show();
}

void MainWindow::on_sliderBottom_valueChanged(int value)
{
    par->line_bottom=par->y+value;
    if (par->line_bottom-par->line_top<1) par->line_top=par->line_bottom-1;
    ui->sliderTop->setValue(par->line_top);
    checkBorderline();
    ui->widget->update();
}

void MainWindow::on_sliderTop_valueChanged(int value)
{
    par->line_top=value;
    if (par->line_bottom-par->line_top<1) par->line_bottom=par->line_top+1;
    ui->sliderBottom->setValue(par->line_bottom-par->y);
    checkBorderline();
    ui->widget->update();
}

void MainWindow::checkBorderline(void)
{
    QFontMetrics fm(par->fnt);
    for (int r=0;r<ui->table->rowCount();r++)
        for (int c=0;c<ui->table->columnCount();c++)
        {
            QRect rect=fm.boundingRect(QChar(par->utf[r*16+c]));
            if ((par->line_top>(par->zero_y+rect.top()))||(par->line_bottom<(par->zero_y+rect.bottom()+1))) {
                if(ui->table->item(r,c)->statusTip()=="yes") ui->table->item(r,c)->setBackgroundColor(Qt::lightGray);}
                    else ui->table->item(r,c)->setBackgroundColor(Qt::white);

        }
    ui->label_xy->setText(tr("character width=%1%4font height=%2%4height to record=%3")
            .arg(par->x)
            .arg(par->y)
            .arg(par->line_bottom-par->line_top)
            .arg("\n"));
    Show_font();
}

void MainWindow::on_actionSave_triggered()
{
    on_push_save_clicked();
}

void MainWindow::on_actionChange_triggered()
{
    on_push_font_clicked();
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

void MainWindow::on_actionPen_triggered()
{
on_push_pen_clicked();
}

void MainWindow::on_actionBrush_triggered()
{
on_push_brush_clicked();
}

void MainWindow::on_actionConfigure_triggered()
{

}
