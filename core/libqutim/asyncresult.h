#ifndef QUTIM_SDK_0_3_ASYNCRESULT_H
#define QUTIM_SDK_0_3_ASYNCRESULT_H

#include <QObject>
#include <QPointer>
#include <tuple>
#include <memory>
#include <vector>
#include <functional>

namespace qutim_sdk_0_3 {

template <typename... Args>
class AsyncResult;

template <typename... Args>
class AsyncResultData
{
	typedef std::tuple<Args...> Tuple;
public:
	typedef std::function<void (const Args &...args)> Function;

	AsyncResultData() noexcept
	{
	}

	AsyncResultData(Args ...args) : m_args(new Tuple(std::forward<Args>(args)...))
	{
	}

	AsyncResultData(const AsyncResultData &) = delete;
	AsyncResultData &operator =(const AsyncResultData &) = delete;

	void connect(QObject *object, Function &&function)
	{
		if (m_args) {
			call(function);
		} else {
			m_callbacks.emplace_back(Callback { object, std::move(function) });
		}
	}

	void handle(Args ...args)
	{
		m_args.reset(new Tuple(std::forward<Args>(args)...));

		std::vector<Callback> callbacks = std::move(m_callbacks);
		for (Callback &callback : callbacks) {
			if (callback.object)
				call(callback.function);
		}
	}

private:
	struct Callback
	{
		QPointer<QObject> object;
		Function function;
	};

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

	void call(Function &function)
	{
		call_impl(typename Generator<sizeof...(Args)>::type(), function);
	}

	template <size_t ...S>
	void call_impl(Sequence<S...>, Function &function)
	{
		function(std::get<S>(*m_args)...);
	}

	friend class AsyncResult<Args...>;

	std::vector<Callback> m_callbacks;
	std::unique_ptr<Tuple> m_args;
};

template <typename... Args>
class AsyncResultHandler;

template <typename... Args>
class AsyncResult
{
public:
	typedef AsyncResultData<Args...> Data;
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
	typedef AsyncResultData<Args...> Data;

	AsyncResultHandler() : m_data(std::make_shared<Data>())
	{
	}

	AsyncResult<Args...> result() noexcept
	{
		AsyncResult<Args...> result;
		result.m_data = m_data;
		return result;
	}

	void handle(Args ...args)
	{
		m_data->handle(std::forward<Args>(args)...);
	}

private:
	std::shared_ptr<Data> m_data;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_ASYNCRESULT_H
