#ifndef CAPABILTY_H
#define CAPABILTY_H

#include <QByteArray>

class Capabilty
{
public:
    Capabilty();
private:
	QByteArray m_data;
	bool m_is_short;
};

#endif // CAPABILTY_H
