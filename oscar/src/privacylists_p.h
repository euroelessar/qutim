#ifndef PRIVACYLISTS_H
#define PRIVACYLISTS_H

#include "feedbag.h"
#include <qutim/actiongenerator.h>

namespace qutim_sdk_0_3 {

class Account;

namespace oscar {

class IcqAccount;

enum Visibility
{
	NoVisibility     = 0,
	AllowAllUsers    = 1,
	BlockAllUsers    = 2,
	AllowPermitList  = 3,
	BlockDenyList    = 4,
	AllowContactList = 5
};

class PrivateListActionGenerator : public ActionGenerator
{
public:
	PrivateListActionGenerator(quint16 type, const QIcon &icon,
				const LocalizedString &text1, const LocalizedString &text2);
	virtual ~PrivateListActionGenerator();
protected:
	virtual void showImpl(QAction*, QObject*);
private:
	quint16 m_type;
	LocalizedString m_text;
	LocalizedString m_text2;
};

class PrivacyActionGenerator : public ActionGenerator
{
public:
	PrivacyActionGenerator(Visibility visibility);
	virtual ~PrivacyActionGenerator();
protected:
	virtual QObject *generateHelper() const;
	virtual void showImpl(QAction *action, QObject *object);
private:
	Visibility m_visibility;
};

class PrivacyLists : public QObject, public FeedbagItemHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::FeedbagItemHandler)
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
    PrivacyLists();
	static PrivacyLists *instance() { Q_ASSERT(self); return self; }
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	void setVisibility(IcqAccount *account, int visibility);
	Visibility getCurrentMode(IcqAccount *account, bool invisibleMode);
protected:
	bool eventFilter(QObject *obj, QEvent *e);
private slots:
	void onModifyPrivateList(QAction *action, QObject *object);
	void onModifyPrivacy(QAction *action, QObject *object);
	void accountAdded(qutim_sdk_0_3::Account *account);
	void statusChanged(const qutim_sdk_0_3::Status &status, const qutim_sdk_0_3::Status &previous);
private:
	static PrivacyLists *self;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // PRIVACYLISTS_H
