#ifndef CIRCLEAVATAR_H
#define CIRCLEAVATAR_H

#include <QWidget>
#include <QPixmap>
#include <QColor>

class CircleAvatar : public QWidget
{
    Q_OBJECT
    // （可选）暴露属性给 Qt Designer，如 size、textColor
    Q_PROPERTY(int size READ size WRITE setSize)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)

public:
    explicit CircleAvatar(QWidget *parent = nullptr);

    // （可选）添加内容设置接口，如图片、字母
    void setImage(const QPixmap &pixmap);
    void setLetter(const QChar &letter);

    // 属性读写
    int size() const { return m_size; }
    void setSize(int size);
    QColor textColor() const { return m_textColor; }
    void setTextColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_size = 80;               // 圆形直径
    QPixmap m_pixmap;              // 要显示的图片
    QChar m_letter;                // 要显示的字母（二选一）
    QColor m_textColor = Qt::white;// 文字颜色
    bool m_showImage = false;      // 标记当前显示图片还是字母
};

#endif // CIRCLEAVATAR_H
