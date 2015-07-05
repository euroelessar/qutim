/****************************************************************************
**
** Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef QTDWM_P_H
#define QTDWM_P_H

#include <QWidget>
#include <QMap>

class BlurManager;
typedef QMap<QWidget *, BlurManager*> WidgetsMap;

/*
 * Internal helper class that notifies windows if the
 * DWM compositing state changes and updates the widget
 * flags correspondingly.
 */
class WindowNotifier : public QWidget
{
	Q_OBJECT

public:
	WindowNotifier();
	void addWidget(QWidget *widget, BlurManager *bm);
#ifdef Q_OS_WIN
	bool winEvent(MSG *message, long *result);

public slots:
	void removeWidget(QObject *widget);
#endif

private:
	WidgetsMap widgets;
};

#endif // QTDWM_P_H
