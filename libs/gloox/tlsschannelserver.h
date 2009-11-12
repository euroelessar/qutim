/*
 * Copyright (c) 2007-2009 by Jakob Schroeter <js@camaya.net>
 * This file is part of the gloox library. http://camaya.net/gloox
 *
 * This software is distributed under a license. The full license
 * agreement can be found in the file LICENSE in this distribution.
 * This software may not be copied, modified, sold or distributed
 * other than expressed in the named license agreement.
 *
 * This software is distributed without any warranty.
 */

#ifndef TLSSCHANNELSERVER_H__
#define TLSSCHANNELSERVER_H__

#include "tlsschannelbase.h"

#include "config.h"

#ifdef HAVE_WINTLS

#include <ctime>

#define SECURITY_WIN32
#include <windows.h>
#include <security.h>
#include <schnlsp.h>
#include <wincrypt.h>

namespace gloox
{

  /**
   * This class implements a server-side TLS backend using SChannel.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.1
   */
  class SChannelServer : public SChannelBase
  {
    public:
      /**
       * Constructor.
       * @param th The TLSHandler to handle TLS-related events.
       */
      SChannelServer( TLSHandler* th  );

      /**
       * Virtual destructor.
       */
      virtual ~SChannelServer();

      // reimplemented from TLSBase
      virtual bool handshake();

    private:
      void handshakeStage();
      virtual void privateCleanup();

  };
}

#endif // HAVE_WINTLS

#endif // TLSSCHANNELSERVER_H__
