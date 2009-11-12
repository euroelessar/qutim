/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "thread.h"

#include "config.h"

#if defined( _WIN32 ) && !defined( __SYMBIAN32__ )
# include <windows.h>
#endif

#ifdef _WIN32_WCE
# include <winbase.h>
#endif

#ifdef HAVE_PTHREAD
# include <pthread.h>
#endif

namespace gloox
{

  namespace util
  {

    class Thread::ThreadImpl
    {
      public:
        ThreadImpl() {}
        ~ThreadImpl() {}

        void start( Thread* thread );
        void join();

      private:
#if defined( _WIN32 ) && !defined( __SYMBIAN32__ )
        HANDLE m_self;
#elif defined( HAVE_PTHREAD )
        pthread_t m_self;
#endif

#if defined( _WIN32 ) && !defined( __SYMBIAN32__ )
      static DWORD WINAPI redirect( void* context );
#elif defined( HAVE_PTHREAD )
      static void* redirect( void* context );
#endif

    };

#if defined( _WIN32 ) && !defined( __SYMBIAN32__ )
    DWORD WINAPI Thread::ThreadImpl::redirect( void* context )
    {
      Thread* self = static_cast<Thread*>( context );
      self->run();
      return 0;
    }
#elif defined( HAVE_PTHREAD )
    void* Thread::ThreadImpl::redirect( void* context )
    {
      Thread* self = static_cast<Thread*>( context );
      self->run();
      return 0;
    }
#endif

    void Thread::ThreadImpl::start( Thread* thread )
    {
#if defined( _WIN32 ) && !defined( __SYMBIAN32__ )
      m_self = CreateThread( 0, 0, redirect, thread, 0, 0 );
#elif defined( HAVE_PTHREAD )
      pthread_create( &m_self, 0, redirect, thread );
#endif
    }

    void Thread::ThreadImpl::join()
    {
#if defined( _WIN32 ) && !defined( __SYMBIAN32__ )
      WaitForSingleObject( m_self, INFINITE );
#elif defined( HAVE_PTHREAD )
      pthread_join( m_self, 0 );
#endif
    }
// ---- ThreadImpl ----

// ---- Thread ----
    Thread::Thread()
      : m_impl( new ThreadImpl() )
    {
    }

    Thread::~Thread()
    {
      delete m_impl;
    }

    void Thread::start()
    {
      if( m_impl )
        m_impl->start( this );
    }

    void Thread::join()
    {
      if( m_impl )
        m_impl->join();
    }

  }

}
