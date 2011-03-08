/****************************************************************************
 *  qtdwm_p.h
 *
 *  (c) LLC INRUSCOM, Russia
 *
*****************************************************************************/

#ifndef QTDWM_P_H
#define QTDWM_P_H
#include <QWidget>

#ifdef Q_WS_WIN

/*
 * Internal helper class that notifies windows if the
 * DWM compositing state changes and updates the widget
 * flags correspondingly.
 */
class WindowNotifier : public QWidget
{
	Q_OBJECT
public:
	WindowNotifier() { winId(); }
	void addWidget(QWidget *widget) { widgets.append(widget); }
	void removeWidget(QWidget *widget) { widgets.removeAll(widget); }
	bool winEvent(MSG *message, long *result);
private:
	QWidgetList widgets;
};

#endif

#endif // QTDWM_P_H
