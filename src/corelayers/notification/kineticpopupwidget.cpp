#include "kineticpopupwidget.h"
#include <QVBoxLayout>
#include "kineticpopupsmanager.h"
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QDesktopWidget>

namespace KineticPopups
{
	PopupWidget::PopupWidget (const ThemeHelper::PopupSettings &popupSettings)
	{
		popup_settings = popupSettings;
		//init browser
		this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
		this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
		this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
		this->setFrameShape ( QFrame::NoFrame );
		this->setWindowFlags(popup_settings.widgetFlags);
		//this->resize(NotificationsManager::self()->defaultSize);

		//init transparent
		this->setAttribute(Qt::WA_TranslucentBackground);
		this->setAttribute(Qt::WA_NoSystemBackground, false);
		this->ensurePolished(); // workaround Oxygen filling the background
		this->setAttribute(Qt::WA_StyledBackground, false);

		this->setStyleSheet(popup_settings.styleSheet);
	}


	QSize PopupWidget::setData ( const QString& title, const QString& body, const QString& imagePath )
	{
		QString data = popup_settings.content;
		data.replace ( "{title}", title );
		data.replace ( "{body}", body );
		data.replace ( "{imagepath}",Qt::escape ( imagePath ) );
		this->document()->setHtml(data);
		this->document()->setTextWidth(popup_settings.defaultSize.width());
		int width = popup_settings.defaultSize.width();
		int height = this->document()->size().height();

		return QSize(width,height);


	}


	void PopupWidget::setTheme (const ThemeHelper::PopupSettings &popupSettings )
	{
		popup_settings = popupSettings;
		this->setStyleSheet (popup_settings.styleSheet);
	}


	void PopupWidget::mouseReleaseEvent ( QMouseEvent* ev )
	{
		if (ev->button() == Manager::self()->action1Trigger) {
			emit action1Activated();
		}
		else if (ev->button() == Manager::self()->action2Trigger)
			emit action2Activated();
		else
			return;
		disconnect(SIGNAL(action1Activated()));
		disconnect(SIGNAL(action2Activated()));
	}
}