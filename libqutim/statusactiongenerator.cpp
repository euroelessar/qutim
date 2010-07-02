#include "statusactiongenerator_p.h"
#include "account.h"

namespace qutim_sdk_0_3
{
	Q_GLOBAL_STATIC(StatusActionHandler, statusActionHandler);

	StatusActionHandler::StatusActionHandler(QObject *parent) :
			QObject(parent), m_memberName(SLOT(changeStatus()))
	{
	}

	void StatusActionHandler::changeStatus()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		MenuController *item = action->data().value<MenuController *>();
		if (Account *account = qobject_cast<Account *>(item)) {
			account->setStatus(action->property("status").value<Status>());
		}
	}

	StatusActionGenerator::StatusActionGenerator(const Status &status) :
			ActionGenerator(*new StatusActionGeneratorPrivate)
	{
		Q_D(StatusActionGenerator);
		d->connectionType = ActionConnectionLegacy;
		d->legacyData = new LegacyActionData;
		d->status = status;
		if (d->status.icon().isNull()) {
			QIcon icon = Status::createIcon(status.type());
			if (!icon.isNull())
				d->status.setIcon(icon);
		}
		d->receiver = statusActionHandler();
		d->member = statusActionHandler()->memberName();
		d->icon = d->status.icon();
		d->text = d->status.name();
		d->type = ActionGenerator::StatusType;
		d->priority = -status.type();
	}

	QObject *StatusActionGenerator::generateHelper() const
	{
		QAction *action = prepareAction(new QAction(NULL));
		action->setProperty("status", qVariantFromValue(d_func()->status));
		return action;
	}
}
