/** @file Event.hpp
	Contains the thread-safe event types. */
#ifndef __libcr_mt_event_hpp_defined
#define __libcr_mt_event_hpp_defined

#include "ConditionVariable.hpp"
#include "detail/SoftMutex.hpp"
#include "../util/Atomic.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	class PODConsumableEventPattern;

	template<class ConditionVariable>
	/** Threadsafe POD repeatable event type.
	@tparam ConditionVariable:
		Which condition variable flavour to use. */
	class PODEventPattern
	{
		friend class PODConsumableEventPattern<ConditionVariable>;
		/** The event's condition variable. */
		ConditionVariable m_cv;
		/** Whether the event happened. */
		std::atomic_bool m_active;
		/** Number of currently registering coroutines. */
		util::Atomic<std::size_t> m_registering;
		/** Userspace mutex. */
		detail::PODSoftMutex m_mutex;
	public:
		/** Initialises the event.
		@param[in] active:
			Whether the event should be active from the beginning. */
		void initialise(
			bool active = false);

		/** Whether the event is active. */
		inline bool active() const;

		/** Fires the event, notifying all waiting coroutines.
			The event stays active until cleared. */
		void fire();
		/** Clears the event. */
		void clear();

		/** Helper class for waiting for an event using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The event to wait for. */
			PODEventPattern<ConditionVariable> &m_event;
		public:
			/** Initialises the wait call.
			@param[in] event:
				The event to wait for. */
			constexpr WaitCall(
				PODEventPattern<ConditionVariable> &event);

			/** Waits for the event.
				Blocks if the event did not happen yet.
			@param[in] coroutine:
				The coroutine to wait for the event.
			@return
				Whether the call blocks. */
			[[nodiscard]] sync::mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits until the event happens.
			If has not yet happened, blocks the coroutine until the event happens. To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr WaitCall wait();
	};

	template<class ConditionVariable>
	/** Threadsafe repeatable event type.
	@tparam ConditionVariable
	Which condition varaible flavour to use. */
	class EventPattern : PODEventPattern<ConditionVariable>
	{
	public:
		using PODEventPattern<ConditionVariable>::fire;
		using PODEventPattern<ConditionVariable>::clear;
		using PODEventPattern<ConditionVariable>::wait;
		using PODEventPattern<ConditionVariable>::active;

		/** Initialises the event. */
		EventPattern();
	};

	template<class ConditionVariable>
	class PODConsumableEventPattern : PODEventPattern<ConditionVariable>
	{
		using PODEventPattern<ConditionVariable>::m_cv;
		using PODEventPattern<ConditionVariable>::m_active;
		using PODEventPattern<ConditionVariable>::m_registering;
		using PODEventPattern<ConditionVariable>::m_mutex;
	public:
		using PODEventPattern<ConditionVariable>::initialise;
		using PODEventPattern<ConditionVariable>::clear;
		using PODEventPattern<ConditionVariable>::active;

		/** Notifies only one coroutine.
			The event is only set if no coroutine is waiting. */
		void fire();

		/** Helper class for waiting for a consumable event using `#CR_AWAIT`. */
		class ConsumeCall
		{
			/** Initialises the event call. */
			PODConsumableEventPattern<ConditionVariable> &m_event;
		public:
			/** Initialises the wait call.
			@param[in] event:
				The consumable event to wait for.*/
			constexpr ConsumeCall(
				PODConsumableEventPattern<ConditionVariable> &event);

			/** Waits for the event.
			@param[in] coroutine:
				The coroutine waiting for the event.
			@return
				Whether the call blocks. */
			[[nodiscard]] sync::mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits until the event happens.
			If has not yet happened, blocks the coroutine until the event happens. Otherwise, clears the event. To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr ConsumeCall consume();
		/** Tries to consume the event, if it is active.
			If it was active, consumes it and sets it to inactive.
		@return
			Whether the event was active. */
		bool try_consume();
	};

	template<class ConditionVariable>
	/** Consumable event type. */
	class ConsumableEventPattern : PODConsumableEventPattern<ConditionVariable>
	{
	public:
		using PODConsumableEventPattern<ConditionVariable>::fire;
		using PODConsumableEventPattern<ConditionVariable>::clear;
		using PODConsumableEventPattern<ConditionVariable>::consume;
		using PODConsumableEventPattern<ConditionVariable>::active;

		/** Initialises the event. */
		ConsumableEventPattern();
	};


	/** POD event type. */
	typedef PODEventPattern<PODConditionVariable> PODEvent;
	/** POD consumable event type. */
	typedef PODConsumableEventPattern<PODConditionVariable> PODConsumableEvent;
	/** POD event type supporting only one waiting coroutine. */
	typedef PODEventPattern<PODFIFOConditionVariable> PODFIFOEvent;
	/** POD consumable event type supporting only one waiting coroutine. */
	typedef PODConsumableEventPattern<PODFIFOConditionVariable> PODFIFOConsumableEvent;

	/** Event type. */
	typedef EventPattern<PODConditionVariable> Event;
	/** Consumable event type. */
	typedef ConsumableEventPattern<PODConditionVariable> ConsumableEvent;
	/** Event type supporting only one waiting coroutine. */
	typedef EventPattern<PODFIFOConditionVariable> FIFOEvent;
	/** Consumable event type supporting only one waiting coroutine. */
	typedef ConsumableEventPattern<PODFIFOConditionVariable> FIFOConsumableEvent;
}

#include "Event.inl"

#endif