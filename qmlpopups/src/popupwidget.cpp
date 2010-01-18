#include "popupwidget.h"
#include <QtDeclarative/qmlview.h>
#include <QGridLayout>
#include <QUrl>
#include <QtDeclarative/qmlcontext.h>
#include <QVariant>
#include <qutim/configbase.h>
#include <QLatin1Literal>
#include <qutim/libqutim_global.h>
#include <qutim/debug.h>

namespace QmlPopups {

	PopupWidget::PopupWidget()
	{
		//TODO need cache
		ConfigGroup general = Config("appearance/qmlpopups").group("general");
		QString theme_name = general.value<QString>("themeName","default");
		
		view = new QmlView(this);
		//view->setContentResizable(true);
		connect(view,SIGNAL(sceneResized(QSize)),this,SLOT(onSceneResized(QSize)));

		setWindowFlags(Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setAttribute(Qt::WA_NoSystemBackground);
		view->viewport()->setAutoFillBackground(false);

		QGridLayout *gridLayout = new QGridLayout(this);
		gridLayout->setMargin(0);
		gridLayout->addWidget(view);

		QString filename = getThemePath("qmlpopups",theme_name) %  QLatin1Literal("/popup.qml");
		debug() << filename;
		view->setUrl(QUrl::fromLocalFile(filename));//url - main.qml

		view->execute();

	}

	void PopupWidget::onSceneResized(QSize size)
	{
		m_size_hint = size;
		emit PopupResized(m_size_hint);
	}


	QSize PopupWidget::sizeHint() const
	{
		return m_size_hint;
	}

	void PopupWidget::setData ( const QString& title, const QString& body, QObject* sender )
	{
		QmlContext *context = view->rootContext();
		context->setContextProperty("PopupTitle",title);
		context->setContextProperty("PopupBody",body);
		context->setContextProperty("PopupImage",sender->property("avatar"));
	}
	
	void PopupWidget::onTimeoutReached()
	{
		emit Activated();
	}

}