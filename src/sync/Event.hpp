/** @file Event.hpp
	Contains the event class. */
#ifndef __libcr_sync_event_hpp_defined
#define __libcr_sync_event_hpp_defined

#include "ConditionVariable.hpp"
#include "Block.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	/** POD repeatable event type.
	@tparam ConditionVariable:
		Which condition variable flavour to use. */
	class PODEventBase
	{
		/** The event's condition variable. */
		ConditionVariable m_cv;
		/** Whether the event happened. */
		bool m_happened;
	public:
		/** Initialises the event. */
		void initialise();

		/** Whether any coroutines are listening for the event. */
		inline bool listeners() const;

		/** Sets the event.
			Notifies all waiting coroutines. The event must not have happened already. */
		void fire();

		/** Clears the event.
			The event must have happened already. */
		void clear();

		/** Helper class for waiting for an event using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The event to wait for. */
			PODEventBase<ConditionVariable> &m_event;
		public:
			/** Initialises the wait call.
			@param[in] event:
				The event to wait for. */
			constexpr WaitCall(
				PODEventBase<ConditionVariable> &event);

			/** Waits for the event.
				Blocks if the event did not happen yet.
			@param[in] coroutine:
				The coroutine to wait for the event.
			@return
				Whether the call blocks. */
			mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits until the event happens.
			If has not yet happened, blocks the coroutine until the event happens. To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr WaitCall wait();

		/** Whether the event happened. */
		inline bool happened() const;
	};

	template<class ConditionVariable>
	/** Repeatable event type.
	@tparam ConditionVariable:
		Which condition variable flavour to use. */
	class EventBase : PODEventBase<ConditionVariable>
	{
	public:
		using PODEventBase<ConditionVariable>::fire;
		using PODEventBase<ConditionVariable>::clear;
		using PODEventBase<ConditionVariable>::wait;
		using PODEventBase<ConditionVariable>::happened;

		/** Initialises the event. */
		EventBase();
	};

	/** POD event type. */
	typedef PODEventBase<PODConditionVariable> PODEvent;
	/** POD event type supporting only one waiting coroutine. */
	typedef PODEventBase<PODSingleConditionVariable> PODSingleEvent;

	/** Event type. */
	typedef EventBase<PODConditionVariable> Event;
	/** Event type supporting only one waiting coroutine. */
	typedef EventBase<PODSingleConditionVariable> SingleEvent;
}

#include "Event.inl"

#endif