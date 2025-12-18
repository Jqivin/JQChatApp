#include "JQChatMainFrame.h"
#include <QtWidgets/QApplication>
#include "JQAppManager.h"
#include <QTranslator>
#include <QString>
#include <QFile>

// 加载资源文件
void LoadResources(QApplication* app)
{
	QTranslator* tranlate = new QTranslator();
	tranlate->load(":/JQChat/translates/JQChat.qm");
	QApplication::installTranslator(tranlate);

	QString strStyle;
	QFile filecss(":/JQChat/css/JQChat.css");
	if (filecss.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString style(filecss.readAll());
		strStyle += style;

		filecss.close();

		app->setStyleSheet(strStyle);
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	LoadResources(&a);

	if (JQApp.InitInstance())
	{
		a.exec();
	}
	return 0;
}
