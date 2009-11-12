/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "asyncdns.h"
#include "asyncdnshandler.h"
#include "dns.h"
#include "mutexguard.h"
#include "thread.h"
#include "util.h"

namespace  gloox
{

#if ( defined( _WIN32 ) && !defined( __SYMBIAN32__ ) ) || defined( HAVE_PTHREAD )

  class AsyncDNSWorker : public util::Thread
  {
    public:
      /**
       * This function asynchronously resolves a service/protocol/domain tuple.
       * @param adh The result handler.
       * @param service The SRV service type.
       * @param proto The SRV protocol.
       * @param domain The domain to search for SRV records.
       * @param logInstance A LogSink to use for logging.
       */
      AsyncDNSWorker( AsyncDNSHandler* adh, const std::string& service, const std::string& proto,
                      const std::string& domain, const LogSink& logInstance );

      /**
       * This is a convenience function which uses @ref resolve() to get a list of hosts
       * and connects to one of them.
       * @param adh The result handler.
       * @param host The host to resolve SRV records for.
       * @param logInstance A LogSink to use for logging.
       */
      AsyncDNSWorker( AsyncDNSHandler* adh, const std::string& host,
                      const LogSink& logInstance );

      /**
       * This is a convenience function which connects to the given host and port. No SRV
       * records will be resolved. Use this function for special setups.
       * @param adh The result handler.
       * @param host The host/IP address to connect to.
       * @param port A custom port to connect to.
       * @param logInstance A LogSink to use for logging.
       */
      AsyncDNSWorker( AsyncDNSHandler* adh, const std::string& host, int port,
                      const LogSink& logInstance );

      /**
       * Virtual destructor.
       */
      virtual ~AsyncDNSWorker();

    protected:
      // reimplemented from Thread
      virtual void run();

    private:
      enum JobType
      {
        Resolve,
        ConnectResolve,
        Connect
      };

      void* m_context;
      AsyncDNSHandler* m_adh;
      std::string m_service;
      std::string m_proto;
      std::string m_domain;
      std::string m_host;
      int m_port;
      const LogSink& m_logInstance;

      JobType m_type;

  };

  AsyncDNSWorker::AsyncDNSWorker( AsyncDNSHandler* adh, const std::string& service,
                                  const std::string& proto, const std::string& domain,
                                  const LogSink& logInstance )
    : m_adh( adh ), m_service( service ), m_proto( proto ), m_domain( domain ), m_port( -1 ),
      m_logInstance( logInstance ), m_type( Resolve )
  {
  }

  AsyncDNSWorker::AsyncDNSWorker( AsyncDNSHandler* adh, const std::string& host,
                                  const LogSink& logInstance )
    : m_adh( adh ), m_host( host ), m_port( -1 ), m_logInstance( logInstance ),
      m_type( ConnectResolve )
  {
  }

  AsyncDNSWorker::AsyncDNSWorker( AsyncDNSHandler* adh, const std::string& host, int port,
                                  const LogSink& logInstance )
    : m_adh( adh ), m_host( host ), m_port( port ), m_logInstance( logInstance ),
      m_type( Connect )
  {
  }

  AsyncDNSWorker::~AsyncDNSWorker()
  {
  }

  void AsyncDNSWorker::run()
  {
    switch( m_type )
    {
      case Resolve:
      {
        const DNS::HostMap& hm = DNS::resolve( m_service, m_proto, m_domain, m_logInstance );
        m_adh->handleAsyncResolveResult( hm, this );
        break;
      }
      case ConnectResolve:
      {
        int fd = DNS::connect( m_host, m_logInstance );
        m_adh->handleAsyncConnectResult( fd, this );
        break;
      }
      case Connect:
      {
        int fd = DNS::connect( m_host, m_port, m_logInstance );
        m_adh->handleAsyncConnectResult( fd, this );
        break;
      }
    }
  }
  // ---- ~AsyncDNSWorker ----

  // ---- AsyncDNS ----
  AsyncDNS::AsyncDNS()
  {
  }

  AsyncDNS::~AsyncDNS()
  {
    WorkerMap::iterator it = m_workers.begin();
    for( ; it != m_workers.end(); ++it )
    {
      delete (*it).first;
    }

    util::clearList( m_obsoleteWorkers );
  }

  void AsyncDNS::resolve( AsyncDNSHandler* adh, const std::string& service, const std::string& proto,
                          const std::string& domain, const LogSink& logInstance, void* context )
  {
    m_obsoleteWorkerMutex.lock();
    util::clearList( m_obsoleteWorkers );
    m_obsoleteWorkerMutex.unlock();

    AsyncDNSWorker* adw = new AsyncDNSWorker( this, service, proto, domain, logInstance );
    AsyncContext ac( adh, context );
    m_workerMutex.lock();
    m_workers.insert( std::make_pair( adw, ac ) );
    m_workerMutex.unlock();
    adw->start();
  }

  void AsyncDNS::connect( AsyncDNSHandler* adh, const std::string& host,
                           const LogSink& logInstance, void* context )
  {
    m_obsoleteWorkerMutex.lock();
    util::clearList( m_obsoleteWorkers );
    m_obsoleteWorkerMutex.unlock();

    AsyncDNSWorker* adw = new AsyncDNSWorker( this, host, logInstance );
    AsyncContext ac( adh, context );
    m_workerMutex.lock();
    m_workers.insert( std::make_pair( adw, ac ) );
    m_workerMutex.unlock();
    adw->start();
  }

  void AsyncDNS::connect( AsyncDNSHandler* adh, const std::string& host, int port,
                           const LogSink& logInstance, void* context )
  {
    m_obsoleteWorkerMutex.lock();
    util::clearList( m_obsoleteWorkers );
    m_obsoleteWorkerMutex.unlock();

    AsyncDNSWorker* adw = new AsyncDNSWorker( this, host, port, logInstance );
    AsyncContext ac( adh, context );
    m_workerMutex.lock();
    m_workers.insert( std::make_pair( adw, ac ) );
    m_workerMutex.unlock();
    adw->start();
  }

  void AsyncDNS::handleAsyncResolveResult( const DNS::HostMap& hosts, void* context )
  {
    AsyncDNSWorker* adw = static_cast<AsyncDNSWorker*>( context );

    util::MutexGuard mg( m_workerMutex );
    WorkerMap::iterator it = m_workers.begin();
    for( ; it != m_workers.end(); ++it )
    {
      if( (*it).first == adw )
      {
        AsyncDNSHandler* adh = (*it).second.handler;
        void* ctx = (*it).second.context;
        m_obsoleteWorkerMutex.lock();
        m_obsoleteWorkers.push_back( (*it).first );
        m_obsoleteWorkerMutex.unlock();
        m_workers.erase( it );
        adh->handleAsyncResolveResult( hosts, ctx );
        return;
      }
    }
  }

  void AsyncDNS::handleAsyncConnectResult( int fd, void* context )
  {
    AsyncDNSWorker* adw = static_cast<AsyncDNSWorker*>( context );

    util::MutexGuard mg( m_workerMutex );
    WorkerMap::iterator it = m_workers.begin();
    for( ; it != m_workers.end(); ++it )
    {
      if( (*it).first == adw )
      {
        AsyncDNSHandler* adh = (*it).second.handler;
        void* ctx = (*it).second.context;
        m_obsoleteWorkerMutex.lock();
        m_obsoleteWorkers.push_back( (*it).first );
        m_obsoleteWorkerMutex.unlock();
        m_workers.erase( it );
        adh->handleAsyncConnectResult( fd, ctx );
        return;
      }
    }
  }

#else

#endif // WANT_ASYNC_DNS

}
