#include "kineticpopup.h"
#include "kineticpopupwidget.h"
#include "kineticpopupsmanager.h"
#include <QPropertyAnimation>
#include <QApplication>
#include <QTimer>
#include <QFinalState>
#include <QState>
#include <QDebug>
#include <QStateMachine>

KineticPopup::KineticPopup ( const QString& id, uint timeOut )
        :	id ( id ), timeout ( timeOut )
{

}

KineticPopup::KineticPopup(QObject* parent): QObject(parent)
{

}

void KineticPopup::setId(const QString& id)
{
	this->id = id;
}

void KineticPopup::setTimeOut(uint timeOut)
{
	this->timeout = timeOut;
}

KineticPopup::~KineticPopup()
{
    KineticPopupsManager::self()->remove (id);
    KineticPopupsManager::self()->updateGeometry();
    notification_widget->deleteLater();
}

void KineticPopup::setMessage ( const QString& title, const QString& body, const QString &imagePath )
{
    this->title = title;
    this->body = body;
    this->image_path = imagePath;
}

void KineticPopup::appendMessage ( const QString& message )
{
    this->body += "<br />" + message;
    QSize newSize = notification_widget->setData(title,body,image_path);
    show_geometry.setSize(newSize);
    updateGeometry(show_geometry);
    KineticPopupsManager::self()->updateGeometry();
    show_state->assignProperty(notification_widget,"geometry",show_geometry);
    if (timeout > 0) {
		killTimer(timer_id);
        timer_id = startTimer(timeout);
	}
}

QString KineticPopup::getId() const
{
    return id;
}

void KineticPopup::send()
{
    KineticPopupsManager *manager = KineticPopupsManager::self();
	machine = new QStateMachine(this);

    notification_widget = new KineticPopupWidget (manager->popupSettings);
    notification_widget->setData(title,body,image_path);
    QSize notify_size = notification_widget->setData ( title,body,image_path );
    connect (notification_widget,SIGNAL(action1Activated()),SIGNAL(action1Activated()));
    connect (notification_widget,SIGNAL(action2Activated()),SIGNAL(action2Activated()));

    show_geometry.setSize(notify_size);
    QRect geom = manager->insert(this);
    if ( geom.isEmpty() )
        deleteLater();

    show_state = new QState();
    hide_state = new QState();
    QFinalState *final_state = new QFinalState();

    int x = manager->popupSettings.margin + geom.width();
    int y = manager->popupSettings.margin + geom.height();
    geom.moveTop(geom.y() - y);

    show_state->assignProperty(notification_widget,"geometry",geom);
    show_geometry = geom;
    geom.moveLeft(geom.left() + x);
    hide_state->assignProperty(notification_widget,"geometry",geom);
    notification_widget->setGeometry(geom);

    show_state->addTransition(notification_widget,SIGNAL(action1Activated()),hide_state);
    show_state->addTransition(notification_widget,SIGNAL(action2Activated()),hide_state);
    hide_state->addTransition(hide_state,SIGNAL(polished()),final_state);
    show_state->addTransition(this,SIGNAL(updated()),show_state); //Black magic

    if (timeout > 0) {
        timer_id = startTimer(timeout);
        show_state->addTransition(this,SIGNAL(timeoutReached()),hide_state);
    }

    machine->addState(show_state);
    machine->addState(hide_state);
    machine->addState(final_state);
    machine->setInitialState (show_state);

    QPropertyAnimation *animation = new QPropertyAnimation ( notification_widget,"geometry" );
    if (manager->animation) {
        machine->addDefaultAnimation (animation);
        animation->setDuration ( manager->animationDuration);
        animation->setEasingCurve (manager->easingCurve);
    }

    connect(machine,SIGNAL(finished()),SLOT(deleteLater()));
    machine->start();
    notification_widget->show();
}

void KineticPopup::update(QRect geom)
{
    show_state->assignProperty(notification_widget,"geometry",geom);
    updateGeometry(geom);
    geom.moveRight(geom.right() + notification_widget->width() + KineticPopupsManager::self()->popupSettings.margin);
    hide_state->assignProperty(notification_widget,"geometry",geom);
}

QRect KineticPopup::geometry() const
{
    return show_geometry;
}

void KineticPopup::updateGeometry(const QRect &newGeometry)
{
    show_geometry = newGeometry;
    emit updated();
}

void KineticPopup::timerEvent ( QTimerEvent* ev)
{
    emit timeoutReached();
    QObject::timerEvent(ev);
}
