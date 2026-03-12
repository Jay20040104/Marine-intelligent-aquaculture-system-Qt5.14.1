#include "passwordtogglehelper.h"
#include <QAction>

/*
 * 给 QLineEdit 密码框添加“显示/隐藏密码”功能
 * 参数说明：
 * - edit：目标 QLineEdit（密码框）
 * - hideIconPath：隐藏密码时的图标路径（初始状态）
 * - showIconPath：显示密码时切换的图标路径
 */

void PasswordToggleHelper::setup(QLineEdit *edit,
                                     const QString &hideIconPath,
                                     const QString &showIconPath)
{
    // 1. 创建“眼睛”动作（初始为“隐藏密码”状态）
    QAction *eyeAction = new QAction(QIcon(hideIconPath), "切换密码可见性", edit);
    eyeAction->setToolTip("显示/隐藏密码"); // 可选：鼠标悬浮提示

    // 2. 将动作绑定到输入框右侧
    edit->addAction(eyeAction, QLineEdit::TrailingPosition);

    // 3. 状态标记：0=隐藏密码，1=显示密码
    int stat = 0;

    // 4. 连接动作触发事件，切换显示/隐藏
    QObject::connect(eyeAction, &QAction::triggered, edit, [=]() mutable {
        if (stat == 0) { // 隐藏 → 显示
            eyeAction->setIcon(QIcon(showIconPath));
            edit->setEchoMode(QLineEdit::Normal);
            stat = 1;
        } else { // 显示 → 隐藏
            eyeAction->setIcon(QIcon(hideIconPath));
            edit->setEchoMode(QLineEdit::Password);
            stat = 0;
        }
    });
}
