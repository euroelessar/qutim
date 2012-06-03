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

#ifndef QUICKSETTINGSLAYER_H
#define QUICKSETTINGSLAYER_H

#include <qutim/settingslayer.h>

namespace MeegoIntegration
{
class QuickSettingsModel;

class QuickGenerator : public qutim_sdk_0_3::ObjectGenerator
{
public:
	QuickGenerator(const QString &component);
	~QuickGenerator();

protected:
	virtual QObject *generateHelper() const;
	virtual const QMetaObject *metaObject() const;

private:
	const QString m_component;
};

class QuickSettingsItem : public qutim_sdk_0_3::SettingsItem
{
public:
	QuickSettingsItem(const QString &component, qutim_sdk_0_3::Settings::Type type,
					  const QIcon &icon, const qutim_sdk_0_3::LocalizedString &text);
	QuickSettingsItem(const QString &component, qutim_sdk_0_3::Settings::Type type,
					  const qutim_sdk_0_3::LocalizedString &text);
	virtual ~QuickSettingsItem();

protected:
	virtual const qutim_sdk_0_3::ObjectGenerator *generator() const;

private:
	const QString m_component;
};

class QuickSettingsLayer : public qutim_sdk_0_3::SettingsLayer
{
    Q_OBJECT
	Q_PROPERTY(QObject* model READ model CONSTANT)
public:
    explicit QuickSettingsLayer();
	
	virtual void show(const qutim_sdk_0_3::SettingsItemList &settings, QObject *controller = 0);
	virtual void close(QObject* controller = 0);
	virtual void update(const qutim_sdk_0_3::SettingsItemList &settings, QObject *controller = 0);
	
	QObject *model();
	Q_INVOKABLE void show(QObject *object = 0);

signals:
	void settingsRequested(QObject *model);

private:
	QMap<QObject*, qutim_sdk_0_3::SettingsItemList> m_items;
	QMap<QObject*, QuickSettingsModel*> m_models;
};
}

#endif // QUICKSETTINGSLAYER_H

