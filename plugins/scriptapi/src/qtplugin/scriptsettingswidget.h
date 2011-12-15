/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef SCRIPTSETTINGSWIDGET_H
#define SCRIPTSETTINGSWIDGET_H

#include <qutim/settingswidget.h>
#include <QScriptValue>
#include <QScriptable>

namespace qutim_sdk_0_3
{
class ScriptSettingsWidget : public SettingsWidget
{
	Q_PROPERTY(QScriptValue load READ loadFunction WRITE setLoadFunction)
	Q_PROPERTY(QScriptValue save READ saveFunction WRITE setSaveFunction)
public:
    ScriptSettingsWidget();

	QScriptValue loadFunction() const;
	void setLoadFunction(const QScriptValue &func);
	QScriptValue saveFunction() const;
	void setSaveFunction(const QScriptValue &func);
	void setThis(const QScriptValue &that) { m_this = that; }

	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

public slots:
	void listenChildrenStates();

private:
	QScriptValue m_this;
	QScriptValue m_load;
	QScriptValue m_save;
};
}

#endif // SCRIPTSETTINGSWIDGET_H

