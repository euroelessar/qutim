#ifndef LINKSOPENNER_H
#define LINKSOPENNER_H

#include <QObject>

class LinksOpenner : public QObject
{
	Q_OBJECT

public:
	explicit LinksOpenner(QObject *parent = 0);
	Q_INVOKABLE void commandReceived(QString str);
};

#endif // LINKSOPENNER_H
