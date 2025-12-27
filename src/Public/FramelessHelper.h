#pragma once

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QRect>

class FramelessHelper : public QObject
{
    Q_OBJECT
public:
    explicit FramelessHelper(QWidget* target);

    void setTitleBarHeight(int h);     // 标题栏高度
    void setResizeBorderWidth(int w);  // 边框拉伸宽度

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    enum ResizeRegion
    {
        None,
        Left, Right, Top, Bottom,
        TopLeft, TopRight,
        BottomLeft, BottomRight
    };

    ResizeRegion detectRegion(const QPoint& pos) const;
    void updateCursor(ResizeRegion region);
    void resizeWindow(const QPoint& globalPos);

private:
    QWidget* m_target = nullptr;

    bool m_leftPressed = false;
    bool m_resizing = false;

    QPoint m_pressOffset;      // 鼠标到窗口左上角的偏移
    QRect  m_pressGeometry;    // 按下时窗口几何

    ResizeRegion m_region = None;

    int m_titleHeight = 32;    // 默认标题栏高度
    int m_borderWidth = 6;     // 默认拉伸区域宽度
};
