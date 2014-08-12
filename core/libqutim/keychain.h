#ifndef QUTIM_SDK_0_3_KEYCHAIN_H
#define QUTIM_SDK_0_3_KEYCHAIN_H

#include <QObject>

namespace qutim_sdk_0_3 {

class KeyChain : public QObject
{
	Q_OBJECT
public:
	explicit KeyChain(QObject *parent = 0);

signals:

public slots:

};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_KEYCHAIN_H
