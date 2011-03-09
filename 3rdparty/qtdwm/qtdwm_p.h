/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef QTDWM_P_H
#define QTDWM_P_H

#include <QWidget>

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
	 void addWidget(QWidget *widget);
	 bool winEvent(MSG *message, long *result);

public slots:
#ifdef Q_WS_WIN
	 void removeWidget(QObject *widget);
#endif

private:
	 QWidgetList widgets;
};

#endif // QTDWM_P_H
