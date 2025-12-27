#include "FramelessHelper.h"

#include <QMouseEvent>
#include <QEvent>
#include <QGuiApplication>
#include <QScreen>


FramelessHelper::FramelessHelper(QWidget* target)
    : QObject(target)
    , m_target(target)
{
    Q_ASSERT(m_target);

    // 无边框 + 作为顶级窗口
    m_target->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    // 必须开启，否则 MouseMove 不连续
    m_target->setMouseTracking(true);

    // 安装事件过滤器
    m_target->installEventFilter(this);
}

void FramelessHelper::setTitleBarHeight(int h)
{
    m_titleHeight = h;
}

void FramelessHelper::setResizeBorderWidth(int w)
{
    m_borderWidth = w;
}

bool FramelessHelper::eventFilter(QObject* obj, QEvent* event)
{
    if (obj != m_target)
        return false;

    switch (event->type())
    {
    case QEvent::MouseButtonPress:
    {
        auto* e = static_cast<QMouseEvent*>(event);
        if (e->button() != Qt::LeftButton)
            break;

        m_leftPressed = true;

        // 记录偏移（用于拖动）
        m_pressOffset =
            e->globalPosition().toPoint() -
            m_target->frameGeometry().topLeft();

        // 记录几何（用于 resize）
        m_pressGeometry = m_target->geometry();

        // 判断是否在 resize 区域
        m_region = detectRegion(e->position().toPoint());
        m_resizing = (m_region != None);

        return true;
    }

    case QEvent::MouseMove:
    {
        auto* e = static_cast<QMouseEvent*>(event);
        QPoint localPos = e->position().toPoint();

        // 未按下：只更新光标
        if (!m_leftPressed)
        {
            updateCursor(detectRegion(localPos));
            return false;
        }

        // resize 优先
        if (m_resizing)
        {
            resizeWindow(e->globalPosition().toPoint());
            return true;
        }

        // 标题栏拖动
        if (localPos.y() <= m_titleHeight)
        {
            m_target->move(
                e->globalPosition().toPoint() - m_pressOffset
            );
            return true;
        }

        break;
    }

    case QEvent::MouseButtonRelease:
    {
        m_leftPressed = false;
        m_resizing = false;
        m_region = None;
        m_target->unsetCursor();
        break;
    }

    default:
        break;
    }

    return false;
}

FramelessHelper::ResizeRegion FramelessHelper::detectRegion(const QPoint& pos) const
{
    QRect r = m_target->rect();

    bool left = pos.x() <= m_borderWidth;
    bool right = pos.x() >= r.width() - m_borderWidth;
    bool top = pos.y() <= m_borderWidth;
    bool bottom = pos.y() >= r.height() - m_borderWidth;

    // ⚠️ 四角必须优先判断
    if (top && left)     return TopLeft;
    if (top && right)    return TopRight;
    if (bottom && left)  return BottomLeft;
    if (bottom && right) return BottomRight;
    if (left)   return Left;
    if (right)  return Right;
    if (top)    return Top;
    if (bottom) return Bottom;

    return None;
}

void FramelessHelper::updateCursor(ResizeRegion region)
{
    Qt::CursorShape cursor = Qt::ArrowCursor;

    switch (region)
    {
    case Left:
    case Right:
        cursor = Qt::SizeHorCursor;
        break;

    case Top:
    case Bottom:
        cursor = Qt::SizeVerCursor;
        break;

    case TopLeft:
    case BottomRight:
        cursor = Qt::SizeFDiagCursor;
        break;

    case TopRight:
    case BottomLeft:
        cursor = Qt::SizeBDiagCursor;
        break;

    default:
        cursor = Qt::ArrowCursor;
        break;
    }

    m_target->setCursor(cursor);
}

void FramelessHelper::resizeWindow(const QPoint& globalPos)
{
    QRect r = m_pressGeometry;
    QPoint delta = globalPos -
        (m_pressGeometry.topLeft() + m_pressOffset);

    switch (m_region)
    {
    case Left:
        r.setLeft(r.left() + delta.x());
        break;
    case Right:
        r.setRight(r.right() + delta.x());
        break;
    case Top:
        r.setTop(r.top() + delta.y());
        break;
    case Bottom:
        r.setBottom(r.bottom() + delta.y());
        break;
    case TopLeft:
        r.setTopLeft(r.topLeft() + delta);
        break;
    case TopRight:
        r.setTopRight(r.topRight() + delta);
        break;
    case BottomLeft:
        r.setBottomLeft(r.bottomLeft() + delta);
        break;
    case BottomRight:
        r.setBottomRight(r.bottomRight() + delta);
        break;
    default:
        break;
    }

    m_target->setGeometry(r);
}
