#ifndef SECOND_H
#define SECOND_H

#include <QMainWindow>
#include <QGraphicsDropShadowEffect>
#include <QCameraInfo>
#include <QCamera>
#include <QVideoWidget>
#include <QDebug>
#include <QCameraImageCapture>
#include <QThread>
#include <QDir>
#include <QDateTime>

namespace Ui {
class second;
}

class second : public QMainWindow
{
    Q_OBJECT

public:
    explicit second(QWidget *parent = nullptr);
    ~second();

private slots:
    void on_BackToFirst_Button_clicked();

    void on_openCameraBtn_clicked();

    void on_closeCameraBtn_clicked();

    void on_takePhoto_clicked();

    // 新增槽函数，用于处理图像捕获保存完成的信号
    void onImageSaved(int id, const QString &fileName);

private:
    Ui::second *ui;
    QCamera *mycamera = nullptr;  // 初始化一下，避免野指针
    QCameraImageCapture *imageCapture = nullptr;  // 新增图像捕获对象成员

    // 添加阴影设置辅助函数
    void setupShadowEffect(QWidget *widget,
                           const QPointF &offset = QPointF(-3, 0),
                           const QColor &color = QColor("#888888"),
                           int blurRadius = 30);
};

#endif // SECOND_H
