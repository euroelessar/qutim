#ifndef READONLYINFOLAYOUT_H
#define READONLYINFOLAYOUT_H

#include "infolayout.h"

namespace Core
{

class ReadOnlyInfoLayout : public AbstractInfoLayout
{
public:
	ReadOnlyInfoLayout(QWidget *parent = 0);
	bool addItem(const InfoItem &item);
	bool addItems(const QList<InfoItem> &items);
	static QWidget *getReadOnlyWidget(const InfoItem &item);
protected:
	static QLabel *getLabel(const QString &str);
};

}

#endif // READONLYINFOLAYOUT_H
