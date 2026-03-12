#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//#include <QTextToSpeech> // 文本转语音功能

// 数据库相关
#include <QSqlDatabase> // 数据库连接
#include <QSqlQuery> // SQL查询

// 子页面
#include <regdialog.h> // 注册对话框
#include <second.h> // 第二个页面(用途未明)
#include <sqlit3test.h> // SQLite数据库测试页面
#include <system_main_page.h> // 系统主页面

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief 主窗口类，继承自QMainWindow
 *
 * 该类实现了应用程序的主窗口，包含登录、注册、数据库测试等功能
 * 并负责管理与其他子页面的交互
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr); // 构造函数
    ~MainWindow(); // 析构函数

private slots:
    void on_Login_Button_clicked();

    void on_Sqlite_Test_Button_clicked();

    void on_Reg_Button_clicked();

    void on_Show_Button_clicked();

    void on_forget_pass_clicked();

private:
    void UI();

    Ui::MainWindow *ui; // UI界面指针
//    QTextToSpeech *tts; // 文本转语音对象

    // 子页面指针
    second *S1; // 第二个页面实例
    sqlit3test *S2; // SQLite测试页面实例
    System_Main_Page *system_main_page; // 声明主页面指针
};
#endif // MAINWINDOW_H
