#ifndef ASTRALROSTER_H
#define ASTRALROSTER_H

#include "astralcontact.h"

struct AstralRosterPrivate;

class AstralRoster : public QObject
{
	Q_OBJECT
public:
	AstralRoster(AstralAccount *acc, ConnectionPtr conn);
	virtual ~AstralRoster();
	AstralContact *contact(const QString &id);
	AstralContact *contact(const ContactPtr &ptr);

private slots:
	void onConnectionReady(Tp::PendingOperation *op);
//	void onPresencePublicationRequested(const Tp::Contacts &);
//	void onItemSelectionChanged();
//	void onAddButtonClicked();
//	void onAuthActionTriggered(bool);
//	void onDenyActionTriggered(bool);
//	void onRemoveActionTriggered(bool);
//	void onBlockActionTriggered(bool);
	void onContactRetrieved(Tp::PendingOperation *op);
//	void updateActions();
private:
	QScopedPointer<AstralRosterPrivate> p;
};

#endif // ASTRALROSTER_H
