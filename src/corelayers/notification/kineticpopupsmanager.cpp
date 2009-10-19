#include "kineticpopupsmanager.h"
#include "kineticpopup.h"
#include <QDebug>
#include <QFile>
#include <QTextDocument>
#include <QApplication>
#include <QMouseEvent>
#include <QDesktopWidget>
#include "libqutim/systeminfo.h"
#include "libqutim/configbase.h"
#include "kineticpopupsettings.h"

// the universe's only Notification manager
KineticPopupsManager *KineticPopupsManager::instance = 0;

KineticPopupsManager::KineticPopupsManager()
{
	loadSettings();
}

KineticPopup *KineticPopupsManager::getById ( const QString& id ) const
{
	int number = getNumber(id);
	return (number == -1) ? 0 : active_notifications.value(number);
}


int KineticPopupsManager::getNumber ( const QString& id ) const
{
	for (int i=0;i!=active_notifications.count();i++) {
		if (active_notifications.value(i)->getId() == id)
			return i;
	}
	return -1;
}


KineticPopup* KineticPopupsManager::getByNumber ( const int& number ) const
{
	return active_notifications.value(number);
}


QRect KineticPopupsManager::insert ( KineticPopup* notification )
{
	if (getNumber(notification->getId()) != -1)
	{
		qWarning() << "Trying to overwrite exist notification";
		return QRect();
	}
	active_notifications.append(notification);
	int number = active_notifications.count()-1;
	if (number == 0) {
		QRect geom = QApplication::desktop()->availableGeometry(QCursor::pos());
		geom = QRect(geom.bottomRight(),notification->geometry().size());
		geom.moveLeft(geom.x() - popupSettings.margin - notification->geometry().width());
		return geom;
	}
	else
		return active_notifications.at(number-1)->geometry();
}


void KineticPopupsManager::remove ( const QString& id )
{
	int number = getNumber(id);
	active_notifications.removeAt(number);
}


void KineticPopupsManager::updateGeometry()
{
	if (updatePosition) {
		QRect geom = QApplication::desktop()->availableGeometry(QCursor::pos());
		int y = geom.bottom();
		geom.moveLeft(geom.right() - popupSettings.margin - popupSettings.defaultSize.width());
		for (int i=0;i!=active_notifications.count();i++) {
			y -= popupSettings.margin + active_notifications.value(i)->geometry().height();
			geom.moveTop(y);
			geom.setSize(active_notifications.value(i)->geometry().size());
			active_notifications.value(i)->update(geom);
		}
	}
}




KineticPopupsManager* KineticPopupsManager::self()
{
	if ( !instance )
		instance = new KineticPopupsManager();
	return instance;
}


void KineticPopupsManager::loadSettings()
{
	ConfigGroup general = Config("appearence/kineticpopups").group("general");
	animationDuration = general.value("animationDuration",1000);
	QString theme_name = general.value<QString>("themeName","default");
	*reinterpret_cast<int *>(&showFlags) = general.value("showFlags", 0xfffffff);
	updatePosition = general.value<bool>("updatePosition",true);
	animation = general.value<bool>("animated",true);
	timeout = general.value<int>("timeout",0);
	appendMode = general.value<bool>("appendMode",true);
	easingCurve.setType(static_cast<QEasingCurve::Type>(general.value<int>("easingCurve",QEasingCurve::OutSine)));

	QString theme_path = KineticPopupThemeHelper::getThemePath(theme_name);
	popupSettings = KineticPopupThemeHelper::loadThemeSetting(theme_path);
	//TODO need global actions handler
	action1Trigger = Qt::LeftButton;
	action2Trigger = Qt::RightButton;
}
