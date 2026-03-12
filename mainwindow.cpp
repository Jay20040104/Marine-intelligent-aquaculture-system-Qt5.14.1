#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 *  qt_material_widget-UI库
 *  neumorphic拟态效果
 */
// UI库开始

#include "qtmaterialautocomplete.h" // 引入Material风格输入框头文件（带自动补全功能）
#include "qtmaterialradiobutton.h" // 引入Material风格单选框头文件（注意：代码里注释写的"复选框"可能是笔误，实际是单选框）
#include "neumorphic.h" // 引入拟态UI效果类头文件（用于实现控件的拟态风格）

// UI库结束

#include <QIcon>  // 用于设置图标

// 调试、消息框、语音合成相关头文件
#include <QDebug>         // 输出调试信息
#include <QMessageBox>    // 弹出提示框
//#include <QTextToSpeech>  // 文本转语音（TTS）

#include "passwordtogglehelper.h"
#include "DatabaseManager.h"

// 主窗口初始化函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)  // 继承QMainWindow，指定父窗口
    , ui(new Ui::MainWindow)  // 初始化UI对象
//    , tts(new QTextToSpeech(this))  // 初始化TTS（文本转语音）对象，父对象为当前窗口
{
    ui->setupUi(this);  // 加载UI界面（由Qt Designer生成）

    UI();

    // 打开数据库（失败则退出）
    if (!DatabaseManager::instance().open("F:/IoT-Code/qt/Project/database/Ocean_System_data.db")) {
        return;
    }
    // 创建用户表
    DatabaseManager::instance().createUserTable();


    // ========== 新增：自动填充记住的用户 ==========
    QVector<QMap<QString, QVariant>> users = DatabaseManager::instance().getAllUsers();
    for (const auto& user : users) {
        if (user["marked"].toInt() == 1) { // 找到标记为“记住”的用户
            QString username = user["username"].toString();
            ui->UserName_Input->setText(username); // 填充用户名
            ui->checkBox->setChecked(true);        // 选中“记住我”

            ui->Password_Input->setFocus();
            break; // 假设仅记住一个用户，找到后退出循环
        }
    }
    // =============================================

    //*/ 初始化TTS（文本转语音）功能
    // 检查系统是否支持语音合成（是否有可用的TTS引擎）
//    if (tts->availableEngines().isEmpty()) {
//        qDebug() << "系统不支持语音合成或未安装TTS引擎";
//    } else {
//        // 尝试设置中文语音（遍历可用语音，寻找包含"Chinese"的语音包）
//        QTextToSpeech ttsTemp;  // 临时TTS对象，用于获取可用语音列表
//        foreach (const QVoice &voice, ttsTemp.availableVoices()) {
//            // 通过语音名称判断是否为中文（不区分大小写）
//            if (voice.name().contains("Chinese", Qt::CaseInsensitive)) {
//                tts->setVoice(voice);  // 设置为找到的中文语音
//                break;  // 找到后退出循环
//            }
//        }
//    }*/

    // 初始化子页面对象（用于页面跳转）
    S1 = new second(this);  // 可视化测试页面
    S2 = new sqlit3test(this);  // SQLite测试页面
}


// 析构函数：释放UI资源
MainWindow::~MainWindow()
{
    delete ui;  // 释放UI对象占用的内存
}

// 登录按钮点击事件处理函数
void MainWindow::on_Login_Button_clicked()
{
    // 获取用户名输入框和密码输入框的内容
    QString username = ui->UserName_Input->text();
    QString password = ui->Password_Input->text();
    bool isRemember = ui->checkBox->isChecked();
    // 2. 验证用户名和密码（调用数据库工具类）
    DatabaseManager& db = DatabaseManager::instance();
    if (db.checkUserExists(username)) { // 先检查用户是否存在
        QSqlQuery query(db.getDatabase());
        query.prepare("SELECT password FROM usertable WHERE username = :username");
        query.bindValue(":username", username);
        if (query.exec() && query.next()) {
            QString storedPwd = query.value("password").toString();
            if (storedPwd == password) { // 密码正确，登录成功
                // 3. 根据“记住我”状态更新 marked
                if (isRemember) {
                    db.updateUserMarked(username, 1); // 标记为“记住”
                } else {
                    db.updateUserMarked(username, 0); // 取消标记
                }

//                // 播放登录成功语音（如果TTS不在说话状态）
//                if (tts->state() != QTextToSpeech::Speaking) {
//                    tts->say("登录成功，欢迎回来！");
//                }

                // 创建主页面实例，并传递用户名
                system_main_page = new System_Main_Page(username, this);
                // 跳转到系统主页面，隐藏当前登录窗口
                system_main_page->show();
                this->hide();

            } else {
                QMessageBox::warning(this, "登录失败", "密码错误！");
            }
        }
    } else {
        qDebug()<<"登录失败";  // 输出调试信息
        QMessageBox::warning(this,"登录认证","登录失败");  // 弹出失败提示

//        // 播放登录失败语音
//        if (tts->state() != QTextToSpeech::Speaking) {
//            tts->say("登录失败，请检查用户名和密码");
//        }
    }
}

// SQLite测试按钮点击事件：跳转到SQLite测试页面
void MainWindow::on_Sqlite_Test_Button_clicked()
{
    this->hide();  // 隐藏当前窗口
    S2->setDatabase(DatabaseManager::instance().getDatabase());  // 将当前数据库连接传递给S2页面
    S2->show();  // 显示SQLite测试页面
}


// 注册按钮点击事件：打开注册对话框
void MainWindow::on_Reg_Button_clicked()
{
    regdialog *dialog = new regdialog(this);  // 创建注册对话框对象
    dialog->setDatabase(DatabaseManager::instance().getDatabase());  // 将数据库连接传递给注册对话框
    dialog->exec();  // 以模态方式显示对话框（阻塞当前窗口操作）
}


// 显示按钮点击事件：跳转到可视化测试页面
void MainWindow::on_Show_Button_clicked()
{
    S1->show();  // 显示可视化测试页面
    this->hide();  // 隐藏当前窗口
}

// 忘记密码点击事件：打开注册对话框
void MainWindow::on_forget_pass_clicked()
{
    regdialog *dialog = new regdialog(this);  // 创建注册对话框对象
    dialog->setDatabase(DatabaseManager::instance().getDatabase());  // 将数据库连接传递给注册对话框
    dialog->exec();  // 以模态方式显示对话框（阻塞当前窗口操作）
}

void MainWindow::UI(){
    // 原密码框：ui->Password_Input
    PasswordToggleHelper::setup(ui->Password_Input,
                                ":/first/invisible.png",  // 隐藏密码的图标
                                ":/first/visible.png");    // 显示密码的图标

    // 窗口设置标题
    this->setWindowTitle(QStringLiteral("智能海洋水产养殖系统"));
    // 加载窗口图标（路径为资源文件中的:first前缀下的system_icon.png）
    QIcon icon(":/first/system_icon.png");
    this->setWindowIcon(icon);  // 设置窗口图标


    // 为输入框添加拟态UI效果（拟态风格：内凹模式，类似"凹陷"效果）
    Neumorphic* username_neum = new Neumorphic(Neumorphic::NeumorphicMode_Inset,ui->UserName_Input);
    ui->UserName_Input->setGraphicsEffect(username_neum);  // 应用到用户名输入框
    Neumorphic* password_neum = new Neumorphic(Neumorphic::NeumorphicMode_Inset,ui->Password_Input);
    ui->Password_Input->setGraphicsEffect(password_neum);  // 应用到密码输入框


    // 为按钮添加拟态UI效果（拟态风格：正常模式，类似"凸起"效果）
    Neumorphic* Reg_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Reg_Button);
    ui->Reg_Button->setGraphicsEffect(Reg_neum);  // 应用到注册按钮
    Neumorphic* Sqlite_Test_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Sqlite_Test_Button);
    ui->Sqlite_Test_Button->setGraphicsEffect(Sqlite_Test_neum);  // 应用到SQLite测试按钮
    Neumorphic* Login_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Login_Button);
    ui->Login_Button->setGraphicsEffect(Login_neum);  // 应用到登录按钮
    Neumorphic* Show_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Show_Button);
    ui->Show_Button->setGraphicsEffect(Show_neum);  // 应用到显示按钮


    // 为社交图标按钮添加拟态效果（QQ、微信等图标按钮）
    Neumorphic* QQ_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->QQ);
    ui->QQ->setGraphicsEffect(QQ_neum);
    Neumorphic* Wechat_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Wechat);
    ui->Wechat->setGraphicsEffect(Wechat_neum);
    Neumorphic* bilibili_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->bilibili);
    ui->bilibili->setGraphicsEffect(bilibili_neum);
    Neumorphic* tuite_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->twitter);
    ui->twitter->setGraphicsEffect(tuite_neum);
    Neumorphic* github_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->github);
    ui->github->setGraphicsEffect(github_neum);
}
