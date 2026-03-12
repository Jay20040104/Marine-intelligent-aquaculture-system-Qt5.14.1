#include "second.h"
#include "ui_second.h"
#include <QMessageBox>

second::second(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::second)
{
    ui->setupUi(this);

    // 使用辅助函数设置阴影
    setupShadowEffect(ui->sensorDataShow);
    setupShadowEffect(ui->camContainer);

    //获取当前系统所有可用摄像头信息
    QList<QCameraInfo> cameralist = QCameraInfo::availableCameras();

    //遍历容器打印摄像头信息
    for (QCameraInfo n:cameralist) {
        qDebug()<<"摄像头描述信息："<<n.description();
        qDebug()<<"摄像头设备名字："<<n.deviceName();

        //下拉框显示摄像头设备名字
        ui->chooseCamera->addItem(n.deviceName());
    }

}

second::~second()
{
    delete ui;
}

// 阴影设置辅助函数实现
void second::setupShadowEffect(QWidget *widget, const QPointF &offset, const QColor &color, int blurRadius)
{
    if (!widget) return; // 防止空指针

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setOffset(offset);
    shadow->setColor(color);
    shadow->setBlurRadius(blurRadius);
    widget->setGraphicsEffect(shadow);
}


void second::on_BackToFirst_Button_clicked()
{
    this->parentWidget()->show();
    this->close();
}

void second::on_openCameraBtn_clicked()
{
    QString name = ui->chooseCamera->currentText();

    // 确保之前的相机已停止（避免资源泄漏）
    if (mycamera && mycamera->status() == QCamera::ActiveStatus) {
        mycamera->stop();
        delete mycamera;
    }

    mycamera = new QCamera(name.toUtf8(),this);

    QVideoWidget *win = new QVideoWidget(ui->cameraShow);

    win->setObjectName("cameraViewfinder"); // 设置对象名，便于后续识别和管理

    // 关键：设置视频显示模式为拉伸填充（保持宽高比可能会留有黑边）
    win->setAspectRatioMode(Qt::IgnoreAspectRatio);

    // 设置为可扩展大小策略，确保能填满父容器
    win->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 清除旧的视频部件（如果有）
    QLayout *layout = ui->cameraShow->layout();
    if (!layout) {
        // 如果没有布局，创建一个垂直布局
        layout = new QVBoxLayout(ui->cameraShow);
        layout->setContentsMargins(0, 0, 0, 0); // 消除边距
        ui->cameraShow->setLayout(layout);
    } else {
        // 清除布局中的旧部件
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }

    // 将视频部件添加到布局中
    layout->addWidget(win);

    mycamera->setViewfinder(win);

    // 初始化图像捕获对象（在相机创建后初始化）
    if (imageCapture) {
        delete imageCapture;
    }
    imageCapture = new QCameraImageCapture(mycamera, this);
    connect(imageCapture, &QCameraImageCapture::imageSaved, this, &second::onImageSaved);


    mycamera->start();
}

void second::on_closeCameraBtn_clicked()
{
    if (mycamera) {
        mycamera->stop();
        // 清除显示区域
        QLayout *layout = ui->cameraShow->layout();
        if (layout) {
            QLayoutItem *item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
        }
    }
}

void second::on_takePhoto_clicked()
{
    if (!mycamera || mycamera->status() != QCamera::ActiveStatus) {
        QMessageBox::warning(this, "提示", "相机未正常开启，请先开启相机！");
        return;
    }

    // ========== 路径推导 + 转绝对路径 ==========
    QString currentFile = __FILE__;
    qDebug() << "当前文件路径：" << currentFile;

    int projectIndex = currentFile.lastIndexOf("Project");
    QString projectRoot = currentFile.left(projectIndex + QString("Project").length());

    // 转绝对路径
    QFileInfo fileInfo(projectRoot);
    QString absoluteProjectRoot = fileInfo.absoluteFilePath();
    qDebug() << "推导的项目根路径（绝对）：" << absoluteProjectRoot;

    QString saveDir = absoluteProjectRoot + "/photos/";
    // ========================================

    QDir dir(saveDir);
    bool createSuccess = true;
    if (!dir.exists()) {
        createSuccess = dir.mkpath(".");
        if (!createSuccess) {
            QMessageBox::critical(this, "错误", "创建保存图片的 photos 文件夹失败，请检查权限！路径：" + saveDir);
            return;
        } else {
            qDebug() << "已自动创建 photos 文件夹，路径：" << saveDir;
        }
    }

    QString fileName = saveDir + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz.jpg");

    // ========== 检查 imageCapture ==========
    if (!imageCapture) {
        QMessageBox::critical(this, "错误", "图像捕获对象未初始化！");
        return;
    }
    // ========================================

    // 执行捕获
    if (!imageCapture->capture(fileName)) {
        QMessageBox::warning(this, "警告", "拍照失败，请重试");
    }
}

void second::onImageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);

    QPixmap pixmap(fileName);
    if (pixmap.isNull()) {
        QMessageBox::warning(this, "错误", "加载照片失败: " + fileName);
        return;
    }

    qDebug() << "图片保存路径：" << fileName << "，加载结果：" << "成功";

    // ========== 强制刷新 UI ==========
    if (ui->picShow) {
        pixmap = pixmap.scaled(ui->picShow->size(),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
        ui->picShow->setPixmap(pixmap);
        ui->picShow->setScaledContents(true);
        ui->picShow->update(); // 强制刷新
    } else {
        qDebug() << "显示标签未找到！";
    }
    // ========================================
}
