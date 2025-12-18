
#include "MessageBubble.h"
#include <QPainter>
#include <QFontMetrics>


MessageBubble::MessageBubble(const QString& text, Side side, QWidget* parent)
	: QWidget(parent), m_text(text), m_side(side)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize MessageBubble::sizeHint() const
{
	QFontMetrics fm(font());
	int w = qMin(400, fm.horizontalAdvance(m_text) + 30);
	int h = fm.boundingRect(QRect(0, 0, w, 1000), Qt::TextWordWrap, m_text).height() + 20;
	return QSize(w + 80, h);
}


void MessageBubble::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);


	QRect bubbleRect;
	int margin = 10;
	int maxWidth = width() - 80;


	QFontMetrics fm(font());
	QRect textRect = fm.boundingRect(QRect(0, 0, maxWidth, 1000), Qt::TextWordWrap, m_text);


	if (m_side == Left)
		bubbleRect = QRect(10, 5, textRect.width() + 20, textRect.height() + 20);
	else
		bubbleRect = QRect(width() - textRect.width() - 30, 5,
			textRect.width() + 20, textRect.height() + 20);


	QColor bg = (m_side == Left) ? QColor(240, 240, 240) : QColor(160, 230, 120);
	p.setBrush(bg);
	p.setPen(Qt::NoPen);
	p.drawRoundedRect(bubbleRect, 8, 8);


	p.setPen(Qt::black);
	p.drawText(bubbleRect.adjusted(10, 10, -10, -10), Qt::TextWordWrap, m_text);
}