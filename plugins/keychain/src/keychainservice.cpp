#include "keychainservice.h"
#include <qt5keychain/keychain.h>

namespace KeyChain {

using namespace qutim_sdk_0_3;
using namespace QKeychain;

Service::Service()
{
}

static QString service_name()
{
#ifdef Q_OS_MAC
	return QStringLiteral("qutIM");
#else
	return QStringLiteral("qutim");
#endif
}

template <typename Job>
Job *createJob(const QString &key, QObject *parent)
{
	Job *job = new Job(service_name(), parent);
	job->setAutoDelete(true);
	job->setKey(key);

	return job;
}

template <typename Result, typename Job, typename Method>
AsyncResult<Result> executeJob1(Job *job, Method method)
{
	AsyncResultHandler<Result> handler;

	QObject::connect(job, &Job::finished, [job, handler, method] () {
		handler.handle(method(job));
	});

	job->start();

	return handler.result();
}

static void fillResult(qutim_sdk_0_3::KeyChain::Result &result, Job *job)
{
	switch (job->error()) {
	case NoError:
		result.error = qutim_sdk_0_3::KeyChain::NoError;
		break;
	case EntryNotFound:
		result.error = qutim_sdk_0_3::KeyChain::EntryNotFound;
		break;
	default:
		result.error = qutim_sdk_0_3::KeyChain::OtherError;
		break;
	}

	result.errorString = job->errorString();
}

template <typename Result, typename Job>
AsyncResult<Result> executeJob(Job *job)
{
	return executeJob1<Result>(job, [] (Job *job) {
		Result result;
		fillResult(result, job);
		return result;
	});
}

AsyncResult<qutim_sdk_0_3::KeyChain::ReadResult> Service::doRead(const QString &key)
{
	auto job = createJob<ReadPasswordJob>(key, this);

	return executeJob1<ReadResult>(job, [] (ReadPasswordJob *job) {
		ReadResult result;
		fillResult(result, job);
		result.textData = job->textData();
		result.binaryData = job->binaryData();
		return result;
	});
}

AsyncResult<qutim_sdk_0_3::KeyChain::Result> Service::doWrite(const QString &key, const QString &value)
{
	auto job = createJob<WritePasswordJob>(key, this);
	job->setTextData(value);
	return executeJob<Result>(job);
}

AsyncResult<qutim_sdk_0_3::KeyChain::Result> Service::doWrite(const QString &key, const QByteArray &value)
{
	auto job = createJob<WritePasswordJob>(key, this);
	job->setBinaryData(value);
	return executeJob<Result>(job);
}

AsyncResult<qutim_sdk_0_3::KeyChain::Result> Service::doRemove(const QString &key)
{
	auto job = createJob<ReadPasswordJob>(key, this);

	return executeJob<Result>(job);
}

} // namespace KeyChain
