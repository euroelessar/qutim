#ifndef READONLYINFOLAYOUT_H
#define READONLYINFOLAYOUT_H

#include "infolayout.h"

namespace Core
{

class ReadOnlyInfoLayout : public AbstractInfoLayout
{
public:
	ReadOnlyInfoLayout(QWidget *parent = 0);
	void addItem(const InfoItem &item);
	void addItems(const QList<InfoItem> &items);
protected:
	void addDataWidget(QWidget *widget, const QString &name);
	QLabel *addLabel(const QString &data, const QString &name);
};

}

#endif // READONLYINFOLAYOUT_H
