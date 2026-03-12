#include "DatabaseManager.h"
#include <QSqlError>
#include <QSqlRecord>

// 初始化单例指针 - 单例模式确保一个类只有一个实例，并提供一个全局访问点
// m_instance是静态成员变量，用于存储类的唯一实例
DatabaseManager* DatabaseManager::m_instance = nullptr;

// 私有构造函数 - 防止外部直接创建对象，只能通过instance()方法获取实例
// 初始化QSQLITE数据库连接
DatabaseManager::DatabaseManager()
    : m_database(QSqlDatabase::addDatabase("QSQLITE"))
{
}

// 获取单例实例 - 线程不安全的懒汉式实现
// 第一次调用时创建实例，之后直接返回已创建的实例
DatabaseManager& DatabaseManager::instance()
{
    if (!m_instance) {
        m_instance = new DatabaseManager();
    }
    return *m_instance;
}

// 打开数据库 - 根据指定路径打开SQLite数据库文件
// 返回true表示成功，false表示失败
bool DatabaseManager::open(const QString& dbPath)
{
    m_database.setDatabaseName(dbPath);
    bool success = m_database.open();

    if (!success) {
        // 弹出错误消息框并输出错误信息到调试窗口
        QMessageBox::critical(nullptr, "数据库错误",
                              "无法打开数据库: " + m_database.lastError().text());
        qDebug() << "数据库打开错误:" << m_database.lastError().text();
    } else {
        printAllTableInfo(); // 打印所有表信息
        qDebug() << "进入mydatabase数据库成功";
    }
    return success;
}

// 打印所有表信息（调试用）
void DatabaseManager::printAllTableInfo()
{
    if (!m_database.isOpen()) {
        qDebug() << "数据库未打开，无法获取表信息";
        return;
    }

    QStringList tables = m_database.tables();
    qDebug() << "\n===== 数据库表结构信息 =====";
    qDebug() << "当前数据库中的表:" << tables;

    foreach (const QString &tableName, tables) {
        qDebug() << "\n--- 表名:" << tableName << "---";

        // 获取表结构
        QSqlQuery query(m_database);
        if (query.exec("PRAGMA table_info(" + tableName + ")")) {
            while (query.next()) {
                QString cid = query.value("cid").toString();
                QString name = query.value("name").toString();
                QString type = query.value("type").toString();
                QString notnull = query.value("notnull").toString();
                QString dflt_value = query.value("dflt_value").toString();
                QString pk = query.value("pk").toString();

                qDebug() << "字段" << cid << ": 名称=" << name
                         << ", 类型=" << type
                         << ", 非空=" << (notnull == "1" ? "是" : "否")
                         << ", 默认值=" << dflt_value
                         << ", 主键=" << (pk == "1" ? "是" : "否");
            }
        } else {
            qDebug() << "获取表结构失败:" << query.lastError().text();
        }

        // 获取表数据行数
        if (query.exec("SELECT COUNT(*) FROM " + tableName)) {
            if (query.next()) {
                int rowCount = query.value(0).toInt();
                qDebug() << "表" << tableName << "的行数:" << rowCount;

                // 如果表中有数据，打印前几行
                if (rowCount > 0) {
                    qDebug() << "表" << tableName << "的前几行数据:";
                    QSqlQuery dataQuery(m_database);
                    dataQuery.exec("SELECT * FROM " + tableName + " LIMIT 5");

                    // 获取字段名
                    QSqlRecord record = dataQuery.record();
                    QStringList fieldNames;
                    for (int i = 0; i < record.count(); i++) {
                        fieldNames.append(record.fieldName(i));
                    }
                    qDebug() << "字段:" << fieldNames.join(", ");

                    // 打印数据
                    int rowNum = 0;
                    while (dataQuery.next() && rowNum < 5) {
                        QStringList rowData;
                        for (int i = 0; i < record.count(); i++) {
                            rowData.append(dataQuery.value(i).toString());
                        }
                        qDebug() << "行" << rowNum + 1 << ":" << rowData.join(", ");
                        rowNum++;
                    }
                }
            }
        } else {
            qDebug() << "获取表行数失败:" << query.lastError().text();
        }
    }
    qDebug() << "==========================\n";
}


// 创建用户表 - 如果表不存在，则创建一个名为usertable的表
// marked: 标记字段，整数类型
// username: 用户名，文本类型，主键(唯一且不为空)
// password: 密码，文本类型
bool DatabaseManager::createUserTable()
{
    QSqlQuery query(m_database);
    const QString sql = R"(
        CREATE TABLE IF NOT EXISTS usertable (
            marked INTEGER DEFAULT 0,
            username TEXT PRIMARY KEY,
            password TEXT
        )
    )";

    if (!query.exec(sql)) {
        QMessageBox::critical(nullptr, "数据库错误",
                              "无法创建用户表: " + query.lastError().text());
        qDebug() << "创建用户表错误:" << query.lastError().text();
        return false;
    } else {
        qDebug() << "表usertable创建成功 或 表usertable已经存在";
    }
    return true;
}

// 插入用户 - 向用户表中添加新用户
// 插入前会检查用户名是否已存在
bool DatabaseManager::insertUser(const QString& username, const QString& password)
{
    if (checkUserExists(username)) {
        qDebug() << "用户已经存在:" << username;
        return false;
    }

    // 使用预处理语句防止SQL注入
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO usertable (marked, username, password) VALUES (0, :username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "用户注册错误:" << query.lastError().text();
        return false;
    }
    return true;
}

// 删除用户 - 根据用户名从表中删除对应的记录
// 返回是否成功删除(影响的行数大于0)
bool DatabaseManager::deleteUser(const QString& username)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM usertable WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "删除用户错误:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// 修改用户 - 更新用户信息，包括用户名和密码
// 注意: 修改用户名时需要确保新用户名不存在
bool DatabaseManager::updateUser(const QString& oldUsername, const QString& newUsername, const QString& newPassword)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE usertable SET username = :newUsername, password = :newPassword WHERE username = :oldUsername");
    query.bindValue(":newUsername", newUsername);
    query.bindValue(":newPassword", newPassword);
    query.bindValue(":oldUsername", oldUsername);

    if (!query.exec()) {
        qDebug() << "更新用户错误:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// 检查用户是否存在 - 查询表中是否存在指定用户名的记录
// 使用SELECT 1而非SELECT *可以减少数据传输，提高效率
bool DatabaseManager::checkUserExists(const QString& username)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT 1 FROM usertable WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "检查用户存在错误:" << query.lastError().text();
        return false;
    }

    // query.next()返回true表示有查询结果，即用户存在
    return query.next();
}

// 获取所有用户 - 查询表中所有记录并返回
// 返回值是一个QVector，每个元素是一个QMap，包含用户的各个字段
QVector<QMap<QString, QVariant>> DatabaseManager::getAllUsers()
{
    QVector<QMap<QString, QVariant>> users;

    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM usertable")) {
        qDebug() << "获取所有用户错误:" << query.lastError().text();
        return users;
    }

    // 遍历查询结果，将每条记录转换为QMap并添加到QVector中
    while (query.next()) {
        QMap<QString, QVariant> user;
        user["marked"] = query.value("marked");
        user["username"] = query.value("username");
        user["password"] = query.value("password");
        users.append(user);
    }

    return users;
}

bool DatabaseManager::updateUserMarked(const QString& username, int marked) {
    // 1. 清空所有用户标记（独立查询对象）
    QSqlQuery clearQuery(m_database);
    if (!clearQuery.exec("UPDATE usertable SET marked = 0")) {
        qDebug() << "清空标记失败：" << clearQuery.lastError().text();
        return false;
    }

    // 2. 更新目标用户标记（独立查询对象）
    QSqlQuery updateQuery(m_database);
    updateQuery.prepare("UPDATE usertable SET marked = :marked WHERE username = :username");
    updateQuery.bindValue(":marked", marked);
    updateQuery.bindValue(":username", username);

    // 【关键调试】打印预处理语句和绑定参数
    qDebug() << "预处理SQL：" << updateQuery.lastQuery();
    qDebug() << "绑定参数：" << updateQuery.boundValues();

    if (!updateQuery.exec()) {
        qDebug() << "更新标记失败：" << updateQuery.lastError().text();
        return false;
    }
    return true;
}

// 获取数据库对象 - 提供对底层QSqlDatabase对象的访问
// 可以用于执行自定义SQL查询
QSqlDatabase DatabaseManager::getDatabase() const
{
    return m_database;
}
