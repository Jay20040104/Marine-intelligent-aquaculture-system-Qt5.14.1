#include "CircleAvatar.h"
#include <QPainter>
#include <QStyleOption>
#include <QStyle>
#include <QStyleOption>

CircleAvatar::CircleAvatar(QWidget *parent)
    : QWidget(parent)
{
    // 关键：启用 QSS 背景渲染（必须加！）
    setAttribute(Qt::WA_StyledBackground, true);
    // 让控件支持拉伸，适配布局
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void CircleAvatar::setImage(const QPixmap &pixmap)
{
    m_pixmap = pixmap.scaled(size(), size(),
                             Qt::KeepAspectRatioByExpanding,
                             Qt::SmoothTransformation);
    m_showImage = true;
    update(); // 触发重绘
}

void CircleAvatar::setLetter(const QChar &letter)
{
    m_letter = letter;
    m_showImage = false;
    update(); // 触发重绘
}

void CircleAvatar::setSize(int size)
{
    m_size = size;
    // 如果有图片，重新缩放
    if (!m_pixmap.isNull()) {
        m_pixmap = m_pixmap.scaled(size, size,
                                   Qt::KeepAspectRatioByExpanding,
                                   Qt::SmoothTransformation);
    }
    update(); // 触发重绘
}

void CircleAvatar::setTextColor(const QColor &color)
{
    m_textColor = color;
    update(); // 触发重绘
}

void CircleAvatar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // ========= 步骤1：渲染 QSS 定义的背景、边框（必须保留！） =========
    // 替换原 QStyleOptionWidget 逻辑
    QStyleOption option;
    option.initFrom(this);
    option.rect = this->rect();
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

    // ========= 步骤2：计算圆形裁剪区域（扣除边框宽度） =========
    // 从 QSS 解析边框宽度（简单示例：假设边框是 solid，提取宽度）
    int borderWidth = 1; // 默认 1px，可动态解析 QSS
    QString styleSheet = this->styleSheet();
    if (styleSheet.contains("border:")) {
        // 正则提取 border-width（更健壮的写法需用正则，这里简化）
        borderWidth = styleSheet.split(" ")[1].toInt();
    }

    // 内容区域 = 控件区域 - 边框宽度
    QRect contentRect = this->rect().adjusted(
        borderWidth, borderWidth,
        -borderWidth, -borderWidth
        );
    int circleSize = qMin(contentRect.width(), contentRect.height());
    int x = contentRect.x() + (contentRect.width() - circleSize) / 2;
    int y = contentRect.y() + (contentRect.height() - circleSize) / 2;

    // 创建圆形裁剪路径
    QPainterPath clipPath;
    clipPath.addEllipse(x, y, circleSize, circleSize);
    painter.setClipPath(clipPath); // 后续绘制仅显示圆形区域

    // ========= 步骤3：绘制内容（图片/字母，自动圆形裁剪） =========
    if (m_showImage && !m_pixmap.isNull()) {
        // 绘制图片（自动裁剪为圆形）
        painter.drawPixmap(x, y, circleSize, circleSize, m_pixmap);
    } else {
        // 绘制字母（居中显示）
        painter.setPen(m_textColor);
        painter.drawText(contentRect, Qt::AlignCenter, QString(m_letter));
    }
}
