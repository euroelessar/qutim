#ifndef CAPABILTY_H
#define CAPABILTY_H

#include <QByteArray>

class Capability
{
public:
    Capability();
    Capability(const QByteArray &data);
    Capability(qint32 d1, qint32 d2, qint32 d3, qint32 d4);
    const QByteArray &data() const { return m_data; }
    bool isEmpty() const { return m_data.isEmpty(); }
    bool operator==(const Capability &rhs) const;
private:
	QByteArray m_data;
	//bool m_is_short;
};

inline uint qHash(const Capability &capability)
{
	return qHash(capability.data());
}

#endif // CAPABILTY_H
