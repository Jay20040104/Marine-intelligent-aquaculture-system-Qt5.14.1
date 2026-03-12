#ifndef REGDIALOG_H
#define REGDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>

namespace Ui {
class regdialog;
}

class regdialog : public QDialog
{
    Q_OBJECT

public:
    explicit regdialog(QWidget *parent = nullptr);
    ~regdialog();

    // 【修改1】用引用接收数据库（避免指针类型不匹配）
    void setDatabase(const QSqlDatabase& db);


private slots:
    void on_submit_Btn_clicked();

    void on_update_password_Btn_clicked();

private:
    Ui::regdialog *ui;
    QSqlDatabase sqldatabase;
};

#endif // REGDIALOG_H
