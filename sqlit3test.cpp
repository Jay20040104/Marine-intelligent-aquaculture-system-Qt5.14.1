#include "sqlit3test.h"
#include "ui_sqlit3test.h"
#include "DatabaseManager.h"

sqlit3test::sqlit3test(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::sqlit3test)
{
    ui->setupUi(this);

    //信息展示表格
    ui->userTable->setColumnCount(5);  // 修改为5列
    QStringList headers;
    headers << "索引" << "图标" << "记住标志" << "用户名" << "密码";
    ui->userTable->setHorizontalHeaderLabels(headers);
    // 可按需设置表格属性，比如允许整行选择
    ui->userTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 设置图标列的宽度（可选）
    ui->userTable->setColumnWidth(1, 50);  // 图标列宽度设为50像素
}

sqlit3test::~sqlit3test()
{
    delete ui;
}

// 【新增】接收数据库对象
void sqlit3test::setDatabase(const QSqlDatabase& db) {
    sqldatabase = db; // 直接赋值（QSqlDatabase 支持值拷贝）
}

void sqlit3test::on_insertBtn_clicked()
{
    //获取输入内容
    QString reg_username = ui->regUsername->text();
    QString reg_password = ui->regPassword->text();

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
        updateTable();
    } else {
        QMessageBox::critical(this,"失败","用户注册失败");
    }
}

void sqlit3test::on_delBtn_clicked()
{
    //获取被删除用户名
    QString del_username = ui->delUsername->text();

    //检查输入内容
    if(del_username.isEmpty()) {
        QMessageBox::warning(this,"警告","请输入要删除的用户名");
        return;
    }

    if (DatabaseManager::instance().deleteUser(del_username)) {
        QMessageBox::information(this, "成功", "数据删除成功");
        updateTable();
    } else {
        QMessageBox::information(this, "提示", "未找到该用户或删除失败");
    }

}

void sqlit3test::on_changeBtn_clicked()
{
    QString old_username = ui->oldUsername->text();
    QString new_username = ui->newUsername->text();
    QString new_password = ui->newPassword->text();
    if (old_username.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入旧用户名");
        return;
    }

    if (new_username.isEmpty() || new_password.isEmpty()) {
        QMessageBox::warning(this, "警告", "新用户名和新密码都不能为空");
        return;
    }

    if (DatabaseManager::instance().updateUser(old_username, new_username, new_password)) {
        QMessageBox::information(this, "成功", "数据修改成功");
        updateTable();
    } else {
        QMessageBox::information(this, "提示", "未找到该用户或修改失败");
    }
}

void sqlit3test::on_selectBtn_clicked()
{
        updateTable();
}

void sqlit3test::updateTable() {
    ui->userTable->setRowCount(0);
    ui->userTable->setIconSize(QSize(32, 32));

    const auto users = DatabaseManager::instance().getAllUsers();
    if (users.isEmpty()) {
        QMessageBox::information(this, "查询结果", "表中无数据");
        return;
    }

    int rowIndex = 1;
    for (const auto& user : users) {
        int row = ui->userTable->rowCount();
        ui->userTable->insertRow(row);

        // 索引列
        ui->userTable->setItem(row, 0, new QTableWidgetItem(QString::number(rowIndex++)));

        // 图标列
        QIcon icon(":/first/common_user_icon.png");
        ui->userTable->setItem(row, 1, new QTableWidgetItem(icon, ""));

        // 记住标志列
        ui->userTable->setItem(row, 2, new QTableWidgetItem(user["marked"].toString()));

        // 用户名列
        ui->userTable->setItem(row, 3, new QTableWidgetItem(user["username"].toString()));

        // 密码列
        ui->userTable->setItem(row, 4, new QTableWidgetItem(user["password"].toString()));
    }
}
