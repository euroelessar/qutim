#ifndef KEYCHAIN_KEYCHAINSERVICE_H
#define KEYCHAIN_KEYCHAINSERVICE_H

#include <qutim/keychain.h>

namespace KeyChain {

class Service : public qutim_sdk_0_3::KeyChain
{
	Q_OBJECT
public:
	explicit Service();

	qutim_sdk_0_3::AsyncResult<ReadResult> doRead(const QString &key) override;
	qutim_sdk_0_3::AsyncResult<Result> doWrite(const QString &key, const QString &value) override;
	qutim_sdk_0_3::AsyncResult<Result> doWrite(const QString &key, const QByteArray &value) override;
	qutim_sdk_0_3::AsyncResult<Result> doRemove(const QString &key) override;
};

} // namespace KeyChain

#endif // KEYCHAIN_KEYCHAINSERVICE_H
