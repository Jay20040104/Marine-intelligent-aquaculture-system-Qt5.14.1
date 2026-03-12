#ifndef SYSTEM_MAIN_PAGE_H
#define SYSTEM_MAIN_PAGE_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include <QDebug>

#include <QTimer>    // 定时器
#include <QDateTime> // 时间处理

#include <QTime>
#include <QRandomGenerator>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include <QCameraInfo>
#include <QCamera>
#include <QVideoWidget>
#include <QCameraImageCapture>
#include <QThread>
#include <QDir>

#include "qcustomplot.h" // QCustomPlot 头文件

#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

#include <QWidget>
#include <QVector>
#include <QMap>
#include "DataBaseManager.h"

namespace Ui {
class System_Main_Page;
}

struct Goods {
    QString name;
    double price;
    int stock;

    // 添加构造函数
    Goods(const QString& name, double price, int stock)
        : name(name), price(price), stock(stock) {}
};

struct CartItem {
    QString name;
    double price;
    int quantity;
    double total;

    // 添加构造函数
    CartItem(const QString& name, double price, int quantity)
        : name(name), price(price), quantity(quantity), total(price * quantity) {}
};

class System_Main_Page : public QMainWindow
{
    Q_OBJECT

public:
    //explicit System_Main_Page(QWidget *parent = nullptr);
    explicit System_Main_Page(const QString& username, QWidget *parent = nullptr);
    ~System_Main_Page();

private slots:
    void on_change_page_itemClicked(QTreeWidgetItem *item, int column); // 切换页面函数

    void updateDateTime(); // 定时更新时间的槽函数
    void fetchWeather(); // 发起天气请求
    void onWeatherReply(QNetworkReply* reply); // 处理天气响应

    void updateSensorData(); // 定时更新时间的槽函数
    void refreshDeviceTable(); // 刷新设备表格
    void updateDeviceStatus(); // 更新设备状态

    void on_openCameraBtn_clicked(); // 打开摄像头
    void on_closeCameraBtn_clicked(); // 关闭摄像头
    void on_takePhoto_clicked(); // 拍照
    void onImageSaved(int id, const QString &fileName); // 新增槽函数，用于处理图像捕获保存完成的信号

    void mian_page_init(); // 主界面相关
    void env_page_init(); // 环境信息界面相关
    void minitor_page_init(); // 监控信息

    void setNeumorphic(); // 拟态风格设置

    void newClientLink();

    void recvClientMsg();

    void clientUnlink();

    // 新增槽函数
    void buy_page_init();             // 初始化购买页面
    void addToCart(int goodsIndex);   // 添加到购物车
    void removeFromCart(int cartIndex); // 从购物车移除
    void updateCartTotal();           // 更新购物车总价
    void on_checkout_btn_clicked();   // 结算按钮点击
    void updateGoodsTable();


    void on_server_Bind_Button_clicked();

    void on_client_Bind_Button_clicked();

    void on_Connect_Button_clicked();

    void on_Sendc_Button_clicked();
    void recvServerMsg();
    void clearAllConnections();
private:
    Ui::System_Main_Page *ui;

    QTimer *datetimer; //日期定时器对象

    QNetworkAccessManager *networkManager;  // 天气请求管理器

    QString seniverseApiKey = "SIcYiyR-7LriIA27E"; // 替换为你的心知天气 API Key

    QTimer *SensorDatatimer; // 环境参数定时器

    // 设备状态相关
    struct DeviceInfo {
        QString name;       // 设备名称
        QString status;     // 状态（正常/停止/维护/故障）
        QString time;       // 检测时间
    };
    QList<DeviceInfo> deviceList;    // 设备列表
    QTimer *deviceStatusTimer;       // 设备状态更新定时器

    QCamera *mycamera = nullptr;     // 摄像头对象
    QCameraImageCapture *imageCapture = nullptr;  // 拍照对象

    // 环境参数当前值（用于平滑更新）
    double currentTemp = 25.0;       // 水温
    int currentSalinity = 30;        // 盐度
    double currentDO = 6.0;          // 溶氧
    double currentPH = 8.0;          // pH值
    int currentTurbidity = 15;       // 浊度
    double currentAmmonia = 0.1;     // 氨氮

    QTcpServer *tcpserver = nullptr;       // 服务器对象
    QList<QTcpSocket*> serverClients;      // 服务器管理的客户端列表
    QTcpSocket *curServerClient = nullptr; // 当前通信的服务器客户端

    // TCP客户端相关
    QTcpSocket *tcpClient = nullptr;       // 客户端套接字（连接外部服务器）

    // 模式标识（0：未激活，1：服务器模式，2：客户端模式）
    enum { MODE_NONE, MODE_SERVER, MODE_CLIENT } currentMode = MODE_NONE;

    QString m_username; // 当前登录用户（用于关联订单）

    // 新增成员变量
    QVector<Goods> m_goodsList;       // 商品列表
    QVector<CartItem> m_cartList;     // 购物车列表
    double m_totalPrice;              // 总价

};

#endif // SYSTEM_MAIN_PAGE_H
