# crow-treasure-chest
Welcome to Crow Treasure Chest, a place where crow stores the collected shiny
objects.

SpinLock class implemented in 
include/spin-lock.h
The class implements spin lock using std::atomic_flag.

CombinedLock class implemented in
include/combined-lock.h
The class implements combination of spin lock and mutex. It will use spin lock
up to a fixed amount of spins before locking the mutex.

StateMachine class implemented in
include/state-machine.h
The class implements classical finite state automata or state machine as its
name suggests.

StateMachine class depends on State class.
State class can be configured to be used with RangedTransition or FastTransition
or CompactTransition. The configuration choice affects performance and memory
usage.

StateMachineStringCheck class
include/state-machine-string-check.h
It implements string checker using finite state automata. It should be
configured to be used with StateMachine class.

AutoThread class implemented in
include/auto-thread.h, include/auto-thread-impl.h
It is a thread implementation that has a life cycle controlled by shared_ptr.

IntervalsNonIntersecting and IntervalsIntersecting class implemented in
include/intervals.h
IntervalsNonIntersecting is a collection of non-intersecting unique intervals.
IntervalsIntersecting is a collection of intersecting intervals.

RingBuffer class implemented in 
include/ring-buffer.h
It is a fixed size ring buffer allowing to store objects of random size.

Exception class and EXCEPTION macro implemented in
include/exception.h
EXCEPTION macro is going to build a string containing file name and line where
exception has been thrown.
