#include "system_main_page.h"
#include "ui_system_main_page.h"

#include "neumorphic.h"
#include <QJsonArray>
#include <QSslSocket>

#include "CircleAvatar.h"
#include <QPixmap>
#include <QIcon>

#include <QRandomGenerator> // 随机数
#include <QTableWidgetItem>
#include <QHeaderView>

#include "qtmaterialtoggle.h"

System_Main_Page::System_Main_Page(const QString& username, QWidget *parent):
//System_Main_Page::System_Main_Page(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::System_Main_Page)
{
    ui->setupUi(this);

    setNeumorphic();

    QtMaterialToggle *mtoggleA = new QtMaterialToggle(ui->mtoggleA);
    mtoggleA->setGeometry(0,0,50,38);
    mtoggleA->setInactiveColor(Qt::red);

    QtMaterialToggle *mtoggleB = new QtMaterialToggle(ui->mtoggleB);
    mtoggleB->setGeometry(0,0,50,38);
    mtoggleB->setInactiveColor(Qt::red);

    QtMaterialToggle *mtoggleC = new QtMaterialToggle(ui->mtoggleC);
    mtoggleC->setGeometry(0,0,50,38);
    mtoggleC->setInactiveColor(Qt::red);

    QtMaterialToggle *mtoggleD = new QtMaterialToggle(ui->mtoggleD);
    mtoggleD->setGeometry(0,0,50,38);
    mtoggleD->setInactiveColor(Qt::red);

    // 窗口设置标题
    this->setWindowTitle(QStringLiteral("智能海洋水产养殖系统"));
    QIcon icon(":/first/system_icon.png");
    // 窗口设置图标
    this->setWindowIcon(icon);

    //主界面相关
    mian_page_init();

    //环境信息界面相关
    env_page_init();

    // 监控信息
    minitor_page_init();

    buy_page_init();  // 新增

    m_username = username; // 存储登录用户名

    qDebug() << "传过来的登录用户名为：" << m_username;


    qDebug()<<QSslSocket::sslLibraryBuildVersionString();
    //看看是否配置了ssl
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    qDebug() << manager->supportedSchemes();
}

System_Main_Page::~System_Main_Page()
{
    delete ui;
}

void System_Main_Page::buy_page_init() {
    qDebug() << "开始初始化商品购买页面...";
    // 1. 商品表格初始化（4列）
    ui->goods_table->setColumnCount(4); // 显式设置列数
    QStringList goodsHeaders = {"商品名称", "单价(元)", "库存", "操作"};
    ui->goods_table->setHorizontalHeaderLabels(goodsHeaders); // 设置表头
    ui->goods_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列自动拉伸

    // 2. 购物车表格初始化（5列）
    ui->cart_table->setColumnCount(5); // 显式设置列数
    QStringList cartHeaders = {"商品名称", "单价", "数量", "小计", "操作"};
    ui->cart_table->setHorizontalHeaderLabels(cartHeaders); // 设置表头
    ui->cart_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列自动拉伸


    // 1. 初始化商品列表
    m_goodsList = {
        {"草鱼", 15.8, 100},
        {"鲈鱼", 28.5, 50},
        {"对虾", 32.0, 80},
        {"扇贝", 12.5, 200},
        {"鱿鱼", 25.0, 60}
    };

    // 2. 设置商品表格
    ui->goods_table->setRowCount(m_goodsList.size());
    ui->goods_table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->goods_table->verticalHeader()->setVisible(false);
    for (int i = 0; i < m_goodsList.size(); ++i) {
        const Goods& goods = m_goodsList[i];
        // 商品名称（居中）
        QTableWidgetItem* nameItem = new QTableWidgetItem(goods.name);
        nameItem->setTextAlignment(Qt::AlignCenter); // 关键：文字水平+垂直居中
        ui->goods_table->setItem(i, 0, nameItem);

        // 单价（居中）
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(goods.price, 'f', 1));
        priceItem->setTextAlignment(Qt::AlignCenter);
        ui->goods_table->setItem(i, 1, priceItem);

        // 库存（居中）
        QTableWidgetItem* stockItem = new QTableWidgetItem(QString::number(goods.stock));
        stockItem->setTextAlignment(Qt::AlignCenter);
        ui->goods_table->setItem(i, 2, stockItem);

        // 添加按钮
        QPushButton* addBtn = new QPushButton("加入购物车");
        addBtn->setObjectName(QString("add_%1").arg(i)); // 标记商品索引
        connect(addBtn, &QPushButton::clicked, this, [=]() {
            addToCart(i); // 绑定添加事件
        });
        ui->goods_table->setCellWidget(i, 3, addBtn);
    }

    // 3. 初始化购物车表格
    ui->cart_table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->cart_table->verticalHeader()->setVisible(false);
    m_totalPrice = 0;
    ui->total_price->setText(QString("总价：¥0.0"));

    // 4. 设置拟态风格
    Neumorphic* goods_table_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal, ui->goods_table);
    ui->goods_table->setGraphicsEffect(goods_table_neum);
    Neumorphic* cart_table_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal, ui->cart_table);
    ui->cart_table->setGraphicsEffect(cart_table_neum);
    Neumorphic* checkout_btn_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal, ui->checkout_btn);
    ui->checkout_btn->setGraphicsEffect(checkout_btn_neum);
}

void System_Main_Page::updateGoodsTable() {
    for (int i = 0; i < m_goodsList.size(); ++i) {
        const Goods& goods = m_goodsList[i];
        // 更新库存显示，库存为0时标红
        QTableWidgetItem* stockItem = ui->goods_table->item(i, 2);
        if (stockItem) {
            stockItem->setText(QString::number(goods.stock));
            if (goods.stock == 0) {
                stockItem->setForeground(QColor("#FF4C4C")); // 库存为0时文字标红
            } else {
                stockItem->setForeground(QColor("#000000")); // 正常库存黑色
            }
        }

        // 库存为0时禁用添加按钮
        QPushButton* addBtn = qobject_cast<QPushButton*>(ui->goods_table->cellWidget(i, 3));
        if (addBtn) {
            addBtn->setEnabled(goods.stock > 0);
            if (goods.stock == 0) {
                addBtn->setText("库存不足"); // 明确提示无法购买
            } else {
                addBtn->setText("加入购物车");
            }
        }
    }
}

void System_Main_Page::addToCart(int goodsIndex) {
    if (goodsIndex < 0 || goodsIndex >= m_goodsList.size()) return;
    Goods& goods = m_goodsList[goodsIndex];

    // 检查库存（核心：只要库存>0就允许加入）
    if (goods.stock <= 0) {
        QMessageBox::warning(this, "提示", "商品已售罄！");
        return;
    }

    // 检查购物车中是否已有该商品
    for (int i = 0; i < m_cartList.size(); ++i) {
        if (m_cartList[i].name == goods.name) {
            // 已存在则数量+1（直接基于库存判断，而非购物车数量与库存的大小）
            m_cartList[i].quantity++;
            m_cartList[i].total = m_cartList[i].price * m_cartList[i].quantity;
            goods.stock--; // 减少库存（核心：只要库存>0就允许减1）
            updateGoodsTable();
            updateCartTotal();
            return;
        }
    }

    // 新增商品到购物车（库存已在上方检查过>0）
    m_cartList.append(CartItem(goods.name, goods.price, 1));
    goods.stock--;
    updateGoodsTable();
    updateCartTotal();
}

void System_Main_Page::removeFromCart(int cartIndex) {
    if (cartIndex >= 0 && cartIndex < m_cartList.size()) {
        const CartItem& item = m_cartList[cartIndex];

        // 找到对应的商品并增加库存
        for (int i = 0; i < m_goodsList.size(); ++i) {
            if (m_goodsList[i].name == item.name) {
                m_goodsList[i].stock += item.quantity;
                break;
            }
        }

        m_cartList.removeAt(cartIndex);
        updateGoodsTable(); // 更新商品表格
        updateCartTotal(); // 更新购物车UI
    }
}

void System_Main_Page::updateCartTotal() {
    // 1. 刷新购物车表格
    ui->cart_table->setRowCount(m_cartList.size());
    m_totalPrice = 0;

    for (int i = 0; i < m_cartList.size(); ++i) {
        const CartItem& item = m_cartList[i];

        // 商品名称（居中）
        QTableWidgetItem* nameItem = new QTableWidgetItem(item.name);
        nameItem->setTextAlignment(Qt::AlignCenter);
        ui->cart_table->setItem(i, 0, nameItem);

        // 单价（居中）
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(item.price, 'f', 1));
        priceItem->setTextAlignment(Qt::AlignCenter);
        ui->cart_table->setItem(i, 1, priceItem);

        // 数量（居中）
        QTableWidgetItem* quantityItem = new QTableWidgetItem(QString::number(item.quantity));
        quantityItem->setTextAlignment(Qt::AlignCenter);
        ui->cart_table->setItem(i, 2, quantityItem);

        // 小计
        ui->cart_table->setItem(i, 3, new QTableWidgetItem(QString::number(item.total, 'f', 1)));

        // 删除按钮
        QPushButton* delBtn = new QPushButton("删除");
        delBtn->setObjectName(QString("del_%1").arg(i));
        connect(delBtn, &QPushButton::clicked, this, [=]() {
            removeFromCart(i);
        });
        ui->cart_table->setCellWidget(i, 4, delBtn);

        m_totalPrice += item.total;
    }

    // 2. 更新总价显示
    ui->total_price->setText(QString("总价：¥%1").arg(m_totalPrice, 0, 'f', 1));

    // 如果购物车为空，清空表格
    if (m_cartList.isEmpty()) {
        ui->cart_table->setRowCount(0);
        ui->total_price->setText("总价：¥0.0");
    }
}

void System_Main_Page::on_checkout_btn_clicked() {
    if (m_cartList.isEmpty()) {
        QMessageBox::information(this, "提示", "购物车为空！");
        return;
    }

    // 简单结算逻辑（实际项目中可连接支付接口）
    QString orderInfo = QString("用户：%1\n\n").arg(m_username);
    orderInfo += "订单商品：\n";
    for (const auto& item : m_cartList) {
        orderInfo += QString("- %1 x%2 ：¥%3\n").arg(item.name).arg(item.quantity).arg(item.total);
    }
    orderInfo += QString("\n总计：¥%1").arg(m_totalPrice, 0, 'f', 1);

    QMessageBox::information(this, "结算成功", orderInfo);

    // 清空购物车并更新UI
    m_cartList.clear();
    updateCartTotal();

    // 重置商品库存状态
    updateGoodsTable();
}

void System_Main_Page::mian_page_init(){
    // 设置主界面用户图片（路径替换为你的资源）
    QPixmap avatarPix(":/first/user_photo.jpg");
    ui->User_photo->setImage(avatarPix);

    // 1. 创建定时器，设置“1秒触发一次”
    datetimer = new QTimer(this);
    connect(datetimer, &QTimer::timeout, this, &System_Main_Page::updateDateTime);
    datetimer->start(1000); // 间隔 1000ms（1秒）

    // 2. 初始化时立即更新一次（避免首次显示延迟）
    updateDateTime();

    // 2. 天气请求初始化（网络管理器）
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &System_Main_Page::onWeatherReply);

    // 3. 启动天气请求（可调整频率，免费版建议 10~30 分钟一次）
    fetchWeather();

}

void System_Main_Page::fetchWeather() {
    // 心知天气 API 构造（以“北京”为例，可动态替换城市）
    QString city = "guangzhou"; // 或经纬度（如 "39.9042,116.4074" ）
    QString url = QString(
                      "https://api.seniverse.com/v3/weather/now.json?"
                      "key=%1&location=%2&language=zh-Hans&unit=c"
                      ).arg(seniverseApiKey).arg(city);


    QNetworkRequest request;
    request.setUrl(QUrl(url));
    networkManager->get(request); // 发起 GET 请求
}

void System_Main_Page::onWeatherReply(QNetworkReply* reply) {
    // 1. 处理网络错误
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "天气请求失败：" << reply->errorString();
        ui->Weather->setText("天气获取失败");
        reply->deleteLater();
        return;
    }

    // 2. 解析 JSON 数据
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() ||!doc.isObject()) {
        ui->Weather->setText("天气解析失败");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray results = root["results"].toArray();
    if (results.isEmpty()) {
        ui->Weather->setText("无天气数据");
        return;
    }

    // 3. 提取关键信息（城市、天气、温度）
    QJsonObject result = results[0].toObject();
    QJsonObject location = result["location"].toObject();
    QJsonObject now = result["now"].toObject();

    QString cityName = location["name"].toString();   // 城市名
    QString weatherText = now["text"].toString();     // 天气状况（如“多云”）
    QString weatherCode = now["code"].toString();     // 天气状况代码（如0），用于判断显示图标
    QString temp = now["temperature"].toString() + "℃"; // 温度

    // 2. 拼接图标资源路径（根据状态码动态生成）
    // 资源路径格式：:/first/black/[状态码]@1x.png
    QString iconPath = ":/first/black/" + weatherCode + "@1x.png";
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull()) {
        ui->Weather_icon->setPixmap(pixmap);
        ui->Weather_icon->setScaledContents(true);
    }

    // 4. 更新 UI（示例格式：北京市 多云 24℃）
    ui->Weather->setText(QString("%1\n%2 %3").arg(cityName).arg(weatherText).arg(temp));
}

void System_Main_Page::updateDateTime() {
    QDateTime now = QDateTime::currentDateTime();

    // 格式化日期（如：2025年7月5日星期六）
    QString dateFormat = "yyyy年M月d日dddd";
    ui->Date->setText(now.toString(dateFormat));

    // 格式化时间（如：20:50:45）
    QString timeFormat = "HH:mm:ss";
    ui->Time->setText(now.toString(timeFormat));
}

// 树控件点击事件
void System_Main_Page::on_change_page_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column); // 标记参数未使用，消除警告

    // 获取当前点击项的文字（以“点击列”为例，若固定取第1列，可传 0）
    QString itemText = item->text(0);
    // 打印文字
    qDebug() << "切换页面：" << itemText;

    // 获取当前点击项的索引（从0开始）
    int index = ui->change_page->indexOfTopLevelItem(item);

    // 设置 page_stackedWidget 显示对应索引的页面
    ui->page_stackedWidget->setCurrentIndex(index);

    if(itemText == "退出登录"){
        this->parentWidget()->show();
        this->close();
    }
}

void System_Main_Page::minitor_page_init(){
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

void System_Main_Page::on_openCameraBtn_clicked()
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
    connect(imageCapture, &QCameraImageCapture::imageSaved, this, &System_Main_Page::onImageSaved);


    mycamera->start();
}

void System_Main_Page::on_closeCameraBtn_clicked()
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

void System_Main_Page::on_takePhoto_clicked()
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

void System_Main_Page::onImageSaved(int id, const QString &fileName)
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

// 2. 定时生成随机数并更新环境信息UI
void System_Main_Page::env_page_init(){
    // 环境信息

    SensorDatatimer = new QTimer(this);
    connect(SensorDatatimer, &QTimer::timeout, this, &System_Main_Page::updateSensorData);
    SensorDatatimer->start(1000); // 间隔 1000ms（1秒）

    // ========== 1. 基础配置 ==========
    ui->sensor_data_list->setColumnCount(3); // 3 列
    ui->sensor_data_list->setHorizontalHeaderLabels(
        {"设备名称", "设备状态", "检测时间"} );

    ui->sensor_data_list->setColumnWidth(0,120);
    ui->sensor_data_list->setColumnWidth(1,100);
    ui->sensor_data_list->setColumnWidth(2,164);

    // 2. 表头文字居中（关键代码）
    ui->sensor_data_list->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 隐藏行号（可选，更简洁）
    ui->sensor_data_list->verticalHeader()->setVisible(false);

    // ========== 初始化设备列表（模拟8个设备）==========
    deviceList = {
        {"温度检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:ss")},
        {"温度检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:ss")},
        {"pH检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:ss")},
        {"pH检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:ss")},
        {"溶氧检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")},
        {"盐度检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")},
        {"浊度检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")},
        {"氨氮检测设备", "正常", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")}
    };

    // 隐藏水平滚动条（下边）
    ui->sensor_data_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 隐藏垂直滚动条（右边）
    ui->sensor_data_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // ========== 关键：启用排序 + 设置默认排序列 ==========
    ui->sensor_data_list->setSortingEnabled(true); // 启用排序功能
    // 按检测时间列（索引2）升序排序（最新时间在底部可改降序）
    ui->sensor_data_list->horizontalHeader()->setSortIndicator(2, Qt::AscendingOrder);

    // 初始化表格数据（替换原有的静态异常数据）
    refreshDeviceTable();

    // ========== 启动设备状态更新定时器（每5秒更新一次）==========
    deviceStatusTimer = new QTimer(this);
    connect(deviceStatusTimer, &QTimer::timeout, this, &System_Main_Page::updateDeviceStatus);
    deviceStatusTimer->start(1000);  // 5秒一次
}

// 环境参数更新（带异常标记）
void System_Main_Page::updateSensorData() {
    // 1. 水温（正常范围：20~32℃）
    currentTemp += QRandomGenerator::global()->bounded(-1, 2) * 0.1;  // ±0.1℃波动
    currentTemp = qBound(10.0, currentTemp, 40.0);  // 限制范围
    bool tempAbnormal = (currentTemp < 20 || currentTemp > 32);
    ui->temp_data->setText(QString::number(currentTemp, 'f', 1) + "℃");
    ui->temp_data->setStyleSheet(tempAbnormal ? "color: #FF4C4C; font-weight: bold;" : "color: rgb(86, 86, 86);");

    // 2. 盐度（正常范围：20~35‰）
    currentSalinity += QRandomGenerator::global()->bounded(-1, 2);  // ±1波动
    currentSalinity = qBound(10, currentSalinity, 40);
    bool saltAbnormal = (currentSalinity < 20 || currentSalinity > 35);
    ui->salt_data->setText(QString::number(currentSalinity) + "‰");
    ui->salt_data->setStyleSheet(saltAbnormal ? "color: #FF4C4C; font-weight: bold;" : "color: rgb(86, 86, 86);");

    // 3. 溶氧（正常范围：5~8mg/L）
    currentDO += QRandomGenerator::global()->bounded(-1, 2) * 0.05;  // ±0.05波动
    currentDO = qBound(2.0, currentDO, 10.0);
    bool doAbnormal = (currentDO < 5 || currentDO > 8);
    ui->DO_data->setText(QString::number(currentDO, 'f', 1) + "mg/L");
    ui->DO_data->setStyleSheet(doAbnormal ? "color: #FF4C4C; font-weight: bold;" : "color: rgb(86, 86, 86);");

    // 4. pH值（正常范围：7.5~8.5）
    currentPH += QRandomGenerator::global()->bounded(-1, 2) * 0.02;  // ±0.02波动
    currentPH = qBound(6.0, currentPH, 10.0);
    bool phAbnormal = (currentPH < 7.5 || currentPH > 8.5);
    ui->PH_data->setText(QString::number(currentPH, 'f', 1));
    ui->PH_data->setStyleSheet(phAbnormal ? "color: #FF4C4C; font-weight: bold;" : "color: rgb(86, 86, 86);");

    // 5. 浊度（正常范围：0~20NTU）
    currentTurbidity += QRandomGenerator::global()->bounded(-1, 2);  // ±1波动
    currentTurbidity = qBound(0, currentTurbidity, 30);
    bool turbidityAbnormal = (currentTurbidity > 20);
    ui->Turbidity_data->setText(QString::number(currentTurbidity) + "NTU");
    ui->Turbidity_data->setStyleSheet(turbidityAbnormal ? "color: #FF4C4C; font-weight: bold;" : "color: rgb(86, 86, 86);");

    // 6. 氨氮（正常范围：0~0.2mg/L）
    currentAmmonia += QRandomGenerator::global()->bounded(-1, 2) * 0.01;  // ±0.01波动
    currentAmmonia = qBound(0.0, currentAmmonia, 0.3);
    bool ammoniaAbnormal = (currentAmmonia > 0.2);
    ui->ammonia_data->setText(QString::number(currentAmmonia, 'f', 2) + "mg/L");
    ui->ammonia_data->setStyleSheet(ammoniaAbnormal ? "color: #FF4C4C; font-weight: bold;" : "color: rgb(86, 86, 86);");
}

void System_Main_Page::refreshDeviceTable() {
    // 清空表格现有数据（保留表头）
    ui->sensor_data_list->clearContents();
    ui->sensor_data_list->setRowCount(deviceList.size());

    // 填充数据到表格
    for (int row = 0; row < deviceList.size(); ++row) {
        const DeviceInfo& info = deviceList[row];

        // 设备名称
        QTableWidgetItem* nameItem = new QTableWidgetItem(info.name);
        nameItem->setTextAlignment(Qt::AlignCenter);
        ui->sensor_data_list->setItem(row, 0, nameItem);

        // 设备状态（故障时文字变红）
        QTableWidgetItem* statusItem = new QTableWidgetItem(info.status);
        statusItem->setTextAlignment(Qt::AlignCenter);
        if (info.status == "故障") {
            statusItem->setForeground(QColor("#FF4C4C"));  // 红色文字
        }
        ui->sensor_data_list->setItem(row, 1, statusItem);

        // 更新时间
        QTableWidgetItem* timeItem = new QTableWidgetItem(info.time);
        timeItem->setTextAlignment(Qt::AlignCenter);
        ui->sensor_data_list->setItem(row, 2, timeItem);
    }

     ui->sensor_data_list->scrollToBottom();
}

void System_Main_Page::updateDeviceStatus() {
    // 1. 定义可能的状态和设备名称
    QStringList statusList = {"正常", "停止", "维护", "故障"};  // 状态池
    QStringList deviceNames = {"温度检测设备", "pH检测设备", "溶氧检测设备", "盐度检测设备", "浊度检测设备", "氨氮检测设备"};

    // 2. 随机选择一个设备（或新增设备记录）
    bool addNew = (QRandomGenerator::global()->bounded(3) == 0);  // 33%概率新增记录，67%概率修改现有记录
    if (addNew && deviceList.size() < 10) {  // 最多10条记录，避免表格过长
        // 新增一条设备记录
        DeviceInfo newDevice;
        newDevice.name = deviceNames[QRandomGenerator::global()->bounded(deviceNames.size())];
        newDevice.status = statusList[QRandomGenerator::global()->bounded(statusList.size())];
        newDevice.time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        deviceList.append(newDevice);
    } else {
        // 修改现有设备的状态和时间
        int index = QRandomGenerator::global()->bounded(deviceList.size());  // 随机选择一条记录
        deviceList[index].status = statusList[QRandomGenerator::global()->bounded(statusList.size())];  // 随机状态
        deviceList[index].time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");  // 当前时间
    }

    // 3. 限制表格最大行数（超过10行则删除最旧的记录）
    if (deviceList.size() > 10) {
        deviceList.removeFirst();  // 删除第一条（最旧的记录）
    }

    // 4. 刷新表格显示
    refreshDeviceTable();
}

void System_Main_Page::setNeumorphic(){
    //主界面
    Neumorphic* base_inf_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->base_inf_widget);
    ui->base_inf_widget->setGraphicsEffect(base_inf_neum);
    Neumorphic* Weather_inf_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Weather_inf);
    ui->Weather_inf->setGraphicsEffect(Weather_inf_neum);
    Neumorphic* User_photo_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->User_photo);
    ui->User_photo->setGraphicsEffect(User_photo_neum);
    Neumorphic* base_inf_widget_1_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->base_inf_widget_1);
    ui->base_inf_widget_1->setGraphicsEffect(base_inf_widget_1_neum);

    //环境信息
    Neumorphic* DO_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->DO);
    ui->DO->setGraphicsEffect(DO_neum);
    Neumorphic* salt_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->salt);
    ui->salt->setGraphicsEffect(salt_neum);
    Neumorphic* temp_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->temp);
    ui->temp->setGraphicsEffect(temp_neum);

    Neumorphic* PH_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->PH);
    ui->PH->setGraphicsEffect(PH_neum);
    Neumorphic* Turbidity_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Turbidity);
    ui->Turbidity->setGraphicsEffect(Turbidity_neum);
    Neumorphic* ammonia_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->ammonia);
    ui->ammonia->setGraphicsEffect(ammonia_neum);
    Neumorphic* sensor_data_list_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->sensor_data_list);
    ui->sensor_data_list->setGraphicsEffect(sensor_data_list_neum);
    Neumorphic* Intelligent_feeding_device_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Intelligent_feeding_device);
    ui->Intelligent_feeding_device->setGraphicsEffect(Intelligent_feeding_device_neum);
    Neumorphic* Oxygenation_equipment_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Oxygenation_equipment);
    ui->Oxygenation_equipment->setGraphicsEffect(Oxygenation_equipment_neum);
    Neumorphic* Water_purification_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Water_purification);
    ui->Water_purification->setGraphicsEffect(Water_purification_neum);
    Neumorphic* Net_regulation_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Net_regulation);
    ui->Net_regulation->setGraphicsEffect(Net_regulation_neum);

    //监控信息
    Neumorphic* camera_widget_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->camera_widget);
    ui->camera_widget->setGraphicsEffect(camera_widget_neum);
    Neumorphic* openCameraBtn_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->openCameraBtn);
    ui->openCameraBtn->setGraphicsEffect(openCameraBtn_neum);
    Neumorphic* closeCameraBtn_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->closeCameraBtn);
    ui->closeCameraBtn->setGraphicsEffect(closeCameraBtn_neum);

    Neumorphic* pic_widget_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->pic_widget);
    ui->pic_widget->setGraphicsEffect(pic_widget_neum);
    Neumorphic* takePhoto_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->takePhoto);
    ui->takePhoto->setGraphicsEffect(takePhoto_neum);

    //网络通信
    Neumorphic* TCP_server_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->TCP_server);
    ui->TCP_server->setGraphicsEffect(TCP_server_neum);
    Neumorphic* TCP_client_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->TCP_client);
    ui->TCP_client->setGraphicsEffect(TCP_client_neum);
    Neumorphic* recive_list_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->recive_list);
    ui->recive_list->setGraphicsEffect(recive_list_neum);
    Neumorphic* send_text_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->send_text);
    ui->send_text->setGraphicsEffect(send_text_neum);
    Neumorphic* Sendc_Button_neum = new Neumorphic(Neumorphic::NeumorphicMode_Normal,ui->Sendc_Button);
    ui->Sendc_Button->setGraphicsEffect(Sendc_Button_neum);
}


// 处理服务器新客户端连接
void System_Main_Page::newClientLink() {
    QTcpSocket *newClient = tcpserver->nextPendingConnection();
    if (!newClient) return;

    // 记录客户端信息
    QString clientInfo = QString("%1:%2").arg(newClient->peerAddress().toString()).arg(newClient->peerPort());
    qDebug() << "新客户端连接：" << clientInfo;
    ui->recive_list->addItem("[服务器] 客户端已连接：" + clientInfo);

    // 添加到客户端列表
    serverClients.append(newClient);
    curServerClient = newClient; // 设置当前通信客户端

    // 关联客户端信号
    connect(newClient, &QTcpSocket::readyRead, this, &System_Main_Page::recvClientMsg);
    connect(newClient, &QTcpSocket::disconnected, this, [=]() {
        qDebug() << "客户端断开：" << clientInfo;
        ui->recive_list->addItem("[服务器] 客户端已断开：" + clientInfo);
        serverClients.removeOne(newClient);
        newClient->deleteLater();
        if (curServerClient == newClient) curServerClient = nullptr;
    });
    connect(newClient, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this, [=]() {
              ui->recive_list->addItem("[服务器] 客户端错误：" + newClient->errorString());
            });

}


// 服务器接收客户端数据
void System_Main_Page::recvClientMsg() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QByteArray data = client->readAll();
    QString msg = QString("[服务器接收] %1:%2：%3")
                      .arg(client->peerAddress().toString())
                      .arg(client->peerPort())
                      .arg(QString(data));
    qDebug() << msg;
    ui->recive_list->addItem(msg);
}

void System_Main_Page::clientUnlink()
{
    // 弹出“关键”提示框，告知客户端已断开
    QMessageBox::critical(this, "断开了", "开发板断开连接了");
}


// 服务器绑定按钮：启动服务器监听
void System_Main_Page::on_server_Bind_Button_clicked() {
    // 绑定前先清理旧连接
    clearAllConnections();

    // 获取服务器IP和端口
    QString bindIp = ui->server_Bind_ip->text().trimmed();
    quint16 bindPort = ui->server_Bind_port->text().toUInt();
    if (bindIp.isEmpty() || bindPort == 0) {
        QMessageBox::warning(this, "错误", "请输入有效的IP和端口");
        return;
    }

    // 创建服务器并监听
    tcpserver = new QTcpServer(this);
    if (!tcpserver->listen(QHostAddress(bindIp), bindPort)) {
        QMessageBox::critical(this, "监听失败", tcpserver->errorString());
        delete tcpserver;
        tcpserver = nullptr;
        return;
    }

    // 切换到服务器模式
    currentMode = MODE_SERVER;
    QString msg = QString("服务器已启动，监听 %1:%2").arg(bindIp).arg(bindPort);
    qDebug() << msg;
    ui->recive_list->addItem("[系统] " + msg);

    // 关联新连接信号
    connect(tcpserver, &QTcpServer::newConnection, this, &System_Main_Page::newClientLink);
}


// 客户端绑定按钮（可选：保存本地IP和端口，用于绑定本地地址）
void System_Main_Page::on_client_Bind_Button_clicked() {
    // 清理旧连接
    clearAllConnections();

    // 获取客户端本地绑定的IP和端口（可选，不绑定则由系统自动分配）
    QString localIp = ui->client_Bind_ip->text().trimmed();
    quint16 localPort = ui->client_Bind_port->text().toUInt();

    // 创建客户端套接字
    tcpClient = new QTcpSocket(this);

    // 绑定本地地址（可选，不绑定可省略）
    if (!localIp.isEmpty() && localPort != 0) {
        if (!tcpClient->bind(QHostAddress(localIp), localPort)) {
            QMessageBox::warning(this, "绑定失败", tcpClient->errorString());
            delete tcpClient;
            tcpClient = nullptr;
            return;
        }
        QString msg = QString("客户端已绑定本地地址：%1:%2").arg(localIp).arg(localPort);
        qDebug() << msg;
        ui->recive_list->addItem("[系统] " + msg);
    }

    // 关联客户端信号（提前绑定接收和错误信号）
    connect(tcpClient, &QTcpSocket::readyRead, this, &System_Main_Page::recvServerMsg);
    connect(tcpClient, &QTcpSocket::disconnected, this, [=]() {
        ui->recive_list->addItem("[客户端] 与服务器断开连接");
        clearAllConnections();
    });
    connect(tcpClient, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this, [=]() {
               ui->recive_list->addItem("[客户端] 错误：" + tcpClient->errorString());
            });
}

// 客户端接收服务器数据
void System_Main_Page::recvServerMsg() {
    if (!tcpClient) return;

    QByteArray data = tcpClient->readAll();
    QString msg = "[客户端接收] 服务器：" + QString(data);
    qDebug() << msg;
    ui->recive_list->addItem(msg);
}

// 客户端连接到目标服务器
void System_Main_Page::on_Connect_Button_clicked() {
    if (!tcpClient) {
        // 若未绑定本地地址，直接创建客户端套接字
        tcpClient = new QTcpSocket(this);
        // 关联信号
        connect(tcpClient, &QTcpSocket::readyRead, this, &System_Main_Page::recvServerMsg);
        connect(tcpClient, &QTcpSocket::disconnected, this, [=]() {
            ui->recive_list->addItem("[客户端] 与服务器断开连接");
            clearAllConnections();
        });

        connect(tcpClient, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
        this, [=]() {
                    ui->recive_list->addItem("[客户端] 错误：" + tcpClient->errorString());
                });
    }

    // 获取目标服务器地址
    QString serverIp = ui->Connect_ip->text().trimmed();
    quint16 serverPort = ui->Connect_port->text().toUInt();
    if (serverIp.isEmpty() || serverPort == 0) {
        QMessageBox::warning(this, "错误", "请输入服务器IP和端口");
        return;
    }

    // 连接服务器
    tcpClient->connectToHost(QHostAddress(serverIp), serverPort);
    currentMode = MODE_CLIENT; // 切换到客户端模式
    ui->recive_list->addItem("[客户端] 正在连接 " + serverIp + ":" + QString::number(serverPort));

    // 连接成功信号（延迟绑定，避免重复连接）
    connect(tcpClient, &QTcpSocket::connected, this, [=]() {
        ui->recive_list->addItem("[客户端] 已连接到服务器");
    });
}

// 清理所有服务器/客户端连接，重置模式
void System_Main_Page::clearAllConnections() {
    // 1. 清理服务器资源
    if (tcpserver) {
        tcpserver->close();               // 停止监听新连接
        delete tcpserver;
        tcpserver = nullptr;
    }
    // 断开所有服务器客户端
    foreach (QTcpSocket *sock, serverClients) {
        if (sock && sock->state() == QAbstractSocket::ConnectedState) {
            sock->disconnectFromHost();
            sock->waitForDisconnected(1000);
            delete sock;
        }
    }
    serverClients.clear();
    curServerClient = nullptr;

    // 2. 清理客户端资源
    if (tcpClient) {
        if (tcpClient->state() == QAbstractSocket::ConnectedState) {
            tcpClient->disconnectFromHost();
            tcpClient->waitForDisconnected(1000);
        }
        delete tcpClient;
        tcpClient = nullptr;
    }

    // 3. 重置模式
    currentMode = MODE_NONE;
    qDebug() << "已清理所有连接，当前模式：未激活";
}

// 发送按钮：根据当前模式发送数据
void System_Main_Page::on_Sendc_Button_clicked() {
    QString msg = ui->send_text->toPlainText().trimmed();
    if (msg.isEmpty()) {
        QMessageBox::warning(this, "提示", "发送内容不能为空");
        return;
    }

    // 根据模式发送数据
    if (currentMode == MODE_SERVER) {
        // 服务器模式：发送给当前客户端
        if (!curServerClient || curServerClient->state() != QAbstractSocket::ConnectedState) {
            QMessageBox::warning(this, "错误", "无可用客户端连接");
            return;
        }
        curServerClient->write(msg.toUtf8());
        ui->recive_list->addItem("[服务器发送] 到 " +
            curServerClient->peerAddress().toString() + "：" + msg);
    }
    else if (currentMode == MODE_CLIENT) {
        // 客户端模式：发送给服务器
        if (!tcpClient || tcpClient->state() != QAbstractSocket::ConnectedState) {
            QMessageBox::warning(this, "错误", "未连接到服务器");
            return;
        }
        tcpClient->write(msg.toUtf8());
        ui->recive_list->addItem("[客户端发送] 到服务器：" + msg);
    }
    else {
        QMessageBox::warning(this, "错误", "请先启动服务器或连接客户端");
    }

    // 清空输入框
    ui->send_text->clear();
}
