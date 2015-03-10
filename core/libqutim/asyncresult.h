#ifndef QUTIM_SDK_0_3_ASYNCRESULT_H
#define QUTIM_SDK_0_3_ASYNCRESULT_H

#include <QObject>
#include <QPointer>
#include <QMutex>
#include <tuple>
#include <memory>
#include <vector>
#include <functional>

namespace qutim_sdk_0_3 {

template <typename... Args>
class AsyncResult;

namespace Detail {

class Callback : public std::function<void ()>
{
public:
	Callback()
	{
	}

	Callback(std::function<void ()> other) : std::function<void ()>(std::move(other))
	{
	}

	// Otherwise std::function thinks that Callback is generic class with operator()
	Callback(const Callback &other) : std::function<void ()>(*static_cast<const std::function<void ()> *>(&other))
	{
	}

	~Callback()
	{
	}

	Callback &operator =(const Callback &other)
	{
		std::function<void ()>::operator =(other);
		return *this;
	}
};

class AsyncInvoker : public QObject
{
	Q_OBJECT
public:
	AsyncInvoker();
	~AsyncInvoker();

public slots:
	void invoke(const qutim_sdk_0_3::Detail::Callback &callback);
};

template <typename... Args>
class AsyncResultData
{
	typedef std::tuple<Args...> Tuple;
	typedef std::shared_ptr<Tuple> TuplePtr;
public:
	typedef std::function<void (const Args &...args)> Function;

	AsyncResultData() : m_invoker(new AsyncInvoker)
	{
	}

	AsyncResultData(Args ...args) : m_args(new Tuple(std::forward<Args>(args)...)), m_invoker(new AsyncInvoker)
	{
	}

	~AsyncResultData()
	{
		m_invoker->deleteLater();
	}

	AsyncResultData(const AsyncResultData &) = delete;
	AsyncResultData &operator =(const AsyncResultData &) = delete;

	void connect(QObject *object, Function function)
	{
		QPointer<QObject> context = object;
		connect_impl([context, function] (const Args &...args) {
			if (context)
				function(args...);
		});
	}

	void connect(Function function)
	{
		connect_impl(std::move(function));
	}

	void handle(Args ...args)
	{
		QMutexLocker locker(&m_lock);
		m_args = std::make_shared<Tuple>(std::forward<Args>(args)...);

		std::vector<Function> callbacks = std::move(m_callbacks);
		for (Function &callback : callbacks) {
			call(std::move(callback));
		}
	}

private:
	void connect_impl(Function function)
	{
		QMutexLocker locker(&m_lock);
		if (m_args) {
			call(std::move(function));
		} else {
			m_callbacks.emplace_back(std::move(function));
		}
	}

	template <size_t ...S>
	struct Sequence
	{
	};

	template <size_t N, size_t ...S>
	struct Generator : Generator<N-1, N-1, S...>
	{
	};

	template <size_t ...S>
	struct Generator<0, S...>
	{
		typedef Sequence<S...> type;
	};

	void call(Function function)
	{
		TuplePtr args = m_args;
		Callback callback([args, function] () {
			AsyncResultData::call(args, typename Generator<sizeof...(Args)>::type(), function);
		});

		QMetaObject::invokeMethod(m_invoker, "invoke", Qt::QueuedConnection,
								  Q_ARG(qutim_sdk_0_3::Detail::Callback, callback));
	}

	template <size_t ...S>
	static void call(TuplePtr args, Sequence<S...>, const Function &function)
	{
		function(std::get<S>(*args)...);
	}

	friend class AsyncResult<Args...>;

	QMutex m_lock;
	std::vector<Function> m_callbacks;
	std::shared_ptr<Tuple> m_args;
	AsyncInvoker *m_invoker;
};

} // namespace Detail

template <typename... Args>
class AsyncResultHandler;

template <typename... Args>
class AsyncResult
{
public:
	typedef Detail::AsyncResultData<Args...> Data;
	typedef AsyncResultHandler<Args...> Handler;
	typedef typename Data::Function Function;

	AsyncResult() noexcept
	{
	}

	AsyncResult(AsyncResult &&other) noexcept :
		m_data(std::move(other.m_data))
	{
	}

	~AsyncResult()
	{
	}

	AsyncResult &operator =(AsyncResult &&other) noexcept
	{
		using std::swap;
		swap(m_data, other.m_data);
		return *this;
	}

	static AsyncResult create(Args ...args)
	{
		AsyncResult result;
		result.m_data = std::make_shared<Data>(std::forward<Args>(args)...);
		return result;
	}

	void connect(QObject *object, Function &&function)
	{
		m_data->connect(object, std::forward<Function>(function));
	}

	void connect(Function &&function)
	{
		m_data->connect(std::forward<Function>(function));
	}

private:
	friend class AsyncResultHandler<Args...>;

	std::shared_ptr<Data> m_data;
};

template <typename... Args>
AsyncResult<typename std::remove_reference<Args>::type...> makeAsyncResult(Args ...args)
{
	return AsyncResult<typename std::remove_reference<Args>::type...>::create(std::forward<Args>(args)...);
}

template <typename... Args>
class AsyncResultHandler
{
public:
	typedef Detail::AsyncResultData<Args...> Data;

	AsyncResultHandler() : m_data(std::make_shared<Data>())
	{
	}

	AsyncResult<Args...> result() noexcept
	{
		AsyncResult<Args...> result;
		result.m_data = m_data;
		return result;
	}

	void handle(Args ...args) const
	{
		m_data->handle(std::forward<Args>(args)...);
	}

private:
	std::shared_ptr<Data> m_data;
};

} // namespace qutim_sdk_0_3

Q_DECLARE_METATYPE(qutim_sdk_0_3::Detail::Callback)

#endif // QUTIM_SDK_0_3_ASYNCRESULT_H
