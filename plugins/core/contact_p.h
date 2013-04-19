#ifndef CONTACT_P_H
#define CONTACT_P_H

#include "contact.h"
#include "presence_p.h"
#include <TelepathyQt/Contact>

class ContactPrivate
{
public:
    ContactPrivate(Contact *q_ptr) : q(q_ptr), isAvatarKnown(false), inList(true)
    {
    }

    static ContactPrivate *get(Contact *contact) { return contact->d.data(); }

    void setContact(const Tp::ContactPtr &contact);

    Tp::ContactPtr contact;
    Contact *q;
    Account *account;

    QString id;
    QString alias;
    QString avatar;
    QStringList groups;
    Presence presence;
    bool isAvatarKnown;
    bool inList;
};

#endif // CONTACT_P_H
