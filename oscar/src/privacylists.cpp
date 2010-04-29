#include "privacylists_p.h"
#include "icqcontact.h"
#include "icqaccount.h"
#include <qutim/icon.h>

namespace qutim_sdk_0_3 {

namespace oscar {

PrivacyLists *PrivacyLists::self = 0;

PrivateListActionGenerator::PrivateListActionGenerator(quint16 type, const QIcon &icon,
				const LocalizedString &text1, const LocalizedString &text2) :
	ActionGenerator(icon, text1, PrivacyLists::instance(), SLOT(onModifyPrivateList())),
	m_type(type), m_text2(text2)
{
	setPriority(45);
	setType(34563);
}

PrivateListActionGenerator::~PrivateListActionGenerator()
{
}

QObject *PrivateListActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	IcqContact *contact = qobject_cast<IcqContact*>(action->data().value<MenuController*>());
	Q_ASSERT(contact);
	if (contact->account()->feedbag()->containsItem(m_type, contact->id()))
		action->setText(m_text2);
	action->setProperty("itemType", m_type);
	return action;
}

void PrivacyLists::onModifyPrivateList()
{
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	quint16 type = action->property("itemType").toInt();
	IcqContact *contact = qobject_cast<IcqContact*>(action->data().value<MenuController*>());
	Q_ASSERT(contact);
	FeedbagItem item = contact->account()->feedbag()->item(type, contact->id(), 0, Feedbag::GenerateId);
	if (item.isInList())
		item.remove();
	else
		item.update();
}

PrivacyLists::PrivacyLists()
{
	Q_ASSERT(!self);
	self = this;
	m_types << SsiPermit << SsiDeny << SsiIgnore;

	typedef QSharedPointer<PrivateListActionGenerator> ActionPointer;
	static QList<ActionPointer> list;
	list << ActionPointer(new PrivateListActionGenerator(SsiPermit, Icon("visible-icq"),
						  QT_TRANSLATE_NOOP("ContactList", "Add to visible list"),
						  QT_TRANSLATE_NOOP("ContactList", "Remove from visible list")))
		<< ActionPointer(new PrivateListActionGenerator(SsiDeny, Icon("invisible-icq"),
						 QT_TRANSLATE_NOOP("ContactList", "Add to invisible list"),
						 QT_TRANSLATE_NOOP("ContactList", "Remove from invisible list")))
		<< ActionPointer(new PrivateListActionGenerator(SsiIgnore, Icon("ignore-icq"),
						 QT_TRANSLATE_NOOP("ContactList", "Add to ignore list"),
						 QT_TRANSLATE_NOOP("ContactList", "Remove from ignore list")));
	foreach (const ActionPointer &action, list)
		MenuController::addAction<IcqContact>(action.data());
}

bool PrivacyLists::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_UNUSED(feedbag);
	if (error != FeedbagError::NoError)
		return false;
	switch (item.type()) {
	case SsiPermit: {
		if (type != Feedbag::Remove)
			debug() << item.name() << "has been added to visible list";
		else
			debug() << item.name() << "has been removed from visible list";
		break;
	}
	case SsiDeny: {
		if (type != Feedbag::Remove)
			debug() << item.name() << "has been added to invisible list";
		else
			debug() << item.name() << "has been removed from invisible list";
		break;
	}
	case SsiIgnore: {
		if (type != Feedbag::Remove)
			debug() << item.name() << "has been added to ignore list";
		else
			debug() << item.name() << "has been removed from ignore list";
		break;
	default:
		return false;
	}
	}
	return true;
}

void PrivacyLists::setVisibility(IcqAccount *account, Visibility visibility)
{
	FeedbagItem item = account->feedbag()->type(SsiVisibility, Feedbag::CreateItem).first();
	TLV data(0x00CA);
	data.append<quint8>(visibility);
	item.setField(data);
	item.setField<qint32>(0x00C9, 0xffffffff);
	item.update();
}

} } // namespace qutim_sdk_0_3::oscar
