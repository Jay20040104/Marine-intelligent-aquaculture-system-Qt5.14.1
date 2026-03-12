#ifndef PASSWORDTOGGLEHELPER_H
#define PASSWORDTOGGLEHELPER_H

#include <QLineEdit>
#include <QIcon>

class PasswordToggleHelper
{
public:
    // 静态函数，无需实例化即可调用
    static void setup(QLineEdit *edit,
                      const QString &hideIconPath,
                      const QString &showIconPath);
};

#endif // PASSWORDTOGGLEHELPER_H
