#include "icqcontact.h"

#ifndef ICQCONTACT_PH_H
#define ICQCONTACT_PH_H

struct IcqContactPrivate
{
	IcqAccount *account;
	QString uin;
	QString name;
	quint16 group_id;
	Status status;
};

#endif // ICQCONTACT_PH_H
