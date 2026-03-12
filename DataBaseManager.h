#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>  // 提供数据库连接功能
#include <QSqlQuery>    // 执行SQL查询
#include <QMessageBox>  // 显示消息框
#include <QDebug>       // 调试输出
#include <QString>      // 字符串处理
#include <QVector>      // 动态数组
#include <QMap>         // 键值对映射

/**
 * 数据库管理类 - 采用单例模式设计
 * 负责管理SQLite数据库的连接和用户数据操作
 */
class DatabaseManager
{
public:
    /**
     * 获取数据库管理器的单例实例
     * @return 返回唯一的DatabaseManager实例引用
     */
    static DatabaseManager& instance();

    // ===== 数据库基础操作 =====

    /**
     * 打开SQLite数据库文件
     * @param dbPath 数据库文件路径
     * @return 成功返回true，失败返回false
     */
    bool open(const QString& dbPath);

    /**
     * 创建用户数据表
     * 表结构: username(主键), password
     * @return 成功返回true，失败返回false
     */
    bool createUserTable();

    /**
     * 获取当前数据库连接
     * @return 返回QSqlDatabase对象引用
     */
    QSqlDatabase getDatabase() const;

    // ===== 用户数据操作 =====

    /**
     * 插入新用户
     * @param username 用户名
     * @param password 密码
     * @return 成功返回true，失败返回false
     */
    bool insertUser(const QString& username, const QString& password);

    /**
     * 删除用户
     * @param username 要删除的用户名
     * @return 成功返回true，失败返回false
     */
    bool deleteUser(const QString& username);

    /**
     * 更新用户信息
     * @param oldUsername 旧用户名
     * @param newUsername 新用户名
     * @param newPassword 新密码
     * @return 成功返回true，失败返回false
     */
    bool updateUser(const QString& oldUsername, const QString& newUsername, const QString& newPassword);

    /**
     * 检查用户是否存在
     * @param username 要检查的用户名
     * @return 存在返回true，不存在返回false
     */
    bool checkUserExists(const QString& username);

    // 头文件 DatabaseManager.h 中声明方法
    bool updateUserMarked(const QString& username, int marked);

    /**
     * 获取所有用户信息
     * @return 返回二维表数据，每行包含username和password字段
     */
    QVector<QMap<QString, QVariant>> getAllUsers();



    void printAllTableInfo();
private:
    /**
     * 私有构造函数 - 防止外部实例化
     */
    DatabaseManager();

    static DatabaseManager* m_instance;  // 单例实例指针
    QSqlDatabase m_database;             // 数据库连接对象
};

#endif // DATABASEMANAGER_H
