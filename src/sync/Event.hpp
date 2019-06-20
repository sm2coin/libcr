/** @file Event.hpp
	Contains the event class. */
#ifndef __libcr_sync_event_hpp_defined
#define __libcr_sync_event_hpp_defined

#include "ConditionVariable.hpp"
#include "Block.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	class PODConsumableEventPattern;

	template<class ConditionVariable>
	/** POD repeatable event type.
	@tparam ConditionVariable:
		Which condition variable flavour to use. */
	class PODEventPattern
	{
		friend class PODConsumableEventPattern<ConditionVariable>;
		/** The event's condition variable. */
		ConditionVariable m_cv;
		/** Whether the event happened. */
		bool m_happened;
	public:
		/** Initialises the event. */
		void initialise();

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
			[[nodiscard]] mayblock libcr_wait(
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
	class EventPattern : PODEventPattern<ConditionVariable>
	{
	public:
		using PODEventPattern<ConditionVariable>::fire;
		using PODEventPattern<ConditionVariable>::clear;
		using PODEventPattern<ConditionVariable>::wait;
		using PODEventPattern<ConditionVariable>::happened;

		/** Initialises the event. */
		EventPattern();
	};

	template<class ConditionVariable>
	/** POD consumable event type.
		Every `fire()` notifies only one coroutine. */
	class PODConsumableEventPattern : PODEventPattern<ConditionVariable>
	{
		using PODEventPattern<ConditionVariable>::m_cv;
		using PODEventPattern<ConditionVariable>::m_happened;
	public:
		using PODEventPattern<ConditionVariable>::initialise;
		using PODEventPattern<ConditionVariable>::clear;
		using PODEventPattern<ConditionVariable>::happened;

		/** Notifies only one coroutine.
			The event is only set if no coroutine is waiting. */
		void fire();

		/** Helper class for waiting for a consumable event using `#CR_AWAIT`. */
		class ConsumeCall
		{
			/** The event to wait for. */
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
			[[nodiscard]] mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits until the event happens.
			If has not yet happened, blocks the coroutine until the event happens. Otherwise, clears the event. To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr ConsumeCall consume();
	};

	template<class ConditionVariable>
	/** Consumable event type. */
	class ConsumableEventPattern : PODConsumableEventPattern<ConditionVariable>
	{
	public:
		using PODConsumableEventPattern<ConditionVariable>::fire;
		using PODConsumableEventPattern<ConditionVariable>::clear;
		using PODConsumableEventPattern<ConditionVariable>::consume;
		using PODConsumableEventPattern<ConditionVariable>::happened;

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