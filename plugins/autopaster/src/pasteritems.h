#pragma once

#include <QObject>
#include "handler.h"
#include "pasterinterface.h"

class PasterItems : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList items READ items)

public:
	PasterItems(){}
	~PasterItems(){}

	QStringList items() const
	{
		QStringList list;
		foreach (PasterInterface *pasters, AutoPasterHandler::pasters()) {
			list  << pasters->name();
		}
		return list;
	}
};
