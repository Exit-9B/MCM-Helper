#pragma once

#include "Co/Task.h"

namespace Co
{
	struct PromiseBase
	{
		std::suspend_never initial_suspend() { return {}; }

		std::suspend_always final_suspend() noexcept
		{
			if (continuation) {
				continuation.resume();
			}

			return {};
		}

		void unhandled_exception() { exception = std::current_exception(); }

		std::exception_ptr exception;
		std::coroutine_handle<> continuation;
	};

	template <typename T>
	struct Promise : PromiseBase
	{
		Task<T> get_return_object() { return Task<T>::from_promise(*this); }
		void return_value(T&& a_value) { value = std::forward<T>(a_value); }

		T value;
	};

	template <>
	struct Promise<void> : PromiseBase
	{
		Task<> get_return_object() { return Task<>::from_promise(*this); }
		void return_void() {}
	};
}
