#ifndef READONLYDATALAYOUT_H
#define READONLYDATALAYOUT_H

#include "abstractdatalayout.h"

namespace Core
{

class ReadOnlyDataLayout : public AbstractDataLayout
{
public:
	ReadOnlyDataLayout(QWidget *parent = 0);
	bool addItem(const DataItem &item);
	bool addItems(const QList<DataItem> &items);
	static QWidget *getReadOnlyWidget(const DataItem &item);
protected:
	static QLabel *getLabel(const QString &str);
};

}

#endif // READONLYDATALAYOUT_H
