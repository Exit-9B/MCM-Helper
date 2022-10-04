#pragma once

#include "CO/Task.h"
#include "CO/Promise.h"

namespace CO
{
	template <typename T>
	inline bool Task<T>::await_ready() const
	{
		return super::done();
	}

	template <typename T>
	inline void Task<T>::await_suspend(std::coroutine_handle<> h) const
	{
		auto& continuation = super::promise().continuation;
		if (continuation) {
			continuation.destroy();
		}

		continuation = h;
	}

	template <typename T>
	inline T Task<T>::await_resume() const
	{
		auto& p = super::promise();
		if (p.exception) {
			std::rethrow_exception(p.exception);
		}

		if constexpr (!std::is_void_v<T>) {
			return p.value;
		}
	}
}
