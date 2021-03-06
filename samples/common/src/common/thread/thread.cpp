//////////////////////////////////////////////////////////////////////////////
//
//    THREAD.CPP
//
//    Copyright © 2006, Rehno Lindeque. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
/*                                 INCLUDES                                 */
#include <common/types.h>
#include "thread.h"

/*                               CLASS SOURCE                               */
Mutex::Mutex() : isLocked(false)
{
  hMutex = CreateMutex(null, false, null);
}

Mutex::~Mutex()
{
  CloseHandle(hMutex);
}

void Mutex::lock()
{
  WaitForSingleObject(hMutex, INFINITE);
  isLocked = true;
}

void Mutex::unlock()
{
  ReleaseMutex(hMutex);
  isLocked = false;
}

bool Mutex::peekUnlock() const
{
  return !isLocked;
}

void Mutex::waitUnlock()
{
  while(isLocked);
}

void runThread(Thread& thread)
{
  thread.Thread::main();
}

void Thread::init()
{}

void Thread::main()
{
  init();
  lock();
    isRunning = true;
  unlock();
  while(!getFinishFlag())
    main();
  end();
  lock();
  isRunning = false;
  unlock();
}

void Thread::end()
{}

Thread::Thread() : finishFlag(false), isRunning(false), hThread(null)
{}

Thread::~Thread()
{
  finish();

  if(getIsRunning())
    waitTerminate();
}

void Thread::start()
{
  hThread = CreateThread(null, null, (LPTHREAD_START_ROUTINE) runThread, this, null, &(DWORD&)ID);
}

void Thread::finish()
{
  lock();
  finishFlag = true;
  unlock();
}

/*void Thread::terminate()
{}*/

void Thread::suspend()
{
  lock();
  SuspendThread(hThread);
  //need to lock here?
  isSuspended = true;
  unlock();
}

void Thread::resume()
{
  lock();
  isSuspended = false;
  ResumeThread(hThread);
  unlock();
}

void Thread::waitStart()
{
  while(!getIsRunning());
}

void Thread::waitTerminate()
{
  WaitForSingleObject(hThread, INFINITE);
}
