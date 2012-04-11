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

#ifndef SETTINGSLAYER_P_H
#define SETTINGSLAYER_P_H

#include "settingslayer.h"
#include "settingswidget.h"
#include "dataforms.h"
#include <QFormLayout>
#include <QLabel>
#include <QMultiMap>
#include <QWeakPointer>
#include "debug.h"

class QLineEdit;
namespace qutim_sdk_0_3
{
struct ConnectInfo
{
	ConnectInfo(const char *s, QObject *r, const char *m) : signal(s), receiver(r), member(m) {}
	QByteArray signal;
	QWeakPointer<QObject> receiver;
	QByteArray member;
};

class SettingsItemPrivate
{
public:
	SettingsItemPrivate() : gen(0), type(Settings::Invalid),order(50),priority(50) {}
	mutable const ObjectGenerator *gen;
	Settings::Type type;
	QIcon icon;
	LocalizedString text; // should be inserted by QT_TRANSLATE_NOOP_UTF8("Settings", "Contact list")
	mutable QWeakPointer<SettingsWidget> widget;
	QList<ConnectInfo> connections;
	int order;
	int priority;
};

class AutoSettingsItemPrivate : public SettingsItemPrivate
{
public:
	QString config;
	QString group;
	QList<AutoSettingsItem::Entry *> entries;
};

class AutoSettingsItem::EntryPrivate
{
public:
	LocalizedString text;
	const ObjectGenerator *gen;
	QString name;
	QList<QPair<QByteArray, QVariant> > properties;
};

typedef QPair<QWidget *, QByteArray> AutoSettingsEntryInfo;

class AutoSettingsWidgetPrivate
{
public:
	QList<AutoSettingsEntryInfo> entries;
};

class AutoSettingsWidget : public SettingsWidget
{
	Q_OBJECT
public:
	AutoSettingsWidget(AutoSettingsItemPrivate *pr);
	virtual ~AutoSettingsWidget();
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
private:
	AutoSettingsItemPrivate *p;
	QScopedPointer<AutoSettingsWidgetPrivate> g;
};

class AutoSettingsGenerator : public ObjectGenerator
{
public:
	AutoSettingsGenerator(AutoSettingsItemPrivate *pr) : p(pr) {}
protected:
	virtual ~AutoSettingsGenerator() {}
	virtual QObject *generateHelper() const
	{
		debug() << Q_FUNC_INFO;
		if(m_object.isNull())
			m_object = new AutoSettingsWidget(p);
		return m_object.data();
	}
	virtual const QMetaObject *metaObject() const
	{
		return &AutoSettingsWidget::staticMetaObject;
	}
private:
	AutoSettingsItemPrivate *p;
	mutable QWeakPointer<QObject> m_object;
};

class AutoSettingsFileChooser;
class AutoSettingsFileChooserPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(AutoSettingsFileChooser)
public:
	inline AutoSettingsFileChooserPrivate() {}
	AutoSettingsFileChooser *q_ptr;
	QLineEdit *edit;
public slots:
	void onButtonClicked(bool toggled);
};

class DataSettingsWidgetPrivate;
class DataSettingsWidget : public SettingsWidget
{
	Q_OBJECT
public:
	DataSettingsWidget(DataSettingsItemPrivate *pr);
	virtual ~DataSettingsWidget();
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
private:
	DataSettingsItemPrivate *p;
	QScopedPointer<DataSettingsWidgetPrivate> g;
};

class DataSettingsItemPrivate : public SettingsItemPrivate
{
public:
	QString config;
	QString group;
	DataItem item;
};

class DataSettingsGenerator : public ObjectGenerator
{
public:
	DataSettingsGenerator(DataSettingsItemPrivate *pr) : p(pr) {}
protected:
	virtual ~DataSettingsGenerator() {}
	virtual QObject *generateHelper() const
	{
		debug() << Q_FUNC_INFO;
		if(m_object.isNull())
			m_object = new DataSettingsWidget(p);
		return m_object.data();
	}
	virtual const QMetaObject *metaObject() const
	{
		return &DataSettingsWidget::staticMetaObject;
	}
private:
	DataSettingsItemPrivate *p;
	mutable QWeakPointer<QObject> m_object;
};

typedef QMultiMap<const QMetaObject *,SettingsItem*> MenuSettingsMap;

}

#endif // SETTINGSLAYER_P_H

