/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef MAEMO5DOCKLET_H
#define MAEMO5DOCKLET_H

#include <QtCore/QCoreApplication>
#include <QIcon>


class Maemo5Docklet : public QObject
{
	Q_OBJECT
public:
	virtual ~Maemo5Docklet();
	static Maemo5Docklet* Instance();
	static void ButtonClickedCallback();
	static void Enable();
	static void Disable();
	static void SetIcon(const QString&, bool);
	static void SetTooltip(const QString&);
	static void SetMute(bool);
	static int GetIconSize();
signals:
	void ButtonClicked();
protected:
	Maemo5Docklet();
private:
	void clickedSignal();
	static Maemo5Docklet* m_instance;

};

#endif // MAEMO5DOCKLET_H

