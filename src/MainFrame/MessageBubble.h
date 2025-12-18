#pragma once
#include <QWidget>


class MessageBubble : public QWidget
{
	Q_OBJECT
public:
	enum Side { Left, Right };
	explicit MessageBubble(const QString& text, Side side, QWidget* parent = nullptr);


protected:
	void paintEvent(QPaintEvent*) override;
	QSize sizeHint() const override;


private:
	QString m_text;
	Side m_side;
};