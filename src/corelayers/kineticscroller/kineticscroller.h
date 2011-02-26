/****************************************************************************
 *  kineticscroller.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef KINETICSCROLLER_H
#define KINETICSCROLLER_H
#include <QObject>

namespace Core {

class KineticScroller : public QObject
{
	Q_CLASSINFO("Service", "Scroller")
	Q_OBJECT
public:
    KineticScroller();
	virtual ~KineticScroller();
	Q_INVOKABLE void enableScrolling(QObject *widget);
public slots:
	void loadSettings();
private:
	int m_scrollingType;
};

} // namespace Core

#endif // KINETICSCROLLER_H
