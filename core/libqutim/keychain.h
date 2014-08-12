#ifndef QUTIM_SDK_0_3_KEYCHAIN_H
#define QUTIM_SDK_0_3_KEYCHAIN_H

#include <QObject>

namespace qutim_sdk_0_3 {

class KeyChainResult : public QObject
{
	Q_OBJECT
public:
};

class KeyChain : public QObject
{
	Q_OBJECT
public:
	enum Flag {
		AskUser = 0x01,
		DontStoreResult = 0x02,
	};
	Q_DECLARE_FLAGS(Flags, Flag)

	explicit KeyChain(QObject *parent = 0);

	void read(const QString &name, Flags flags, const std::function<void (const QString &value, bool success)> &);
	void write(const QString &name, const QString &value, const std::function<void (bool success)> &);
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_KEYCHAIN_H
