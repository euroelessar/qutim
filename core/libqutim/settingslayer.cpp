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

#include "settingslayer_p.h"
#include "objectgenerator.h"
#include <QCoreApplication>
#include "settingswidget.h"
#include "dataforms.h"
#include <QVariant>
#include <QCheckBox>
#include <QRadioButton>
#include "configbase.h"
#include "menucontroller.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QToolButton>
#include <QSet>
#include "icon.h"
#include "servicemanager.h"
#include "systemintegration.h"

namespace qutim_sdk_0_3
{

SettingsItem::SettingsItem(SettingsItemPrivate &d) : d_ptr(&d)
{
	d_ptr->text.setContext("Settings");
}

SettingsItem::SettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text) : d_ptr(new SettingsItemPrivate)
{
	Q_D(SettingsItem);
	d->type = type;
	d->icon = icon;
	d->text = text;
}

SettingsItem::SettingsItem(Settings::Type type, const LocalizedString &text) : d_ptr(new SettingsItemPrivate)
{
	Q_D(SettingsItem);
	d->type = type;
	d->text = text;
}

SettingsItem::~SettingsItem()
{
	Q_D(SettingsItem);
	Settings::removeItem(this);
	clearWidget();
	if (d->gen)
		delete d->gen;
}

Settings::Type SettingsItem::type() const
{
	Q_D(const SettingsItem);
	if(d->type < Settings::General || d->type > Settings::Special)
		return Settings::Invalid;
	return d->type;
}

QIcon SettingsItem::icon() const
{
	// TODO: If icon is null choose it by type
	return d_func()->icon;
}

LocalizedString SettingsItem::text() const
{
	return d_func()->text;
}

SettingsWidget *SettingsItem::widget() const
{
	return qobject_cast<SettingsWidget*>(object());
}

QGraphicsObject *SettingsItem::graphicsObject() const
{
	return qobject_cast<QGraphicsObject*>(object());
}

QObject *SettingsItem::object() const
{
	Q_D(const SettingsItem);
	if (!d->gen)
		d->gen = SystemIntegration::settingsGenerator(const_cast<SettingsItem*>(this));
	if (!d->gen)
		d->gen = generator();
	if(d->gen && d->object.isNull()) {
		d->object = d->gen->generate<QObject>();
		foreach (const ConnectInfo &info, d->connections)
			QObject::connect(d->object.data(), info.signal, info.receiver.data(), info.member);
	}
	return d->object.data();
}

bool SettingsItem::isWidget() const
{
	Q_D(const SettingsItem);
	if (!d->gen)
		d->gen = SystemIntegration::settingsGenerator(const_cast<SettingsItem*>(this));
	if (!d->gen)
		d->gen = generator();
	return d->gen && d->gen->extends<SettingsWidget>();
}

void SettingsItem::clearWidget()
{
	Q_D(SettingsItem);
	if(!d->object.isNull())
		delete d->object.data();
}

void SettingsItem::connect(const char *signal, QObject *receiver, const char *member)
{
	Q_ASSERT(signal && receiver && member);
	d_func()->connections << ConnectInfo(signal, receiver, member);
}

int SettingsItem::order() const
{
	return d_func()->order;
}

void SettingsItem::setOrder(int order)
{
	d_func()->order = order;
}

AutoSettingsWidget::AutoSettingsWidget(AutoSettingsItemPrivate *pr) : p(pr), g(new AutoSettingsWidgetPrivate)
{
	QFormLayout *layout = new QFormLayout(this);
	setLayout(layout);
	foreach(AutoSettingsItem::Entry *entry, p->entries)
	{
		QWidget *widget = entry->widget(this);
		const char *prop;
		if(!widget || !(prop = lookForWidgetState(widget)))
		{
			delete widget;
			continue;
		}
		widget->setObjectName(entry->name());
		if(QAbstractButton *button = qobject_cast<QAbstractButton *>(widget))
		{
			button->setText(entry->text());
			layout->addRow(QString(), widget);
		}
		else
			layout->addRow(entry->text(), widget);
		g->entries.append(qMakePair(widget, QByteArray(prop)));
	}
}

AutoSettingsWidget::~AutoSettingsWidget()
{
}

void AutoSettingsWidget::loadImpl()
{
	ConfigGroup group = Config(p->config).group(p->group);
	foreach(const AutoSettingsEntryInfo &entry, g->entries)
		entry.first->setProperty(entry.second, group.value(entry.first->objectName(), QVariant()));
}

void AutoSettingsWidget::saveImpl()
{
	ConfigGroup group = Config(p->config).group(p->group);
	foreach(const AutoSettingsEntryInfo &entry, g->entries)
		group.setValue(entry.first->objectName(), entry.first->property(entry.second));
	group.sync();
}

void AutoSettingsWidget::cancelImpl()
{
}

AutoSettingsItem::Entry::Entry(const LocalizedString &text, const ObjectGenerator *gen) : p(new EntryPrivate)
{
	p->text = text;
	p->gen = gen;
}

AutoSettingsItem::Entry::~Entry()
{
}

AutoSettingsItem::Entry *AutoSettingsItem::Entry::setProperty(const char *name, QVariant value)
{
	if(name && *name)
		p->properties << qMakePair(QByteArray(name), value);
	return this;
}
AutoSettingsItem::Entry *AutoSettingsItem::Entry::setName(const QString &name)
{
	p->name = name;
	return this;
}

const LocalizedString &AutoSettingsItem::Entry::text() const
{
	return p->text;
}

const ObjectGenerator *AutoSettingsItem::Entry::generator() const
{
	return p->gen;
}

QWidget *AutoSettingsItem::Entry::widget(QWidget *parent) const
{
	if(QWidget *widget = p->gen->generate<QWidget>())
	{
		widget->setParent(parent);
		typedef QPair<QByteArray, QVariant> Prop;
		foreach(const Prop &prop, p->properties)
			widget->setProperty(prop.first, prop.second);
		return widget;
	}
	return 0;
}

const QString &AutoSettingsItem::Entry::name() const
{
	return p->name;
}

AutoSettingsItem::AutoSettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text)
	: SettingsItem(*new AutoSettingsItemPrivate)
{
	Q_D(AutoSettingsItem);
	d->type = type;
	d->icon = icon;
	d->text = text;
	d->gen = new AutoSettingsGenerator(d);
}

AutoSettingsItem::AutoSettingsItem(Settings::Type type, const LocalizedString &text)
	: SettingsItem(*new AutoSettingsItemPrivate)
{
	Q_D(AutoSettingsItem);
	d->type = type;
	d->text = text;
	d->gen = new AutoSettingsGenerator(d);
}

AutoSettingsItem::~AutoSettingsItem()
{
}

void AutoSettingsItem::setConfig(const QString &config, const QString &group)
{
	Q_D(AutoSettingsItem);
	d->config = config;
	d->group = group;
}

AutoSettingsItem::Entry *AutoSettingsItem::addEntry(const LocalizedString &text, const ObjectGenerator *gen)
{
	Q_D(AutoSettingsItem);
	if(!gen->extends<QWidget>())
		return 0;
	Entry *entry = new Entry(text, gen);
	d->entries.append(entry);
	return entry;
}

const ObjectGenerator *AutoSettingsItem::generator() const
{
	return d_func()->gen;
}

QStringList AutoSettingsComboBox::items() const
{
	QStringList ls;
	for(int i = 0, size = count(); i < size; i++)
		ls << itemText(i);
	return ls;
}

void AutoSettingsComboBox::setItems(const QStringList &ls)
{
	clear();
	addItems(ls);
}

AutoSettingsFileChooser::AutoSettingsFileChooser(QWidget *parent) :
	QWidget(parent),
	d_ptr(new AutoSettingsFileChooserPrivate)
{
	Q_D(AutoSettingsFileChooser);
	d->q_ptr = this;
	new QHBoxLayout(this);
	d->edit = new QLineEdit(this);
	connect(d->edit,SIGNAL(textChanged(QString)),SIGNAL(pathChanged(QString)));
	QToolButton *btn = new QToolButton(this);
	btn->setText("...");
	layout()->addWidget(d->edit);
	layout()->addWidget(btn);
	connect(btn,SIGNAL(clicked(bool)),d,SLOT(onButtonClicked(bool)));
}

AutoSettingsFileChooser::~AutoSettingsFileChooser()
{

}

void AutoSettingsFileChooser::setPath(const QString &p)
{
	Q_D(AutoSettingsFileChooser);
	d->edit->setText(p);
}

QString AutoSettingsFileChooser::path() const
{
	return d_func()->edit->text();
}

void AutoSettingsFileChooserPrivate::onButtonClicked(bool)
{
	Q_Q(AutoSettingsFileChooser);
	q->setPath(QFileDialog::getSaveFileName(q, QT_TRANSLATE_NOOP("Settings","Open File"),
											q->path(),
											QT_TRANSLATE_NOOP("Settings","Log file (*.log)")));
}

SettingsLayer::SettingsLayer()
{
	if (1) {} else Q_UNUSED(QT_TRANSLATE_NOOP("Service", "Settings Window"));
}

SettingsLayer::~SettingsLayer()
{
}

namespace Settings
{

struct ActionEntry
{
	ActionEntry(const LocalizedString &t,const QString &i = QString())
	{
		text = t;
		iconName = i;
	}
	ActionEntry() {}
	LocalizedString text;
	QString iconName;
};

typedef QMap<Settings::Type,ActionEntry> ActionEntryMap;

struct SettingsPrivate
{
	SettingsItemList items;
	ActionEntryMap entries;
	MenuSettingsMap map;
	ServicePointer<SettingsLayer> service;
};

bool itemLessThan(const SettingsItem *a,const SettingsItem *b)
{
	if (a->type() != b->type())
		return (a->type() < b->type());

	if (a->priority() != b->priority())
		return a->priority() < b->priority();

	return a->text().toString().compare(b->text().toString(), Qt::CaseInsensitive) >= 0;
}

static SettingsPrivate *p = 0;

inline void settings_private_helper()
{
	p = new SettingsPrivate;
	p->entries.insert(Settings::General,ActionEntry(QT_TRANSLATE_NOOP("Settings","General"),QLatin1String("preferences-system")));
	p->entries.insert(Settings::Protocol,ActionEntry(QT_TRANSLATE_NOOP("Settings","Protocols"),QLatin1String("applications-internet")));
	p->entries.insert(Settings::Appearance,ActionEntry(QT_TRANSLATE_NOOP("Settings","Appearance"),QLatin1String("applications-graphics")));
	p->entries.insert(Settings::Plugin,ActionEntry(QT_TRANSLATE_NOOP("Settings","Plugins"),QLatin1String("applications-other")));
	p->entries.insert(Settings::Special,ActionEntry(QT_TRANSLATE_NOOP("Settings","Special")));
	p->entries.insert(Settings::Invalid,ActionEntry(QT_TRANSLATE_NOOP("Settings","Invalid")));
}
void ensure_settings_private()
{
	if(!p)
		settings_private_helper();
}

void registerItem(qutim_sdk_0_3::SettingsItem* item, const QMetaObject* meta)
{
	Q_ASSERT(item && meta);
	ensure_settings_private();
	p->map.insert(meta,item);
}

SettingsItemList items(const QMetaObject *meta)
{
	ensure_settings_private();
	QSet<const QMetaObject *> metaObjects;
	SettingsItemList list;
	while (meta) {
		if (metaObjects.contains(meta))
			break;
		list.append(p->map.values(meta));
		metaObjects.insert(meta);
		meta = meta->superClass();
	}
	return list;
}

LocalizedString getTypeTitle(Type type)
{
	ensure_settings_private();
	return p->entries.value(type).text;
}

QIcon getTypeIcon(Type type)
{
	ensure_settings_private();
	return Icon(p->entries.value(type).iconName);
}

void registerItem(SettingsItem *item)
{
	ensure_settings_private();
	SettingsItemList::iterator before = qLowerBound(p->items.begin(),
													p->items.end(),
													item,
													itemLessThan
													);
	if (before != p->items.end() && *before == item)
		return;
	p->items.insert(before,item);
	if (p->service)
		p->service->update(p->items);
}

void removeItem(SettingsItem *item)
{
	ensure_settings_private();
	if (p->items.removeAll(item) > 0 && p->service)
		p->service->update(p->items);
}

void showWidget()
{
	ensure_settings_private();
	if (p->service)
		p->service->show(p->items);
}

void closeWidget()
{
	if (p->service)
		p->service->close();
}
}

void SettingsLayer::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

void SettingsLayer::show(MenuController *controller)
{	
	show(Settings::items(controller->metaObject()), controller);
}

int SettingsItem::priority() const
{
	return d_ptr->priority;
}

void SettingsItem::setPriority(int priority)
{
	d_ptr->priority = priority;
}


}

