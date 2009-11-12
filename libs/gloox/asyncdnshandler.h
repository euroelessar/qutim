/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef ASYNCDNSHANDLER_H__
#define ASYNCDNSHANDLER_H__

#include "dns.h"

namespace gloox
{

  /**
   * @brief A handler that will receive results of the asynchronous DNS resolver.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.1
   */
  class GLOOX_API AsyncDNSHandler
  {
    public:
      /**
       *
       * @param hosts A list of weighted hostname/port pairs from SRV records, or A records if
       * no SRV records where found.
       * @param context User-defined context information.
       */
      virtual void handleAsyncResolveResult( const DNS::HostMap& hosts, void* context ) = 0;

      /**
       *
       * @param fd A file descriptor for the established connection.
       * @param context User-defined context information.
       */
      virtual void handleAsyncConnectResult( int fd, void* context ) = 0;

  };

}

#endif // ASYNCDNSHANDLER_H__
