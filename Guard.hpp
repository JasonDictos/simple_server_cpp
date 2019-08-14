#pragma once

// Winner of the 'Most useful class' award 5 years and counting,
// this incredibly simple but useful class just does stuff on destruction,
// (and optionally on construction) allowing you to avoid using gotos ;)
template<typename CallType>
class Guard {
public:
	static_assert(std::is_invocable_v<CallType>, "CallType must be invocable");

	// Eliminate default construction and copying/assignment
	Guard() noexcept = delete;
	Guard(const Guard &) = delete;
	Guard &operator=(const Guard &) = delete;

	// Move operator/constructor - efficiently move the scope around
	Guard(Guard &&scope) noexcept {
		operator=(std::move(scope));
	}

	Guard & operator = (Guard &&scope) noexcept {
		m_post = std::move(scope.m_post);
		scope.m_post = {};
		return *this;
	}

	// Post version, will call the invocable type when the scope destructs
	Guard(CallType &&post) noexcept :
		m_post(std::forward<CallType>(post)) {
	}

	// Pre/post version, executes the first invocable as part of construction
	// then stashes post for call on destruction
	template<typename PreCallType>
	Guard(PreCallType &&pre, CallType &&post) noexcept(false)  :
		Guard(std::move(post)) {
		std::invoke(std::move(pre));
	}

	// Upon destruction, the callback is executed
	~Guard() noexcept { exec(); }

	// Manually execute the guard.
	void exec() noexcept {
		if (!m_post)
			return;

		// Prevent any throws from the destructor
		try {
			std::invoke(m_post);
		}
		catch (...)
		{
		}

		m_post = {};
	}

	// @description
	// Cancel the guard.
	void cancel() noexcept {
		m_post = {};
	}

protected:
	std::function<void()> m_post;
};
