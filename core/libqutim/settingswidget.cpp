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

#include "settingswidget.h"
#include <QAbstractButton>
#include <QAbstractSlider>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QSignalMapper>
#include <QMetaObject>
#include <QMetaProperty>
#include <QWeakPointer>
#include <algorithm>

namespace qutim_sdk_0_3
{
class AbstractWidgetInfo
{
public:
	AbstractWidgetInfo(const char *p, const char *s) : property(p), signal(s) {}
	~AbstractWidgetInfo() {}
	virtual bool handle(QObject *obj) = 0;
	const char * const property;
	const char * const signal;
};

template <typename T>
class WidgetMetaInfo : public AbstractWidgetInfo
{
public:
	WidgetMetaInfo(const char *p, const char *s) : AbstractWidgetInfo(p, s) {}
	virtual bool handle(QObject *obj) { return qobject_cast<T*>(obj) != NULL; }
};

static AbstractWidgetInfo * widget_infos[] = {
	new WidgetMetaInfo<QAbstractButton>("checked",      SIGNAL(toggled(bool))),
	new WidgetMetaInfo<QAbstractSlider>("value",        SIGNAL(valueChanged(int))),
	new WidgetMetaInfo<QComboBox>      ("currentIndex", SIGNAL(currentIndexChanged(int))),
	new WidgetMetaInfo<QDateTimeEdit>  ("dateTime",     SIGNAL(dateTimeChanged(QDateTime))),
	new WidgetMetaInfo<QLineEdit>      ("text",         SIGNAL(textChanged(QString))),
	new WidgetMetaInfo<QListWidget>    ("currentRow",   SIGNAL(currentRowChanged(int))),
	new WidgetMetaInfo<QSpinBox>       ("value",        SIGNAL(valueChanged(int)))
};

//static struct WidgetMetaInfo
//{
//	const QMetaObject *meta;
//	const char *property;
//	const char *signal;
//} widget_infos[] =  {
//#if	!defined(Q_OS_SYMBIAN)	 //staticMetaObject doesn't link on Symbian platform
//	{ &QAbstractButton::staticMetaObject,   "checked",      SIGNAL(toggled(bool)) },
//	{ &QAbstractSlider::staticMetaObject,   "value",        SIGNAL(valueChanged(int)) },
//	{ &QComboBox::staticMetaObject,         "currentIndex", SIGNAL(currentIndexChanged(int)) },
//	{ &QDateTimeEdit::staticMetaObject,     "dateTime",     SIGNAL(dateTimeChanged(QDateTime)) },
//	{ &QLineEdit::staticMetaObject,         "text",         SIGNAL(textChanged(QString)) },
//	{ &QListWidget::staticMetaObject,       "currentRow",   SIGNAL(currentRowChanged(int)) },
//	{ &QSpinBox::staticMetaObject,          "value",        SIGNAL(valueChanged(int)) }
//#endif
//};

struct WidgetInfo
{
	QWeakPointer<QWidget> obj;
	const char *property;
	QVariant value;
	bool is_changed;
};

struct SettingsWidgetPrivate
{
	QSignalMapper *mapper;
	QList<WidgetInfo> infos;
	uint changed_num;
	bool modified;
	bool sleep; // Used to block signal sending on widget changing it's status
	void clearValues();
};

void SettingsWidgetPrivate::clearValues()
{
	for(int i = 0, size = infos.size(); i < size; i++)
	{
		WidgetInfo &info = infos[i];
		if(info.obj)
			info.value = info.obj.data()->property(info.property);
		else
			info.value.clear();
		info.is_changed = false;
	}
	changed_num = 0;
	modified = false;
}

SettingsWidget::SettingsWidget(QWidget *parent) :
	QWidget(parent),
	p(new SettingsWidgetPrivate)
{
	p->mapper = new QSignalMapper(this);
	connect(p->mapper, SIGNAL(mapped(int)), this, SLOT(onStateChanged(int)));
	p->changed_num = 0;
	p->modified = false;
	p->sleep = true;
}

SettingsWidget::~SettingsWidget()
{
}

bool SettingsWidget::isModified() const
{
	return (p->modified || p->changed_num > 0);
}

void SettingsWidget::load()
{
	p->sleep = true;
	loadImpl();
	p->clearValues();
	p->sleep = false;
}

void SettingsWidget::save()
{
	if(!isModified())
		return;
	p->sleep = true;
	saveImpl();
	p->clearValues();
	p->sleep = false;
	emit saved();
	emit modifiedChanged(false);
}

void SettingsWidget::cancel()
{
	if(!isModified())
		return;
	p->sleep = true;
	for(int i = 0, size = p->infos.size(); i < size; i++)
	{
		WidgetInfo &info = p->infos[i];
		if(info.obj && info.is_changed)
			 info.obj.data()->setProperty(info.property, info.value);
		info.is_changed = false;
	}
	cancelImpl();
	p->changed_num = 0;
	p->modified = false;
	p->sleep = false;
	emit modifiedChanged(false);
}

#ifdef Q_CC_MSVC
#   pragma warning (push)
#   pragma warning (disable : 4996) // MSVC thinks that QList is unsafe
#endif
void SettingsWidget::listenChildrenStates(const QWidgetList &exceptions)
{
	QWidgetList widgets;
	if (!exceptions.isEmpty()) {
		QWidgetList ls1 = findChildren<QWidget *>();
		QWidgetList ls2 = exceptions;
		std::sort(ls1.begin(), ls1.end());
		std::sort(ls2.begin(), ls2.end());
		std::set_difference(ls1.begin(), ls1.end(), ls2.begin(), ls2.end(),
							std::insert_iterator<QWidgetList>(widgets, widgets.begin()));
	} else {
		widgets = findChildren<QWidget *>();
	}
	foreach (QWidget *widget, widgets)
		lookForWidgetState(widget);
}
#ifdef Q_CC_MSVC
#   pragma warning (pop)
#endif

const char *SettingsWidget::lookForWidgetState(QWidget *widget, const char *property, const char *signal)
{
	const QMetaObject *meta = widget->metaObject();
	WidgetInfo info = { widget, NULL, QVariant(), false };
	bool free_signal = false;
	// Firstly try to search this widget in predefined classes
	if (!signal && !property) {
		for (int i = 0, size = sizeof(widget_infos) / sizeof(AbstractWidgetInfo*); i < size; i++) {
			if (widget_infos[i]->handle(widget)) {
				info.property = widget_infos[i]->property;
				signal = widget_infos[i]->signal;
				break;
			}
		}
	}
	// Then try to find "User" property with signal or signal by property
	if (!signal) {
		for (int i = 0, size = meta->propertyCount(); i < size; i++) {
			QMetaProperty prop = meta->property(i);
			if (prop.hasNotifySignal()
				&& ((property && !qstrcmp(prop.name(), property))
					|| (!property && prop.isUser()))) {
				info.property = prop.name();
				const char *sig = prop.notifySignal().signature();
				int len = strlen(sig);
				char *str = (char *)qMalloc(sizeof(char) * (len + 2));
				str[0] = QSIGNAL_CODE;
				qstrcpy(str + 1, sig);
				signal = str;
				free_signal = true;
				break;
			}
		}
	}
	bool result(signal);
	if (result) {
		p->mapper->setMapping(widget, p->infos.size());
		connect(widget, signal, p->mapper, SLOT(map()));
		p->infos << info;
	}
	if (free_signal)
		qFree((void *)signal);
	return result ? info.property : 0;
}

void SettingsWidget::onStateChanged(int index)
{
	if (index < 0 || index >= p->infos.size() || p->sleep)
		return;
	const bool oldModified = isModified();
	WidgetInfo &info = p->infos[index];
	QVariant value = info.obj.data()->property(info.property);
	bool equal = info.value == value;
	if (equal && info.is_changed)
		p->changed_num--;
	else if (!info.is_changed && !equal)
		p->changed_num++;
	else
		return;
	info.is_changed = !equal;
	if (oldModified != isModified())
		emit modifiedChanged(!oldModified);
}

void SettingsWidget::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

void SettingsWidget::setController(QObject *controller)
{
	Q_UNUSED(controller);
}

void SettingsWidget::setModified(bool modified)
{
	const bool oldModified = isModified();
	p->modified = modified;
	if (oldModified != isModified())
		emit modifiedChanged(!oldModified);
}

}

