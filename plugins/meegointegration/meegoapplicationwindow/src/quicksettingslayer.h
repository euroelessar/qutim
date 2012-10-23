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
#include <qutim/datasettingsobject.h>

namespace MeegoIntegration
{
class QuickSettingsModel;

struct DataSettingsObjectCreator
{
	DataSettingsObjectCreator(Ureen::SettingsItem *item);
	Ureen::DataSettingsItem *item;
	Ureen::DataSettingsObject *create();
};

class QuickGenerator : public Ureen::ObjectGenerator
{
public:
	QuickGenerator(const QString &component, DataSettingsObjectCreator *creator = 0);
	~QuickGenerator();

protected:
	virtual QObject *generateHelper() const;
	virtual const QMetaObject *metaObject() const;

private:
	const QString m_component;
	DataSettingsObjectCreator *m_creator;
};

class QuickSettingsItem : public Ureen::SettingsItem
{
public:
	QuickSettingsItem(const QString &component, Ureen::Settings::Type type,
					  const QIcon &icon, const Ureen::LocalizedString &text);
	QuickSettingsItem(const QString &component, Ureen::Settings::Type type,
					  const Ureen::LocalizedString &text);
	virtual ~QuickSettingsItem();

protected:
	virtual const Ureen::ObjectGenerator *generator() const;

private:
	const QString m_component;
};

class QuickSettingsLayer : public Ureen::SettingsLayer
{
    Q_OBJECT
	Q_PROPERTY(QObject* model READ model CONSTANT)
public:
    explicit QuickSettingsLayer();
	
	virtual void show(const Ureen::SettingsItemList &settings, QObject *controller = 0);
	virtual void close(QObject* controller = 0);
	virtual void update(const Ureen::SettingsItemList &settings, QObject *controller = 0);
	
	QObject *model();
	Q_INVOKABLE void show(QObject *object = 0);

signals:
	void settingsRequested(QObject *model);

private:
	QMap<QObject*, Ureen::SettingsItemList> m_items;
	QMap<QObject*, QuickSettingsModel*> m_models;
};
}

#endif // QUICKSETTINGSLAYER_H

