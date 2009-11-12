/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef THREAD_H__
#define THREAD_H__

#include "config.h"

#include "macros.h"

namespace gloox
{

  namespace util
  {

    /**
     * @brief A simple wrapper around the platform's native thread implementation.
     *
     * @author JakobSchroeter <js@camaya.net>
     * @since 1.1
     */
    class GLOOX_API Thread
    {
      public:
        /**
         * Constructs a new wrapper.
         */
        Thread();

        /**
         * Virtual destructor.
         */
        virtual ~Thread();

        /**
         * Starts the thread.
         */
        void start();

        /**
         * Joins the thread with the calling thread, i.e. waits for the thread's termination.
         */
        void join();

      protected:
        /**
         * Reimplement this function to customize the thread. The thread will exit when
         * this function exits.
         */
        virtual void run() = 0;

      private:
        class ThreadImpl;
        friend class ThreadImpl;

        Thread& operator=( const Thread& );

        ThreadImpl* m_impl;

    };

  }

}

#endif // THREAD_H__
