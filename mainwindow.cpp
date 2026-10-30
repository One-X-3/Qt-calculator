#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <math.h>

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

    connect(ui->btnAdd, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorclicked()));
    connect(ui->btnSub, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorclicked()));
    connect(ui->btnMul, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorclicked()));
    connect(ui->btnDiv, SIGNAL(clicked()),this,SLOT(btnBinaryOperatorclicked()));

    connect(ui->btnPercentage, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnReciprocal, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSquare, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSqrt, SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));

    ui->display->setText("0"); // 启动时显示0
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::calculation(double rightOperand, bool *ok)
{
    double result = 0.0;
    if (ok) *ok = true;

    if (pendingOperator == "+") {
        result = leftOperand + rightOperand;
    } else if (pendingOperator == "-") {
        result = leftOperand - rightOperand;
    } else if (pendingOperator == "×" || pendingOperator == "*") {
        result = leftOperand * rightOperand;
    } else if (pendingOperator == "÷" || pendingOperator == "/") {
        if (qFuzzyIsNull(rightOperand)) {
            ui->display->setText("除数不能为零");
            if (ok) *ok = false;
            return "Error";
        }
        result = leftOperand / rightOperand;
    } else {
        result = rightOperand;
    }

    return QString::number(result, 'g', 15);
}

//数字键输入逻辑
void MainWindow::btnNumClicked()
{
    QString digit = qobject_cast<QPushButton*>(sender())->text();

    // 如果是等待新输入的状态（按了运算符或等号之后），先清空当前输入
    if (waitingForOperand) {
        operand = "";
        waitingForOperand = false;
    }

    // 避免出现 "05" 这样的前导零
    if (operand == "0" && digit != "0") {
        operand = "";
    }

    // 针对 0 的特殊处理
    if (operand == "" && digit == "0") {
        operand = "0";
    } else {
        operand += digit;
    }

    ui->display->setText(operand);
}


//小数点逻辑，一个数只能有一个小数点
void MainWindow::on_btnPoint_clicked()
{
    // 如果处于等待状态，先自动补0
    if (waitingForOperand) {
        operand = "0";
        waitingForOperand = false;
    }

    // 限制只能输入一个小数点
    if (!operand.contains(".")) {
        if (operand.isEmpty()) operand = "0";
        operand += ".";
        ui->display->setText(operand);
    }
}

//退格逻辑，从左边去字符串长度-1
void MainWindow::on_btnBackspace_clicked()
{
    // 如果处于等待新输入状态，不允许退格修改原有的结果
    if (waitingForOperand) return;

    operand.chop(1); // 删除最后一个字符

    if (operand.isEmpty() || operand == "-") {
        operand = "0";
        waitingForOperand = true; // 删光了，回到初始状态
    }
    ui->display->setText(operand);
}

//CLear键逻辑，清空
void MainWindow::on_btnClear_clicked()
{
    operand = "0";
    leftOperand = 0.0;
    pendingOperator.clear();
    waitingForOperand = true;
    ui->display->setText("0");
    ui->statusbar->clearMessage();
}

void MainWindow::btnBinaryOperatorclicked()
{
    QString opcode = qobject_cast<QPushButton*>(sender())->text();
    double currentOperand = ui->display->text().toDouble();

    // 如果之前已经有运算符，且不是等待状态（说明用户输入了第二个操作数）
    // 此时需要先把前面的结算掉（例如 2 + 3，按第二个 + 时，先算出 5）
    if (!pendingOperator.isEmpty() && !waitingForOperand) {
        bool ok = false;
        QString result = calculation(currentOperand, &ok);
        if (!ok) {
            return; // 除零等错误直接返回
        }
        currentOperand = result.toDouble();
        ui->display->setText(result);
        // 由于已经算完了，现在的 operand 也要同步更新为结果
        operand = result;
    }

    // 保存状态
    leftOperand = currentOperand;
    pendingOperator = opcode;
    waitingForOperand = true; // 等待输入第二个操作数

    ui->statusbar->showMessage(QString("暂存: %1 %2").arg(leftOperand).arg(pendingOperator));
}

void MainWindow::btnUnaryOperatorClicked()
{
    double currentVal = ui->display->text().toDouble();
    QString op = qobject_cast<QPushButton*>(sender())->text();
    double result = 0.0;
    bool ok = true;

    if (op == "%") {
        result = currentVal / 100.0;
    } else if (op == "1/x") {
        if (qFuzzyIsNull(currentVal)) {
            ui->display->setText("除数不能为零");
            ok = false;
        } else {
            result = 1.0 / currentVal;
        }
    } else if (op == "x^2") {
        result = currentVal * currentVal;
    } else if (op == "√") {
        if (currentVal < 0) {
            ui->display->setText("无效输入");
            ok = false;
        } else {
            result = sqrt(currentVal);
        }
    }

    if (ok) {
        // 算完后，直接更新 operand，并设置等待新输入，防止用户继续乱按
        operand = QString::number(result, 'g', 15);
        ui->display->setText(operand);
        waitingForOperand = true;
    } else {
        // 出错了，清理状态
        leftOperand = 0;
        pendingOperator.clear();
        waitingForOperand = true;
    }
}

void MainWindow::on_btnEqual_clicked()
{
    if (pendingOperator.isEmpty() || waitingForOperand) {
        return; // 没有运算符，或者连按等号，不做处理
    }

    double currentOperand = ui->display->text().toDouble();
    bool ok = false;

    QString resultStr = calculation(currentOperand, &ok);

    if (ok) {
        ui->display->setText(resultStr);

        // 计算完成后，结果作为新的 operand 和 leftOperand
        operand = resultStr;
        leftOperand = resultStr.toDouble();
        pendingOperator.clear();
        waitingForOperand = true; // 下次按数字键会覆盖当前结果
    } else {
        operand = "0";
        leftOperand = 0.0;
        pendingOperator.clear();
        waitingForOperand = true;
    }
}

