#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnNum0,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum1,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum2,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum3,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum4,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum5,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum6,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum7,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum8,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
    connect(ui->btnNum9,SIGNAL(clicked()),this,SLOT(btnNumClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//数字键输入逻辑
void MainWindow::btnNumClicked()
{
    QString digit = qobject_cast<QPushButton*>(sender())->text();

    if (digit == "0" && operand == "0")
        digit = "";

    if(operand == "0" && digit != "0")
        operand = "";

    operand += digit;

    ui->display->setText(operand);
}

//小数点逻辑，一个数只能有一个小数点
void MainWindow::on_btnPoint_clicked()
{
    if(!operand.contains("."))
        operand += qobject_cast<QPushButton*>(sender())->text();
    ui->display->setText(operand);

}

//退格逻辑，从左边去字符串长度-1
void MainWindow::on_btnBackspace_clicked()
{
    operand = operand.left(operand.length() - 1);
    ui->display->setText(operand);
}

//CLear键逻辑，清空
void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    ui->display->setText(operand);
}

