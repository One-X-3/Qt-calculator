#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void btnNumClicked();
    void btnBinaryOperatorclicked();
    void btnUnaryOperatorClicked();

    void on_btnPoint_clicked();
    void on_btnBackspace_clicked();
    void on_btnClear_clicked();
    void on_btnEqual_clicked();
    void on_btnClearEntry_clicked(); // 清除当前输入 (CE)
    void on_btnNegate_clicked();     // 正负号切换 (+/-)

private:
    Ui::MainWindow *ui;

    QString calculation(double rightOperand, bool *ok = nullptr);

    QString operand = "0";         // 当前输入的字符串
    double leftOperand = 0.0;      // 第一个操作数
    QString pendingOperator = "";  // 挂起的运算符 (+, -, *, /)
    bool waitingForOperand = true; // 标记是否等待输入新的操作数
};
#endif // MAINWINDOW_H
