/****************************************************************************
 *  simpleactions.h
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

#ifndef SIMPLEACTIONS_H
#define SIMPLEACTIONS_H
#include <QObject>
#include <QScopedPointer>

class QAction;
namespace qutim_sdk_0_3
{
class ActionGenerator;
}

namespace Core {

class SimpleActions : public QObject
{
	Q_OBJECT
public:
	SimpleActions();
	virtual ~SimpleActions();
private slots:
	void onTagsEditCreated(QAction *, QObject *);
	void onTagsEditAction(QObject*);
private:
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_tagEditGen;
};

} // namespace Core

#endif // SIMPLEACTIONS_H
