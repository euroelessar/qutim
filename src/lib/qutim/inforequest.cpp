/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "inforequest.h"
#include "account.h"
#include "buddy.h"
#include "protocol.h"

namespace qutim_sdk_0_3
{

typedef QMultiHash<QObject*, InfoObserver*> ObserversHash;
Q_GLOBAL_STATIC(ObserversHash, observers)

class InfoRequestPrivate
{
public:
	InfoRequestPrivate() :
		state(InfoRequest::Initialized)
	{}
	void storeDataItem(Config &cfg, const DataItem &item);
	QObject *object;
	InfoRequest::State state;
	mutable DataItem dataItem;
	LocalizedString errorString;
};

class InfoObserverPrivate
{
public:
	QObject *object;
	InfoRequestFactory *factory;
	InfoRequestFactory::SupportLevel level;
	bool observing;
};

static inline bool isRequestBusy(InfoRequest::State state, bool showWarning = true)
{
	if (state == InfoRequest::Requesting ||
		state == InfoRequest::Updating)
	{
		if (showWarning)
			qWarning() << "Can't make two information requests simultaneously";
		return true;
	}
	return false;
}

InfoRequestFactory::~InfoRequestFactory()
{
}

InfoRequest *InfoRequestFactory::dataFormRequest(QObject *object)
{
	InfoRequestFactory *f = 0;
	if (Account *account = qobject_cast<Account*>(object))
		f = account->infoRequestFactory();
	else if (Buddy *buddy = qobject_cast<Buddy*>(object))
		f = buddy->account()->infoRequestFactory();
	else
		Q_ASSERT(!"Unsupported object");
	return f ? f->createrDataFormRequest(object) : 0;
}

InfoRequestFactory *InfoRequestFactory::factory(QObject *object)
{
	if (Account *account = qobject_cast<Account*>(object))
		return account->infoRequestFactory();
	else if (Buddy *buddy = qobject_cast<Buddy*>(object))
		return buddy->account()->infoRequestFactory();
	else
		Q_ASSERT(!"Unsupported object");
	return 0;
}

void InfoRequestFactory::setSupportLevel(QObject *object, SupportLevel level)
{
	foreach (InfoObserver *observer, observers()->values(object)) {
		observer->d_func()->level = level;
		emit observer->supportLevelChanged(level);
	}
}

InfoRequestFactory::InfoRequestFactory()
{
}

InfoRequest::~InfoRequest()
{
}

void InfoRequestFactory::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

QObject *InfoRequest::object() const
{
	return d_func()->object;
}

InfoRequest::State InfoRequest::state() const
{
	return d_func()->state;
}

DataItem InfoRequest::dataItem() const
{
	Q_D(const InfoRequest);
	if (d->dataItem.isNull())
		d->dataItem = createDataItem();
	return d->dataItem;
}

LocalizedString InfoRequest::errorString() const
{
	Q_D(const InfoRequest);
	if (d->errorString.isNull())
		return QT_TRANSLATE_NOOP("InformationRequest", "Unknown error");
	return d->errorString;
}

void InfoRequest::requestData(const QSet<QString> &hints)
{
	Q_D(InfoRequest);
	if (!isRequestBusy(d->state))
		doRequest(hints);
}

void InfoRequest::updateData(const DataItem &dataItem)
{
	Q_D(InfoRequest);
	if (!isRequestBusy(d->state))
		doUpdate(dataItem);
}

void InfoRequest::cancel()
{
	Q_D(InfoRequest);
	if (isRequestBusy(d->state, false))
		doCancel();
}

QVariant InfoRequest::value(const QString &name, const QVariant &def) const
{
	QVariant val = getValue(name);
	return val.isNull() ? def : val;
}

InfoRequest::InfoRequest(QObject *object) :
	d_ptr(new InfoRequestPrivate)
{
	d_ptr->object = object;
}

void InfoRequest::setState(State state)
{
	Q_D(InfoRequest);
	d->state = state;
	if (state == InfoRequest::RequestDone ||
		state == InfoRequest::LoadedFromCache ||
		state == InfoRequest::Initialized)
	{
		d->dataItem = DataItem();
	}
	emit stateChanged(state);
}

void InfoRequest::setErrorString(const LocalizedString &errorString)
{
	Q_D(InfoRequest);
	d->errorString = errorString;
	emit errorStringChanged(errorString);
}

QVariant InfoRequest::getValue(const QString &name) const
{
	return dataItem().subitem(name, true).data();
}

void InfoRequest::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

InfoObserver::InfoObserver(QObject *object) :
	QObject(object),
	d_ptr(new InfoObserverPrivate)
{
	Q_D(InfoObserver);
	d->object = object;
	connect(object, SIGNAL(destroyed(QObject*)), SLOT(onObjectDestroyed(QObject*)));
	d->factory = InfoRequestFactory::factory(object);
	if (d->factory && (observers()->contains(object) || d->factory->startObserve(object))) {
		observers()->insert(object, this);
		d->level = d->factory->supportLevel(object);
		d->observing = true;
	} else {
		d->level = InfoRequestFactory::NotSupported;
		d->observing = false;
	}
}

InfoObserver::~InfoObserver()
{
	Q_D(InfoObserver);
	if (d->observing && d->object) {
		observers()->remove(d->object, this);
		if (!observers()->contains(d->object))
			d->factory->stopObserve(d->object);
	}
}

QObject *InfoObserver::object() const
{
	return d_func()->object;
}

InfoRequestFactory::SupportLevel InfoObserver::supportLevel() const
{
	return d_func()->level;
}

void InfoObserver::onObjectDestroyed(QObject *object)
{
	Q_D(InfoObserver);
	Q_ASSERT(d->object = object);
	d->object = 0;
	if (observers()->remove(object, this) > 0)
		emit supportLevelChanged(InfoRequestFactory::NotSupported);
}

}

