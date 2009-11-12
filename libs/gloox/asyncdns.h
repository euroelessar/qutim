/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef ASYNCDNS_H__
#define ASYNCDNS_H__

#include "asyncdnshandler.h"
#include "config.h"
#include "macros.h"
#include "mutex.h"

#include <list>
#include <map>
#include <string>

namespace gloox
{

  class AsyncDNSWorker;
  class LogSink;

  /**
   * @brief An asynchronous DNS resolver class.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.1
   */
  class GLOOX_API AsyncDNS : public AsyncDNSHandler
  {
    public:
      /**
       * Constructor.
       */
      AsyncDNS();

      /**
       * Destructor.
       */
      ~AsyncDNS();

      /**
       * This function asynchronously resolves a service/protocol/domain tuple.
       * @param adh The result handler.
       * @param service The SRV service type.
       * @param proto The SRV protocol.
       * @param domain The domain to search for SRV records.
       * @param logInstance A LogSink to use for logging.
       * @param context User-supplied context information.
       */
      void resolve( AsyncDNSHandler* adh, const std::string& service, const std::string& proto,
                    const std::string& domain, const LogSink& logInstance, void* context );

      /**
       * This is a convenience funtion which uses @ref resolve() to resolve SRV records asynchronously
       * for a given domain, using a service of @b xmpp-client and a proto of @b tcp.
       * @param adh The result handler.
       * @param domain The domain to resolve SRV records for.
       * @param logInstance A LogSink to use for logging.
       * @param context User-supplied context information.
       */
      void resolve( AsyncDNSHandler* adh, const std::string& domain,
                    const LogSink& logInstance, void* context )
        { resolve( adh, "xmpp-client", "tcp", domain, logInstance, context ); }

      /**
       * This is a convenience function which uses @ref resolve() to get a list of hosts
       * and connects to one of them.
       * @param adh The result handler.
       * @param host The host to resolve SRV records for.
       * @param logInstance A LogSink to use for logging.
       * @param context User-supplied context information.
       */
      void connect( AsyncDNSHandler* adh, const std::string& host,
                    const LogSink& logInstance, void* context );

      /**
       * This is a convenience function which connects to the given host and port. No SRV
       * records are resolved. Use this function for special setups.
       * @param adh The result handler.
       * @param host The host/IP address to connect to.
       * @param port A custom port to connect to.
       * @param logInstance A LogSink to use for logging.
       * @param context User-supplied context information.
       */
      void connect( AsyncDNSHandler* adh, const std::string& host, int port,
                    const LogSink& logInstance, void* context );

    protected:
      // reimplemented from AsyncDNSHandler
      virtual void handleAsyncResolveResult( const DNS::HostMap& hosts, void* context );

      // reimplemented from AsyncDNSHandler
      virtual void handleAsyncConnectResult( int fd, void* context );

    private:
      struct AsyncContext
      {
        AsyncContext( AsyncDNSHandler* adh, void* ctx ) : handler( adh ), context( ctx ) {}
        AsyncDNSHandler* handler;
        void* context;
      };

      typedef std::map<AsyncDNSWorker*, AsyncContext> WorkerMap;
      typedef std::list<AsyncDNSWorker*> WorkerList;
      WorkerMap m_workers;
      WorkerList m_obsoleteWorkers;

      util::Mutex m_workerMutex;
      util::Mutex m_obsoleteWorkerMutex;

  };

}

#endif // ASYNCDNS_H__
