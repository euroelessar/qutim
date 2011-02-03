#include "privacylists_p.h"
#include "icqcontact.h"
#include "icqaccount.h"
#include "icqprotocol.h"
#include <qutim/icon.h>

namespace qutim_sdk_0_3 {

namespace oscar {

const int actionType = ActionTypeContactList;

PrivacyLists *PrivacyLists::self = 0;

PrivateListActionGenerator::PrivateListActionGenerator(quint16 type, const QIcon &icon,
				const LocalizedString &text1, const LocalizedString &text2) :
	ActionGenerator(icon, text1, PrivacyLists::instance(), SLOT(onModifyPrivateList(QAction*,QObject*))),
	m_type(type), m_text(text1), m_text2(text2)
{
	setPriority(45);
	setType(actionType);
}

PrivateListActionGenerator::~PrivateListActionGenerator()
{
}

void PrivateListActionGenerator::showImpl(QAction *action, QObject *object)
{
	Q_ASSERT(qobject_cast<IcqContact*>(object) != 0);
	IcqContact *contact = reinterpret_cast<IcqContact*>(object);
	action->setVisible(contact->isInList());
	if (!action->isVisible())
		return;
	IcqAccount *account = contact->account();
	Status::Type status = account->status().type();
	action->setVisible(status != Status::Offline && status != Status::Connecting);
	if (!action->isVisible())
		return;
	if (!account->feedbag()->containsItem(m_type, contact->id()))
		action->setText(m_text);
	else
		action->setText(m_text2);
	action->setProperty("itemType", m_type);
}

void PrivacyLists::onModifyPrivateList(QAction *action, QObject *object)
{
	quint16 type = action->property("itemType").toInt();
	Q_ASSERT(qobject_cast<IcqContact*>(object) != 0);
	IcqContact *contact = reinterpret_cast<IcqContact*>(object);
	FeedbagItem item = contact->account()->feedbag()->item(type, contact->id(), 0, Feedbag::GenerateId);
	if (item.isInList())
		item.remove();
	else
		item.update();
}


PrivacyActionGenerator::PrivacyActionGenerator(Visibility visibility) :
	ActionGenerator(QIcon(), LocalizedString(), PrivacyLists::instance(), SLOT(onModifyPrivacy(QAction*,QObject*))),
	m_visibility(visibility)
{
	setPriority(100 - visibility);
}

PrivacyActionGenerator::~PrivacyActionGenerator()
{
}

QObject *PrivacyActionGenerator::generateHelper() const
{
	static QActionGroup group(0);
	QAction *action = prepareAction(new QAction(NULL));
	action->setCheckable(true);
	group.addAction(action);
	return action;
}

void PrivacyActionGenerator::showImpl(QAction *action, QObject *object)
{
	if (action->menu())
		return;

	Q_ASSERT(qobject_cast<IcqAccount*>(object) != 0);
	IcqAccount *account = reinterpret_cast<IcqAccount*>(object);

	bool isInvisible = account->status() == Status::Invisible;
	if (isInvisible && m_visibility != AllowPermitList && m_visibility != BlockAllUsers) {
		action->setVisible(false);
		return;
	}
	action->setVisible(true);

	QList<FeedbagItem> items = account->feedbag()->type(SsiVisibility);
	Visibility curVisibility = !items.isEmpty() ? (Visibility)items.first().field<quint8>(0x00CA) : NoVisibility;
	if (curVisibility == NoVisibility)
		curVisibility = PrivacyLists::instance()->getCurrentMode(account, isInvisible);
	action->setChecked(curVisibility == m_visibility);

	switch (m_visibility) {
	case AllowAllUsers:
		action->setText(QT_TRANSLATE_NOOP("Privacy", "Visible for all"));
		break;
	case BlockAllUsers:
		action->setText(QT_TRANSLATE_NOOP("Privacy", "Invisible for all"));
		break;
	case AllowPermitList:
		action->setText(QT_TRANSLATE_NOOP("Privacy", "Visible only for visible list"));
		break;
	case BlockDenyList:
		action->setText(QT_TRANSLATE_NOOP("Privacy", "Invisible only for invisible list"));
		break;
	case AllowContactList:
		action->setText(QT_TRANSLATE_NOOP("Privacy", "Visible only for contact list"));
		break;
	default:
		action->setText(QT_TRANSLATE_NOOP("Privacy", "Unknown privacy"));
		return;
	}
	action->setProperty("visibility", m_visibility);
}

void PrivacyLists::onModifyPrivacy(QAction *action, QObject *object)
{
	Q_ASSERT(qobject_cast<IcqAccount*>(object) != 0);
	IcqAccount *account = reinterpret_cast<IcqAccount*>(object);
	// Set new visibility
	Visibility visibility = (Visibility)action->property("visibility").toInt();
	setVisibility(account, visibility);
	// Store it into the config
	Config cfg = account->config("privacy");
	if (account->status() == Status::Invisible) {
		account->setProperty("invisibleMode", visibility);
		cfg.setValue("invisibleMode", visibility);
	} else {
		account->setProperty("visibility", visibility);
		cfg.setValue("visibility", visibility);
	}
}

PrivacyLists::PrivacyLists() :
	FeedbagItemHandler(30)
{
	Q_ASSERT(!self);
	self = this;
	m_types << SsiPermit << SsiDeny << SsiIgnore << SsiVisibility;
	foreach (Account *account, IcqProtocol::instance()->accounts())
		accountAdded(account);
	connect(IcqProtocol::instance(), SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), SLOT(accountAdded(qutim_sdk_0_3::Account*)));

	typedef QSharedPointer<ActionGenerator> ActionPointer;
	static QList<ActionPointer> contactMenuList;
	contactMenuList
			<< ActionPointer(new PrivateListActionGenerator(SsiPermit, Icon("im-visible-contact-icq"),
						QT_TRANSLATE_NOOP("ContactList", "Add to visible list"),
						QT_TRANSLATE_NOOP("ContactList", "Remove from visible list")))
			<< ActionPointer(new PrivateListActionGenerator(SsiDeny, Icon("im-invisible-contact-icq"),
						QT_TRANSLATE_NOOP("ContactList", "Add to invisible list"),
						QT_TRANSLATE_NOOP("ContactList", "Remove from invisible list")))
			<< ActionPointer(new PrivateListActionGenerator(SsiIgnore, Icon("im-ignore-contact-icq"),
						QT_TRANSLATE_NOOP("ContactList", "Add to ignore list"),
						QT_TRANSLATE_NOOP("ContactList", "Remove from ignore list")));
	foreach (const ActionPointer &action, contactMenuList)
		MenuController::addAction<IcqContact>(action.data());

	static QList<ActionPointer> accountMenuList;
	accountMenuList << ActionPointer(new PrivacyActionGenerator(AllowAllUsers))
			<< ActionPointer(new PrivacyActionGenerator(BlockAllUsers))
			<< ActionPointer(new PrivacyActionGenerator(AllowPermitList))
			<< ActionPointer(new PrivacyActionGenerator(BlockDenyList))
			<< ActionPointer(new PrivacyActionGenerator(AllowContactList));
	Q_UNUSED(QT_TRANSLATE_NOOP("Privacy", "Privacy status"));
	foreach (const ActionPointer &action, accountMenuList)
		MenuController::addAction<IcqAccount>(action.data(), QList<QByteArray>() << "Privacy status");

	IcqProtocol::instance()->installEventFilter(this);
}

bool PrivacyLists::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_UNUSED(feedbag);
	if (error != FeedbagError::NoError)
		return false;
	QString name;
	QString icon;
	switch (item.type()) {
	case SsiPermit: {
		name = "visible";
		if (type != Feedbag::Remove) {
			icon = "im-visible-contact-icq";
			debug() << item.name() << "has been added to visible list";
		} else {
			debug() << item.name() << "has been removed from visible list";
		}
		break;
	}
	case SsiDeny: {
		name = "invisible";
		if (type != Feedbag::Remove) {
			icon = "im-invisible-contact-icq";
			debug() << item.name() << "has been added to invisible list";
		} else {
			debug() << item.name() << "has been removed from invisible list";
		}
		break;
	}
	case SsiIgnore: {
		name = "ignore";
		if (type != Feedbag::Remove) {
			icon = "im-ignore-contact-icq";
			debug() << item.name() << "has been added to ignore list";
		} else {
			debug() << item.name() << "has been removed from ignore list";
		}
		break;
	default:
		return false;
	}
	}
	IcqContact *contact = feedbag->account()->getContact(item.name());
	if (!contact)
		return true;
	Status status = contact->status();
	if (!icon.isEmpty()) {
		QVariantHash clientInfo;
		clientInfo.insert("id", name);
		clientInfo.insert("icon", qVariantFromValue(ExtensionIcon(icon)));
		clientInfo.insert("showInTooltip", false);
		clientInfo.insert("priority", 20);
		status.setExtendedInfo(name, clientInfo);
	} else {
		status.removeExtendedInfo(name);
	}
	contact->setStatus(status);
	return true;
}

void PrivacyLists::setVisibility(IcqAccount *account, int visibility)
{
	FeedbagItem item = account->feedbag()->type(SsiVisibility, Feedbag::CreateItem).first();
	TLV data(0x00CA);
	data.append<quint8>(visibility);
	item.setField(data);
	item.setField<qint32>(0x00C9, 0xffffffff);
	item.update();
}

Visibility PrivacyLists::getCurrentMode(IcqAccount *account, bool invisibleMode)
{
	Visibility current;
	QVariant currentVariant = invisibleMode ?
							  account->property("invisibleMode") :
							  account->property("visibility");
	if (currentVariant.isNull()) {
		Config cfg = account->config("privacy");
		if (invisibleMode) {
			current = cfg.value("invisibleMode", AllowPermitList);
			account->setProperty("invisibleMode", current);
		} else {
			current = cfg.value("visibility", AllowContactList);
			account->setProperty("visibility", current);
		}
	} else {
		current = (Visibility)currentVariant.toInt();
	}
	return current;
}

bool PrivacyLists::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == ExtendedInfosEvent::eventType() && obj == IcqProtocol::instance()) {
		ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(e);
		QVariantHash extStatus;
		// Visible
		extStatus.insert("id", "visible");
		extStatus.insert("settingsDescription", tr("Show \"visible\" icon if contact in visible list"));
		event->addInfo("visible", extStatus);
		// Invisible
		extStatus.insert("id", "invisible");
		extStatus.insert("settingsDescription", tr("Show \"invisible\" icon if contact in invisible list"));
		event->addInfo("invisible", extStatus);
		// Ignore
		extStatus.insert("id", "ignore");
		extStatus.insert("settingsDescription", tr("Show \"ignore\" icon if contact in ignore list"));
		event->addInfo("ignore", extStatus);
	}
	return QObject::eventFilter(obj, e);
}

void PrivacyLists::accountAdded(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void PrivacyLists::statusChanged(const qutim_sdk_0_3::Status &status, const qutim_sdk_0_3::Status &previous)
{
	if (status == Status::Connecting || status == Status::Offline)
		return;
	IcqAccount *account = qobject_cast<IcqAccount*>(sender());
	Q_ASSERT(account);
	if (previous == Status::Offline ||
		((status == Status::Invisible) != (previous == Status::Invisible)))
	{
		setVisibility(account, getCurrentMode(account, status == Status::Invisible));
	}
}

} } // namespace qutim_sdk_0_3::oscar
