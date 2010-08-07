/****************************************************************************
 *  settingslayer.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "settingslayer_p.h"
#include "objectgenerator.h"
#include <QtGui/QApplication>
#include "settingswidget.h"
#include <QVariant>
#include <QCheckBox>
#include <QRadioButton>
#include "configbase.h"
#include "menucontroller.h"

namespace qutim_sdk_0_3
{
	SettingsItem::SettingsItem(SettingsItemPrivate &d) : p(&d)
	{
		p->text.setContext("Settings");
	}

	SettingsItem::SettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text) : p(new SettingsItemPrivate)
	{
		p->type = type;
		p->icon = icon;
		p->text = text;
	}

	SettingsItem::SettingsItem(Settings::Type type, const LocalizedString &text) : p(new SettingsItemPrivate)
	{
		p->type = type;
		p->text = text;
	}

	SettingsItem::~SettingsItem()
	{
		clearWidget();
		if (p->gen)
			delete p->gen;
	}

	Settings::Type SettingsItem::type() const
	{
		if(p->type < Settings::General || p->type > Settings::Special)
			return Settings::Invalid;
		return p->type;
	}

	QIcon SettingsItem::icon() const
	{
		// TODO: If icon is null choose it by type
		return p->icon;
	}

	LocalizedString SettingsItem::text() const
	{
		return p->text;
	}

	SettingsWidget *SettingsItem::widget() const
	{
		if(!p->gen)
			p->gen = generator();
		if(p->gen && p->widget.isNull()) {
			p->widget = p->gen->generate<SettingsWidget>();
			foreach (const ConnectInfo &info, p->connections)
				QObject::connect(p->widget, info.signal, info.receiver, info.member);
		}
		return p->widget;
	}

	void SettingsItem::clearWidget()
	{
		if(!p->widget.isNull())
			delete p->widget.data();
	}

	void SettingsItem::connect(const char *signal, QObject *receiver, const char *member)
	{
		Q_ASSERT(signal && receiver && member);
		p->connections << ConnectInfo(signal, receiver, member);
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
		AutoSettingsItemPrivate *d = static_cast<AutoSettingsItemPrivate *>(p.data());
		d->type = type;
		d->icon = icon;
		d->text = text;
		d->gen = new AutoSettingsGenerator(d);
	}

	AutoSettingsItem::AutoSettingsItem(Settings::Type type, const LocalizedString &text)
			: SettingsItem(*new AutoSettingsItemPrivate)
	{
		AutoSettingsItemPrivate *d = static_cast<AutoSettingsItemPrivate *>(p.data());
		d->type = type;
		d->text = text;
		d->gen = new AutoSettingsGenerator(d);
	}

	AutoSettingsItem::~AutoSettingsItem()
	{
	}

	void AutoSettingsItem::setConfig(const QString &config, const QString &group)
	{
		AutoSettingsItemPrivate *d = static_cast<AutoSettingsItemPrivate *>(p.data());
		d->config = config;
		d->group = group;
	}

	AutoSettingsItem::Entry *AutoSettingsItem::addEntry(const LocalizedString &text, const ObjectGenerator *gen)
	{
		AutoSettingsItemPrivate *d = static_cast<AutoSettingsItemPrivate *>(p.data());
		if(!gen->extends<QWidget>())
			return 0;
		Entry *entry = new Entry(text, gen);
		d->entries.append(entry);
		return entry;
	}

	const ObjectGenerator *AutoSettingsItem::generator() const
	{
		return p->gen;
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

	SettingsLayer::SettingsLayer()
	{
	}

	SettingsLayer::~SettingsLayer()
	{
	}

	namespace Settings
	{
		struct SettingsPrivate
		{
			SettingsItemList items;
			QPointer<SettingsLayer> widget;
			const ObjectGenerator *gen;
			bool inited;
		};

		static SettingsPrivate *p = 0;

		void ensure_settings_private_helper()
		{
			p = new SettingsPrivate;
			p->gen = 0;
			p->inited = false;
		}

		inline void ensure_settings_private()
		{ if(!p) ensure_settings_private_helper();}

		void registerItem(SettingsItem *item)
		{
			ensure_settings_private();
			p->items.append(item);
			if(!p->widget.isNull())
				p->widget->update(p->items);
		}

		void removeItem(SettingsItem *item)
		{
			ensure_settings_private();
			p->items.removeAll(item);
			if(!p->widget.isNull())
				p->widget->update(p->items);
		}

		void showWidget()
		{
			ensure_settings_private();
			if(isCoreInited() && p->widget.isNull())
			{
				if(!p->inited)
				{
					GeneratorList layers = moduleGenerators<SettingsLayer>();
					p->gen = layers.size() ? layers.first() : 0;
					p->inited = true;
				}
				if(p->gen)
					p->widget = p->gen->generate<SettingsLayer>();
			}
			if(!p->widget.isNull())
				p->widget->show(p->items);
			else
				qWarning("There is no '%s' module", SettingsLayer::staticMetaObject.className());
		}

		void closeWidget()
		{
			ensure_settings_private();
			if(!p->widget.isNull())
				p->widget->close();
		}
	}

	void SettingsLayer::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}

	void SettingsLayer::show(MenuController *controller)
	{
		show(controller->settings(),controller);
	}

}
