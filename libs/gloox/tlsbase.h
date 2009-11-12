/*
  Copyright (c) 2007-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef TLSBASE_H__
#define TLSBASE_H__

#include "gloox.h"
#include "mutex.h"
#include "tlshandler.h"

namespace gloox
{

  /**
   * @brief An abstract base class for TLS implementations.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API TLSBase
  {
    public:
      /**
       * Constructor.
       * @param th The TLSHandler to handle TLS-related events.
       * @param server The server to use in certificate verification.
       */
      TLSBase( TLSHandler* th, const std::string server )
        : m_handler( th ), m_server( server ), m_secure( false ), m_valid( false ), m_initLib( true )
      {}

      /**
       * Virtual destructor.
       */
      virtual ~TLSBase() {}

      /**
       * Initializes the TLS module. This function must be called (and execute successfully)
       * before the module can be used.
       * @param clientKey The absolute path to the user's private key in PEM format.
       * @param clientCerts A path to a certificate bundle in PEM format.
       * @param cacerts A list of absolute paths to CA root certificate files in PEM format.
       * @return @b False if initialization failed, @b true otherwise.
       * @note The arguments are not used in the SChannel (native on Windows) implementation,
       * init() needs to be called nonetheless. Use setSubject() to set client/server
       * key/certificate.
       * @since 1.0
       */
      virtual bool init( const std::string& clientKey = EmptyString,
                         const std::string& clientCerts = EmptyString,
                         const StringList& cacerts = StringList() ) = 0;

      /**
       * Enables/disables initialization of the underlying TLS library. By default,
       * initialization is performed. You may want to switch it off if the TLS library
       * is used elsewhere in your applicationas well and you have no control over the
       * initialization.
       * @param init Whether or not to intialize the underlying TLS library.
       */
      void setInitLib( bool init ) { m_initLib = init; }

      /**
       * Sets the subject/common name to search the system certificate store for. Used only by the
       * SChannel implementation (Windows). Required for SChannel server, optional for SChannel client.
       * The system 'MY' certificate store will be searched for the @b subject (substring match) for a
       * private key/certificate pair which will be used.
       * @note On the server-side, initialization
       * will fail if @b subject is not set or if no associated key/certificate could be found.
       * On the client-side initialization will fail if a @b subject was set for which no
       * key/certificate pair could be found. Setting no @b subject on the client-side is fine.
       * @note setSubject() must be called before init() to be effective.
       * @param subject The to use.
       */
      virtual void setSubject( const std::string& subject ) { m_subject = subject; }

      /**
       * Use this function to feed unencrypted data to the encryption implementation.
       * The encrypted result will be pushed to the TLSHandler's handleEncryptedData() function.
       * @param data The data to encrypt.
       * @return Whether or not the data was used successfully.
       */
      virtual bool encrypt( const std::string& data ) = 0;

      /**
       * Use this function to feed encrypted data or received handshake data to the
       * encryption implementation. Handshake data will be eaten, unencrypted data
       * will be pushed to the TLSHandler's handleDecryptedData() function.
       * @param data The data to decrypt.
       * @return The number of bytes used from the input.
       */
      virtual int decrypt( const std::string& data ) = 0;

      /**
       * This function performs internal cleanup and will be called after a failed handshake attempt.
       */
      virtual void cleanup() = 0;

      /**
       * This functiopn performs the TLS handshake. Handshake data from the server side should be
       * fed in using decrypt(). Handshake data that is to be sent to the other side is pushed through
       * TLSBase's handleEncryptedData().
       * @return @b True if the handshake was successful or if more input is needed, @b false if the
       * handshake failed.
       */
      virtual bool handshake() = 0;

      /**
       * Returns the state of the encryption.
       * @return The state of the encryption.
       */
      virtual bool isSecure() const { return m_secure; }

      /**
       * This function is used to retrieve certificate and connection info of a encrypted connection.
       * @return Certificate information.
       */
      virtual const CertInfo& fetchTLSInfo() const { return m_certInfo; }

    protected:
      TLSHandler* m_handler;
      StringList m_cacerts;
      std::string m_clientKey;
      std::string m_clientCerts;
      std::string m_server;
      std::string m_subject;
      CertInfo m_certInfo;
      util::Mutex m_mutex;
      bool m_secure;
      bool m_valid;
      bool m_initLib;

  };

}

#endif // TLSBASE_H__
