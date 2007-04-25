//
// Thread_POSIX.h
//
// $Id: //poco/Main/Foundation/include/Poco/Thread_POSIX.h#5 $
//
// Library: Foundation
// Package: Threading
// Module:  Thread
//
// Definition of the ThreadImpl class for POSIX Threads.
//
// Copyright (c) 2004-2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Foundation_Thread_POSIX_INCLUDED
#define Foundation_Thread_POSIX_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/Runnable.h"
#include "Poco/SignalHandler.h"
#include "Poco/Event.h"
#include "Poco/RefCountedObject.h"
#include "Poco/AutoPtr.h"
#include <pthread.h>
#if !defined(POCO_NO_SYS_SELECT_H)
#include <sys/select.h>
#endif
#include <errno.h>


namespace Poco {


class Foundation_API ThreadImpl
{
public:	
	enum Priority
	{
		PRIO_LOWEST_IMPL,
		PRIO_LOW_IMPL,
		PRIO_NORMAL_IMPL,
		PRIO_HIGH_IMPL,
		PRIO_HIGHEST_IMPL
	};

	ThreadImpl();				
	~ThreadImpl();

	Runnable& targetImpl() const;
	void setPriorityImpl(int prio);
	int getPriorityImpl() const;
	void startImpl(Runnable& target);

	void joinImpl();
	bool joinImpl(long milliseconds);
	bool isRunningImpl() const;
	static void sleepImpl(long milliseconds);
	static void yieldImpl();
	static ThreadImpl* currentImpl();

protected:
	static void* entry(void* pThread);
	static int mapPrio(int prio);

private:
	struct ThreadData: public RefCountedObject
	{
		ThreadData():
			pTarget(0),
			thread(0),
			prio(PRIO_NORMAL_IMPL),
			done(false)
		{
		}

		Runnable* pTarget;
		pthread_t thread;
		int       prio;
		Event     done;
	};
	
	AutoPtr<ThreadData> _pData;
	
	static pthread_key_t _currentKey;
	static bool          _haveCurrentKey;
	
#if defined(POCO_OS_FAMILY_UNIX)
	SignalHandler::JumpBufferVec _jumpBufferVec;
	friend class SignalHandler;
#endif
};


//
// inlines
//
inline int ThreadImpl::getPriorityImpl() const
{
	return _pData->prio;
}


inline void ThreadImpl::sleepImpl(long milliseconds)
{
#if defined(__VMS) || defined(__digital__)
		// This is specific to DECThreads
		struct timespec interval;
		interval.tv_sec  = milliseconds / 1000;
		interval.tv_nsec = (milliseconds % 1000)*1000000; 
		pthread_delay_np(&interval);
#else 
		struct timeval tv;
		tv.tv_sec  = milliseconds / 1000;
		tv.tv_usec = (milliseconds % 1000) * 1000;
		select(0, NULL, NULL, NULL, &tv); 	
#endif
}


inline void ThreadImpl::yieldImpl()
{
	sched_yield();
}


} // namespace Poco


#endif // Foundation_Thread_POSIX_INCLUDED
