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
	static QWidget *getReadOnlyWidget(const DataItem &item, bool *twoColumn = 0);
protected:
	static QLabel *getLabel(const QString &str);
};

}

#endif // READONLYDATALAYOUT_H
