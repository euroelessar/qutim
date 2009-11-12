/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLESESSIONHANDLER_H__
#define JINGLESESSIONHANDLER_H__

#include "macros.h"
#include "jinglesession.h"

namespace gloox
{

  namespace Jingle
  {

    /**
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class GLOOX_API SessionHandler
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~SessionHandler() {}

        /**
         *
         */
        virtual void handleSessionStateChange( const Session* session, const Session::Jingle* jingle ) = 0;

        /**
         *
         */
        virtual void handleSessionInfo( const Session* session, const Session::Jingle* jingle ) = 0;

        /**
         *
         */
        virtual void handleTransportInfo( const Session* session, const Session::Jingle* jingle ) = 0;

    };

  }

}

#endif // JINGLESESSIONHANDLER_H__
