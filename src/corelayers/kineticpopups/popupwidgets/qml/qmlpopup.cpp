#include "qmlpopup.h"
#include <QGridLayout>
#include <QUrl>
#include <QtDeclarative/QDeclarativeContext>
#include <QVariant>
#include <QLatin1Literal>
#include <QMouseEvent>
#include "manager.h"
#include <QtDeclarative/QDeclarativeView>
#include <QVBoxLayout>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/messagesession.h>
#include <qutim/qtwin.h>
#include <qutim/emoticons.h>
#include <qutim/thememanager.h>

namespace Core
{
namespace KineticPopups
{

PopupWidget::PopupWidget()
{
	//TODO need cache
	QString theme_name = Manager::self()->themeName;
	m_view = new QDeclarativeView(this);
	QVBoxLayout *l = new QVBoxLayout(this);
	setLayout(l);
	l->setMargin(0);
	l->addWidget(m_view);

	QString themePath = ThemeManager::path("qmlpopups",theme_name);
	ConfigGroup appearance = Config(themePath + "/settings.json").group("appearance");
	setWindowFlags(static_cast<Qt::WindowFlags>(appearance.value<int>("widgetFlags",Qt::ToolTip)));
	PopupWidgetFlags popupFlags = static_cast<PopupWidgetFlags>(appearance.value<int>("popupFlags",Transparent));

	connect(m_view,SIGNAL(sceneResized(QSize)),this,SLOT(onSceneResized(QSize)));
	setAttribute(Qt::WA_DeleteOnClose);

	if (popupFlags & Transparent) {
		setAttribute(Qt::WA_NoSystemBackground);
		setAttribute(Qt::WA_TranslucentBackground);
		m_view->viewport()->setAutoFillBackground(false);
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
	m_view->setResizeMode(QDeclarativeView::SizeViewToRootObject);
	m_view->rootContext()->setContextProperty("popupWidget",this);
	m_view->rootContext()->setContextProperty("timeout",Manager::self()->timeout);
}

void PopupWidget::setTheme(const QString &themeName)
{
	QString themePath = ThemeManager::path("qmlpopups",themeName);

	QString filename =themePath % QLatin1Literal("/main.qml");
	m_view->setSource(QUrl::fromLocalFile(filename));//url - main.qml
	m_view->rootContext()->setContextProperty("popupWidget",this);
}

void PopupWidget::onSceneResized(QSize size)
{
	debug() << "resized:" << size;
	m_size_hint = size;
	emit sizeChanged(m_size_hint);
}


QSize PopupWidget::sizeHint() const
{
	return m_view->sizeHint();
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

	QDeclarativeContext *context = m_view->rootContext();

	context->setContextProperty("popupTitle",title);
	context->setContextProperty("popupBody",unescape(body));
	QString image_path = sender ? sender->property("avatar").toString() : QString();
	context->setContextProperty("popupImage",image_path);

	if (m_view->status() == QDeclarativeView::Error)
		emit activated();
}

void PopupWidget::accept()
{
	if (ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender)) {
		ChatUnit *metaContact = unit->metaContact();
		if (metaContact)
			unit = metaContact;
		ChatLayer::get(unit,true)->activate();
	}
	else if (QWidget *widget = qobject_cast<QWidget *>(m_sender)) {
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

QString PopupWidget::parseEmoticons(const QString &text)
{
	return Emoticons::theme().parseEmoticons(text);
}


}
}
