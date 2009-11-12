/*
 * Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
 * This file is part of the gloox library. http://camaya.net/gloox
 *
 * This software is distributed under a license. The full license
 * agreement can be found in the file LICENSE in this distribution.
 * This software may not be copied, modified, sold or distributed
 * other than expressed in the named license agreement.
 *
 * This software is distributed without any warranty.
 */

#ifndef CONNECTIONCOMPRESSION_H__
#define CONNECTIONCOMPRESSION_H__

#include "gloox.h"
#include "logsink.h"
#include "connectionbase.h"
#include "connectiondatahandler.h"
#include "compressiondatahandler.h"

#include <string>

namespace gloox
{

  class CompressionDefault;

  /**
   * @brief This is an implementation of a de/compressing connection.
   *
   * There should be no need to use this class directly. It is
   * used by ClientBase internally.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.1
   */

  class GLOOX_API ConnectionCompression : public CompressionDataHandler, public ConnectionBase, ConnectionDataHandler
  {
    public:
      /**
       * Constructs a new ConnectionCompression object.
       * @param cdh The ConnectionDataHandler that will be notified of events from this connection
       * @param conn A transport connection. It should be configured to connect to
       * the server and port you wish to make the encrypted connection to.
       * ConnectionCompression will own the transport connection and delete it in its destructor.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       */
      ConnectionCompression( ConnectionDataHandler* cdh, ConnectionBase* conn, const LogSink& log );

      /**
       * Constructs a new ConnectionCompression object.
       * @param conn A transport connection. It should be configured to connect to
       * the server and port you wish to make the encrypted connection to.
       * ConnectionCompression will own the transport connection and delete it in its destructor.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       */
      ConnectionCompression( ConnectionBase* conn, const LogSink& log );

      /**
       * Virtual Destructor.
       */
      virtual ~ConnectionCompression();

      /**
       * Sets the transport connection.
       * @param connection The transport connection to use.
       */
      void setConnectionImpl( ConnectionBase* connection );

      // reimplemented from ConnectionBase
      virtual ConnectionError connect();

      // reimplemented from ConnectionBase
      virtual ConnectionError recv( int timeout = -1 );

      // reimplemented from ConnectionBase
      virtual bool send( const std::string& data );

      // reimplemented from ConnectionBase
      virtual ConnectionError receive();

      // reimplemented from ConnectionBase
      virtual void disconnect();

      // reimplemented from ConnectionBase
      virtual void cleanup();

      // reimplemented from ConnectionBase
      virtual void getStatistics( long int& totalIn, long int& totalOut );

      // reimplemented from ConnectionDataHandler
      virtual void handleReceivedData( const ConnectionBase* connection, const std::string& data );

      // reimplemented from ConnectionDataHandler
      virtual void handleConnect( const ConnectionBase* connection );

      // reimplemented from ConnectionDataHandler
      virtual void handleDisconnect( const ConnectionBase* connection, ConnectionError reason );

      // reimplemented from ConnectionDataHandler
      virtual ConnectionBase* newInstance() const;

      // reimplemented from CompressionDataHandler
      virtual void handleCompressedData( const std::string& data );

      // reimplemented from CompressionDataHandler
      virtual void handleDecompressedData( const std::string& data );


    private:
      ConnectionCompression& operator=( const ConnectionCompression& );
      ConnectionBase* m_connection;
      CompressionDefault* m_compression;
      const LogSink & m_log;

  };

}

#endif // CONNECTIONCOMPRESSION_H__
