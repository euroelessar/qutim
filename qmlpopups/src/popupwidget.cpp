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
#include <qutim/messagesession.h>
#include <QMouseEvent>
#include <qutim/qtwin.h>

namespace QmlPopups {

	PopupWidget::PopupWidget()
	{
		//TODO need cache
		ConfigGroup general = Config("appearance/qmlpopups").group("general");
		QString theme_name = general.value<QString>("themeName","default");
		
		QString themePath = getThemePath("qmlpopups",theme_name);
		ConfigGroup appearance = Config(themePath + "/settings.json").group("appearance");
		setWindowFlags(static_cast<Qt::WindowFlags>(appearance.value<int>("widgetFlags",Qt::ToolTip)));
		PopupWidgetFlags popupFlags = static_cast<PopupWidgetFlags>(appearance.value<int>("popupFlags",Transparent));

		view = new QmlView(this);
		connect(view,SIGNAL(sceneResized(QSize)),this,SLOT(onSceneResized(QSize)));
		//view->setContentResizable(true);
		setAttribute(Qt::WA_DeleteOnClose);

		if (popupFlags & Transparent) {
			setAttribute(Qt::WA_NoSystemBackground);
			setAttribute(Qt::WA_TranslucentBackground);
			view->viewport()->setAutoFillBackground(false);			
		}

		if (popupFlags & AeroBackground) {
			if (QtWin::isCompositionEnabled()) {
				QtWin::extendFrameIntoClientArea(this);
				setContentsMargins(0, 0, 0, 0);
			}
		}

		if (popupFlags & AeroBlurBehind) {
			if (QtWin::isCompositionEnabled())
				QtWin::enableBlurBehindWindow(this,true);
		}

		QGridLayout *gridLayout = new QGridLayout(this);
		gridLayout->setMargin(0);
		gridLayout->addWidget(view);

		QString filename =themePath % QLatin1Literal("/popup.qml");
		view->setUrl(QUrl::fromLocalFile(filename));//url - main.qml

		view->execute();
		view->rootContext()->setContextProperty("Popup",this);

	}

	void PopupWidget::onSceneResized(QSize size)
	{
		debug() << "resized:" << size;
		m_size_hint = size;
		emit PopupResized(m_size_hint);
	}


	QSize PopupWidget::sizeHint() const
	{
		return m_size_hint;
	}

	void PopupWidget::setData ( const QString& title, const QString& body, QObject* sender )
	{
		m_sender = sender;
		QmlContext *context = view->rootContext();
		context->setContextProperty("PopupTitle",title);
		context->setContextProperty("PopupBody",body);
		QString image_path = sender ? sender->property("avatar").toString() : QString();
		if(image_path.isEmpty())
			image_path = QLatin1String("buddyicon.png");
		context->setContextProperty("PopupImage",image_path);
	}
	
	void PopupWidget::onTimeoutReached()
	{
		emit activated();
	}

	void PopupWidget::activate(int action)
	{
		if (action == 1) {
			ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
			ChatSession *sess;
			if (unit && (sess = ChatLayer::get(unit))) {
				sess->setActive(true);
			}
			else {
				QWidget *widget = qobject_cast<QWidget *>(m_sender);
				if (widget)
					widget->raise();
			}
		}
		emit activated();
	}

}
