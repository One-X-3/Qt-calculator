#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <math.h>
#include <QKeyEvent>
#include <QTimer>

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

    // 【新增】：让所有按钮都不抢占键盘焦点，保证按键事件统统交给主窗口处理
        QList<QPushButton*> allButtons = this->findChildren<QPushButton*>();
        for (QPushButton* btn : allButtons) {
            btn->setFocusPolicy(Qt::NoFocus);
        }

        // 让显示框不可获取焦点，防止它拦截退格键
        ui->display->setFocusPolicy(Qt::NoFocus);

        // 让主窗口强制捕获所有键盘事件
        this->setFocusPolicy(Qt::StrongFocus);

        // 启动时让主窗口获取焦点
        this->setFocus();

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

// CE 键逻辑：仅清除当前输入的数字，保留之前的操作符和第一操作数
void MainWindow::on_btnClearEntry_clicked()
{
    operand = "0";                 // 将当前输入重置为 "0"
    ui->display->setText("0");     // 显示框归零
    waitingForOperand = true;      // 标记等待新输入（这样用户按下一个数字时会覆盖这个 0）
    ui->statusbar->showMessage("CE: 已清除当前输入", 2000); // 状态栏提示2秒
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

// +/- 键逻辑：正负号切换
void MainWindow::on_btnNegate_clicked()
{
    // 如果处于等待新输入的状态（比如刚按完运算符），默认输入一个 "-0"
    if (waitingForOperand) {
        operand = "-0";
        ui->display->setText(operand);
        waitingForOperand = false;
        return;
    }

    // 如果是正常的数字
    if (operand != "0") {
        if (operand.startsWith("-")) {
            // 如果是负数，去掉负号
            operand.remove(0, 1);
        } else {
            // 如果是正数，加上负号
            operand.prepend("-");
        }
        ui->display->setText(operand);
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

// 键盘事件处理函数
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Backspace) {
        ui->btnBackspace->animateClick(100); // 模拟按下100毫秒
        return;
    }

    auto simulateClick = [](QPushButton *btn) {
        if (!btn) return;
        btn->setDown(true);

        QTimer::singleShot(80, btn, [btn]() {
            btn->setDown(false);
            btn->click();
        });
    };

    int key = event->key();

        if (key == Qt::Key_Percent || (key == Qt::Key_5 && (event->modifiers() & Qt::ShiftModifier))) {
            simulateClick(ui->btnPercentage);
            return;
        }

    // 1. 数字键 0-9
    if (key >= Qt::Key_0 && key <= Qt::Key_9 && !(key == Qt::Key_5 && (event->modifiers() & Qt::ShiftModifier))) {
        QString btnName = QString("btnNum%1").arg(key - Qt::Key_0);
        simulateClick(this->findChild<QPushButton*>(btnName));
        return;
    }

    // 2. 运算符与功能键
    switch (key) {
        case Qt::Key_Plus:      simulateClick(ui->btnAdd); break;
        case Qt::Key_Minus:     simulateClick(ui->btnSub); break;
        case Qt::Key_Asterisk:  simulateClick(ui->btnMul); break;
        case Qt::Key_Slash:     simulateClick(ui->btnDiv); break;
        case Qt::Key_Return:
        case Qt::Key_Enter:     simulateClick(ui->btnEqual); break;
        case Qt::Key_Period:    simulateClick(ui->btnPoint); break;
        case Qt::Key_Escape:    simulateClick(ui->btnClear); break;
        case Qt::Key_Delete:    simulateClick(ui->btnClearEntry); break;
        case Qt::Key_0 + Qt::KeypadModifier: simulateClick(ui->btnNum0); break;


        default:
            QMainWindow::keyPressEvent(event);
            break;
    }
}
