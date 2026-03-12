#include "regdialog.h"
#include "ui_regdialog.h"

#include "qtmaterialautocomplete.h" //qtmaterial输入框头文件
#include "neumorphic.h"

#include "passwordtogglehelper.h"
#include "DatabaseManager.h"

regdialog::regdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::regdialog)
{
    ui->setupUi(this);

    PasswordToggleHelper::setup(ui->regpassword,
                                ":/first/invisible.png",  // 隐藏密码的图标
                                ":/first/visible.png");    // 显示密码的图标


    // 窗口设置标题
    this->setWindowTitle(QStringLiteral("欢迎新用户注册"));

    Neumorphic* regusername_neum = new Neumorphic(Neumorphic::NeumorphicMode_Inset);
    ui->regusername->setGraphicsEffect(regusername_neum);
    Neumorphic* regpassword_neum = new Neumorphic(Neumorphic::NeumorphicMode_Inset);
    ui->regpassword->setGraphicsEffect(regpassword_neum);

    Neumorphic* submit_neum = new Neumorphic(Neumorphic::NeumorphicMode_Inset);
    ui->submit_Btn->setGraphicsEffect(submit_neum);
    Neumorphic* update_password_neum = new Neumorphic(Neumorphic::NeumorphicMode_Inset);
    ui->update_password_Btn->setGraphicsEffect(update_password_neum);

    Neumorphic* QQ_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal);
    ui->QQ->setGraphicsEffect(QQ_neum);
    Neumorphic* WeChat_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal);
    ui->WeChat->setGraphicsEffect(WeChat_neum);

}

regdialog::~regdialog()
{
    delete ui;
}

// 【新增】接收数据库对象
void regdialog::setDatabase(const QSqlDatabase& db) {
    sqldatabase = db; // 直接赋值（QSqlDatabase 支持值拷贝）
}


void regdialog::on_submit_Btn_clicked()
{
    //获取输入内容
    QString reg_username = ui->regusername->text();
    QString reg_password = ui->regpassword->text();

    //检查输入内容
    if (reg_username.isEmpty() || reg_password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名或密码不能为空");
        return;
    }

    if(DatabaseManager::instance().checkUserExists(reg_username)){
        QMessageBox::warning(this, "警告", "用户名已存在");
        return;
    }

    if(DatabaseManager::instance().insertUser(reg_username,reg_password))
    {
        QMessageBox::information(this, "成功", "用户注册成功");
    } else {
        QMessageBox::critical(this,"失败","用户注册失败");
    }
}

void regdialog::on_update_password_Btn_clicked()
{
    QString current_username = ui->regusername->text();
    QString new_password = ui->regpassword->text();

    if (current_username.isEmpty() || new_password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名和新密码都不能为空");
        return;
    }

    if (DatabaseManager::instance().updateUser(current_username, current_username, new_password)) {
        QMessageBox::information(this, "成功", "密码修改成功");
    } else {
        QMessageBox::information(this, "提示", "未找到该用户或修改失败");
    }
}
