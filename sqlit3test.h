#ifndef SQLIT3TEST_H
#define SQLIT3TEST_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QMessageBox>

namespace Ui {
class sqlit3test;
}

class sqlit3test : public QMainWindow
{
    Q_OBJECT

public:
    explicit sqlit3test(QWidget *parent = nullptr);
    ~sqlit3test();

    // 【修改1】用引用接收数据库（避免指针类型不匹配）
    void setDatabase(const QSqlDatabase& db);

private slots:
    void on_insertBtn_clicked();

    void on_delBtn_clicked();

    void on_changeBtn_clicked();

    void on_selectBtn_clicked();

    void updateTable();
private:
    Ui::sqlit3test *ui;
    QSqlDatabase sqldatabase;
};

#endif // SQLIT3TEST_H
