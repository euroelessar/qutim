/****************************************************************************
 *  backend.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef BACKEND_H
#define BACKEND_H

#include <qutim/notificationslayer.h>
#include <QWidget>

#if defined(KINETICPOPUPS_LIBRARY)
#  define KINETICPOPUPS_EXPORT Q_DECL_EXPORT
#else
#  define KINETICPOPUPS_EXPORT Q_DECL_IMPORT
#endif

namespace Core
{
namespace KineticPopups
{
using namespace qutim_sdk_0_3;

class Popup;
class Backend : public PopupBackend
{
	Q_OBJECT
	Q_CLASSINFO("Uses", "PopupWidget")
public:
	virtual void show(Notifications::Type type, QObject* sender,
					  const QString& body, const QVariant& data);
	Backend ();
};

class KINETICPOPUPS_EXPORT AbstractPopupWidget : public QWidget
{
	Q_OBJECT	
public:
	virtual	void setTheme(const QString &themeName) = 0;
	virtual void setData(const QString& title,
				 const QString& body,
				 QObject *sender,
				 const QVariant &data) = 0;
signals:
	void sizeChanged(const QSize &size);
	void activated();
};

class KINETICPOPUPS_EXPORT PopupGenerator : public QObject
{
	Q_CLASSINFO("Service","PopupWidget")
	Q_OBJECT
public:
	virtual AbstractPopupWidget *generate() = 0;
	virtual QString category() const = 0;
	virtual ~PopupGenerator() {}
};

}
}
#endif //BACKEND_H
