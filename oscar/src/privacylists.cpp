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


PrivacyActionGenerator::PrivacyActionGenerator(Visibility visibility, bool invisibleMode) :
	ActionGenerator(QIcon(), LocalizedString(), PrivacyLists::instance(), SLOT(onModifyPrivacy(QAction*,QObject*))),
	m_visibility(visibility), m_invisibleMode(invisibleMode)
{
	if (invisibleMode)
		setType(actionType+0x20000);
	else
		setType(actionType+0x10000);
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
	Q_ASSERT(qobject_cast<IcqAccount*>(object) != 0);
	IcqAccount *account = reinterpret_cast<IcqAccount*>(object);
	action->setChecked(PrivacyLists::instance()->getCurrentMode(account, m_invisibleMode) == m_visibility);
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
		// FIXME: What should be done here?
		return;
	}
	action->setProperty("visibility", m_visibility);
	action->setProperty("invisibleMode", m_invisibleMode);
}

void PrivacyLists::onModifyPrivacy(QAction *action, QObject *object)
{
	Q_ASSERT(qobject_cast<IcqAccount*>(object) != 0);
	IcqAccount *account = reinterpret_cast<IcqAccount*>(object);
	Config cfg = account->config("privacy");
	Visibility visibility = static_cast<Visibility>(action->property("visibility").toInt());
	bool invisibleMode = action->property("invisibleMode").toBool();
	if (invisibleMode) {
		account->setProperty("invisibleMode", visibility);
		cfg.setValue("invisibleMode", static_cast<int>(visibility));
	} else {
		account->setProperty("visibility", visibility);
		cfg.setValue("visibility", static_cast<int>(visibility));
	}
	if (invisibleMode == (account->status() == Status::Invisible))
		setVisibility(account, visibility);
}

SeparatorGenerator::SeparatorGenerator(const LocalizedString &text, int priority, int type) :
	ActionGenerator(QIcon(), text, 0, 0)
{
	setPriority(priority);
	setType(type);
}

QObject *SeparatorGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	action->setEnabled(true);
	QFont font = action->font();
	font.setBold(true);
	action->setFont(font);
	return action;
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
	accountMenuList
			<< ActionPointer(new SeparatorGenerator(QT_TRANSLATE_NOOP("Privacy", "Visible mode:"), 105, actionType+0x10000))
			<< ActionPointer(new PrivacyActionGenerator(AllowAllUsers))
			<< ActionPointer(new PrivacyActionGenerator(BlockAllUsers))
			<< ActionPointer(new PrivacyActionGenerator(AllowPermitList))
			<< ActionPointer(new PrivacyActionGenerator(BlockDenyList))
			<< ActionPointer(new PrivacyActionGenerator(AllowContactList))
			<< ActionPointer(new SeparatorGenerator(QT_TRANSLATE_NOOP("Privacy", "Invisible mode:"), 105, actionType+0x20000))
			<< ActionPointer(new PrivacyActionGenerator(AllowPermitList, true))
			<< ActionPointer(new PrivacyActionGenerator(BlockAllUsers, true));
	Q_UNUSED(QT_TRANSLATE_NOOP("MetaController", "Additional"));
	Q_UNUSED(QT_TRANSLATE_NOOP("Privacy", "Privacy status"));
	foreach (const ActionPointer &action, accountMenuList)
		MenuController::addAction<IcqAccount>(action.data(), QList<QByteArray>() << "Additional" << "Privacy status");
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

int PrivacyLists::getCurrentMode(IcqAccount *account, bool invisibleMode)
{
	int current;
	if (account->status() != Status::Offline && invisibleMode == (account->status() == Status::Invisible)) {
		QList<FeedbagItem> items = account->feedbag()->type(SsiVisibility);
		if (items.isEmpty())
			current = NoVisibility;
		else
			current = items.first().field<quint8>(0x00CA);
	} else {
		QVariant currentVariant = invisibleMode ? account->property("invisibleMode") :
								  account->property("visibility");
		if (currentVariant.isNull()) {
			Config cfg = account->config("privacy");
			if (invisibleMode) {
				current = cfg.value("invisibleMode", static_cast<int>(AllowPermitList));
				account->setProperty("invisibleMode", current);
			} else {
				current = cfg.value("visibility", static_cast<int>(AllowContactList));
				account->setProperty("visibility", current);
			}
		} else {
			current = currentVariant.toInt();
		}
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
			SLOT(statusChanged(qutim_sdk_0_3::Status)));
}

void PrivacyLists::statusChanged(const qutim_sdk_0_3::Status &status)
{
	if (status == Status::Connecting || status == Status::Offline)
		return;
	IcqAccount *account = qobject_cast<IcqAccount*>(sender());
	Q_ASSERT(account);
	if (account->status() == Status::Offline ||
		((status == Status::Invisible) != (account->status() == Status::Invisible)))
	{
		setVisibility(account, getCurrentMode(account, status == Status::Invisible));
	}
}

} } // namespace qutim_sdk_0_3::oscar
