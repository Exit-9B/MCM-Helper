#pragma once

namespace Co
{
	template <typename T>
	struct Promise;

	template <typename T = void>
	struct Task : std::coroutine_handle<Promise<T>>
	{
		using super = std::coroutine_handle<Promise<T>>;
		using promise_type = Promise<T>;

		constexpr Task() noexcept = default;
		Task(const super& a_other) : super(a_other) {}
		Task(super&& a_other) : super(std::forward<super>(a_other)) {}

		bool await_ready() const;
		void await_suspend(std::coroutine_handle<> a_handle) const;
		T await_resume() const;
	};
}

#include "CO/Task.inl"
