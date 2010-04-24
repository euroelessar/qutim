#include "popupwidget.h"
#include <QGridLayout>
#include <QUrl>
#include <QtDeclarative/QDeclarativeContext>
#include <QVariant>
#include <qutim/configbase.h>
#include <QLatin1Literal>
#include <qutim/libqutim_global.h>
#include <qutim/debug.h>
#include <qutim/messagesession.h>
#include <QMouseEvent>
#include <qutim/qtwin.h>
#include "manager.h"

namespace QmlPopups {

    PopupWidget::PopupWidget()
    {
		//TODO need cache
		QString theme_name = Manager::self()->themeName;

		QString themePath = getThemePath("qmlpopups",theme_name);
		ConfigGroup appearance = Config(themePath + "/settings.json").group("appearance");
		setWindowFlags(static_cast<Qt::WindowFlags>(appearance.value<int>("widgetFlags",Qt::ToolTip)));
		PopupWidgetFlags popupFlags = static_cast<PopupWidgetFlags>(appearance.value<int>("popupFlags",Transparent));

		connect(this,SIGNAL(sceneResized(QSize)),this,SLOT(onSceneResized(QSize)));
		setAttribute(Qt::WA_DeleteOnClose);

		if (popupFlags & Transparent) {
			setAttribute(Qt::WA_NoSystemBackground);
			setAttribute(Qt::WA_TranslucentBackground);
			viewport()->setAutoFillBackground(false);
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

		setTheme(theme_name);
		setResizeMode(QDeclarativeView::SizeRootObjectToView);
		rootContext()->setContextProperty("popupWidget",this);
		rootContext()->setContextProperty("timeout",Manager::self()->timeout);
    }
    PopupWidget::PopupWidget(const QString &themeName)
    {
		setTheme(themeName);
		setResizeMode(QDeclarativeView::SizeRootObjectToView);
    }

    void PopupWidget::setTheme(const QString &themeName)
    {
		QString themePath = getThemePath("qmlpopups",themeName);

		QString filename =themePath % QLatin1Literal("/main.qml");
		setSource(QUrl::fromLocalFile(filename));//url - main.qml
		rootContext()->setContextProperty("popupWidget",this);
    }

    void PopupWidget::onSceneResized(QSize size)
    {
		debug() << "resized:" << size;
		m_size_hint = size;
		emit sizeChanged(m_size_hint);
    }


    QSize PopupWidget::sizeHint() const
    {
		return QDeclarativeView::sizeHint();
		//return m_size_hint;
    }

    void PopupWidget::setData ( const QString& title,
								const QString& body,
								QObject *sender,
								const QVariant &data
								)
    {
		m_sender = sender;
		m_data = data;

		QDeclarativeContext *context = rootContext();
		context->setContextProperty("popupTitle",title);
		context->setContextProperty("popupBody",body);
		QString image_path = sender ? sender->property("avatar").toString() : QString();
		context->setContextProperty("popupImage",image_path);
    }

    void PopupWidget::accept()
    {
		ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
		if (unit)
			unit = const_cast<ChatUnit *>(unit->getHistoryUnit());

		if (ChatSession *sess = ChatLayer::get(unit)) {
			sess->setActive(true);
		}
		else {
			QWidget *widget = qobject_cast<QWidget *>(m_sender);
			if (widget)
				widget->raise();
		}
		emit activated();
    }

    void PopupWidget::ignore()
    {
		ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
		ChatSession *sess;
		if (unit && (sess = ChatLayer::get(unit,false))) {

			if (m_data.canConvert<Message>())
				sess->markRead(m_data.value<Message>().id());
		}
		emit activated();
    }


}
