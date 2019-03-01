# Introduction to libcr

In this document, a short introduction to the syntax of libcr is given.
Note that macro names are prefixed with `#`, so that if you use doxygen, they create links to the macros' documentations (the `#` no longer shows up in doxygen).
Also note that this is example code, and might contain all kinds of errors.
It is just meant to give a general impression of how to use the library.
More details can be found by browsing the documentation generated via doxygen.

## 1. Creating a coroutine

A coroutine is created as follows:


~~~cpp

typedef cr::SchedulerBase<cr::mt::ConditionVariable> SchedulerType;

/* ACoroutine is the type of the coroutine, and SchedulerType
   is the type of the scheduler we want to use. This can also
   be void if we do not want to have any scheduler (but then,
   CR_YIELD is no longer available inside the coroutine). */
COROUTINE(ACoroutine, SchedulerType)

/* Here, you can put functions such as getters, setters and
   more. These will be publicly visible. */


/* Everything after this is part of the Coroutine's internal
   state (its local variables). */
CR_STATE
	int i;
	int limit;

	void cr_prepare(int limit) { this->limit = limit; }
/* CR_INLINE allows us to implement the coroutine directly in
   its declaration. */
CR_INLINE
	for(i = 0; i < 10; i++)
	{
		std::cout << i << "\n";
/* CR_YIELD yields the execution, which is resumed the next
   time the scheduler is called. */
		CR_YIELD;
	}
/* This marks the end of the coroutine implementation. It is
   and implicit return statement. */
CR_INLINE_END

int main(int argc, char ** argv)
{
	ACoroutine cr;
/* This passes the arguments we want to the coroutine. Note
   that we call `prepare` instead of `cr_prepare`: This is a
   helper function that does additional initialisation. The
   first argument is the task-local storage, which we can
   leave as null for now. The rest of the arguments is used
   to invoke `cr_prepare`. */
	cr.prepare(nullptr, 100);

/* This starts the execution of the coroutine. The `start`
   function is separated from the `prepare` function, as it
   might not be optimally*/
	cr.start();

/* This calls the scheduler until no coroutine is waiting to
   be scheduled anymore. */
	while(SchedulerType::instance().schedule());

	return 0;
}

~~~

Here, we learned multiple things:

* Coroutines are declared using the `#COROUTINE` macro, which takes two arguments:
	The coroutine's type name, as well as the type name of the scheduler the coroutine should have access to via `#CR_YIELD`.
	Of course, if the coroutine never needs to manually yield, you can simply pass `void` instead of a scheduler (just beware, using CR_YIELD will result in an error then).
* The `CR_YIELD` macro allows a coroutine (with access to a scheduler) to pause its execution voluntarily, so that other coroutines can take turns executing.
* Local variables have to be put into the `#CR_STATE` section, as yielding destroys native local variables.
	This serves two purposes: It separates the public section of the coroutine state from its private, internal state, and it makes it easier to read the code.
* `#CR_INLINE` and `#CR_INLINE_END` are used to hide the boilerplate of the coroutine implementation.

## 2. Nesting coroutines

Coroutines can be nested so that they can call each other like functions.
This is done as follows:

~~~cpp
COROUTINE(Receive, void)
CR_STATE
	Connection * connection;
	char * destination;
	std::size_t size;

	void cr_prepare(
		Connection * connection,
		void * destination,
		std::size_t size)
	{
		this->connection = connection;
		this->destination = (char *)destination;
		this->size = size;
	}
CR_INLINE
	while(size)
	{
/* Pauses the coroutine, and resumes it once the event
   (connection->can_receive.wait()) happens. The second
   argument is optional and contains error handling code that
   is executed in case the event will never happen or similar
   cases. If omitted, it simply calls CR_THROW. */
		CR_AWAIT(connection->can_receive.wait(), {
			std::cerr << "Error while receiving!\n";
/* This terminates the coroutine and executes the error
   handling code in the calling coroutine. */
			CR_THROW;
		});

/* Local variables are only allowed if their lifetime does
   not contain any CR_AWAIT, CR_CALL, or CR_YIELD statements.
   This reduces the size of the coroutine's state. */
		std::size_t received = connection->recv(destination, size);
		destination += received;
		size -= received;
	}
CR_INLINE_END

COROUTINE(GetMessage, void)
/* These are publicly visible. */
	bool success;
	char message[256];
CR_STATE
/* Instead of dynamically allocating other coroutines, for
   fixed nesting depth, they can simply be included directly
   into the coroutine's state. As coroutines are POD by
   default, they can even be grouped in unions, so that
   coroutines that are invoked sequentially take up less
   space. */
	Receive receive;
	Connection * connection;

	void cr_prepare(
		Connection * connection)
	{
		this->connection = connection;
	}
CR_INLINE
/* This macro calls a coroutine in a syntax that resembles a
   normal function call. Note that no additional first
   argument has to be passed. CR_CALL can only be used from
   within a coroutine. The arguments are passed within
   parentheses, for technical reasons. The last argument
   is optional and contains error handling code. If omitted,
   it simply calls CR_THROW. */
	CR_CALL(receive, (connection, message, sizeof(message)-1), {
		success = false;
		connection->close();
/* This macro ends the coroutine, and resumes the calling
   coroutine, if any. */
		CR_RETURN;
	});
	message[sizeof(message)-1] = '\0';
	success = true;
	connection->close();
CR_INLINE_END

int main(int argc, char ** argv)
{
	GetMessage getMessage;
/* Imagine that this gets us some connection. */
	Connection * connection = getConnection();
	getMessage.start(nullptr, connection);

/* Imagine that this blocks until the connection is closed by
   the coroutine. */
	connection.event_loop();

	if(getMessage.success)
	{
		std::cout << "Received message: " << getMessage.message << "\n";
		return 0;
	} else
	{
		std::err << ":(\n";
		return 1;
	}
}
~~~

## 3. Separating declaration and implementation

Although the style of defining a coroutine within its declaration is neat, it quickly gets messy.
For this reason, coroutine implementations can be made external:

~~~cpp
COROUTINE(Something, void)

CR_STATE
	int x, y, z;
	int cr_prepare(int x, int y, int z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
/* This macro marks the coroutine implementation as external,
   just like it is done in classes. */
CR_EXTERNAL
~~~

And somewhere else, in some other file:

~~~cpp
/* This macro starts an externalised implementation of a
   coroutine. */
CR_IMPL(Something)
	// Please imagine something meaningful here.

/* This macro marks the end of a coroutine's external
   implementation. */
CR_IMPL_END
~~~

Now, we learned how to keep our code clean.

## 4. Templates and coroutines

This is simple; Just replace the `COROUTINE` macro with `#TEMPLATE_COROUTINE` like this:

~~~cpp
template<class T, std::size_t size>
TEMPLATE_COROUTINE(Coroutine, (T, size), void)
CR_EXTERNAL

template<class T, class size>
CR_IMPL(Coroutine<T, size>)
CR_IMPL_END
~~~