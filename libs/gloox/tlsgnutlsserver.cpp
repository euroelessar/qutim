/*
  Copyright (c) 2005-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "tlsgnutlsserver.h"

#ifdef HAVE_GNUTLS

#include <errno.h>

namespace gloox
{

  GnuTLSServer::GnuTLSServer( TLSHandler* th )
    : GnuTLSBase( th ), m_dhBits( 1024 )
  {
  }

  GnuTLSServer::~GnuTLSServer()
  {
    gnutls_certificate_free_credentials( m_x509cred );
    gnutls_dh_params_deinit( m_dhParams );
  }

  void GnuTLSServer::cleanup()
  {
    GnuTLSBase::cleanup();
    init();
  }

  bool GnuTLSServer::init( const std::string& clientKey,
                           const std::string& clientCerts,
                           const StringList& cacerts )
  {
    const int protocolPriority[] = {
#ifdef GNUTLS_TLS1_2
       GNUTLS_TLS1_2,
#endif
      GNUTLS_TLS1_1, GNUTLS_TLS1, 0 };
    const int kxPriority[]       = { GNUTLS_KX_RSA, GNUTLS_KX_DHE_RSA, GNUTLS_KX_DHE_DSS, 0 };
    const int cipherPriority[]   = { GNUTLS_CIPHER_AES_256_CBC, GNUTLS_CIPHER_AES_128_CBC,
                                     GNUTLS_CIPHER_3DES_CBC, GNUTLS_CIPHER_ARCFOUR, 0 };
    const int compPriority[]     = { GNUTLS_COMP_ZLIB, GNUTLS_COMP_NULL, 0 };
    const int macPriority[]      = { GNUTLS_MAC_SHA, GNUTLS_MAC_MD5, 0 };

    if( m_initLib && gnutls_global_init() != 0 )
      return false;

    if( gnutls_certificate_allocate_credentials( &m_x509cred ) < 0 )
      return false;

    setClientCert( clientKey, clientCerts );
    setCACerts( cacerts );

    generateDH();
    gnutls_certificate_set_dh_params( m_x509cred, m_dhParams );
    gnutls_certificate_set_rsa_export_params( m_x509cred, m_rsaParams);
//     gnutls_priority_init( &m_priorityCache, "NORMAL", 0 );

    if( gnutls_init( m_session, GNUTLS_SERVER ) != 0 )
      return false;

//     gnutls_priority_set( m_session, m_priorityCache );
    gnutls_protocol_set_priority( *m_session, protocolPriority );
    gnutls_cipher_set_priority( *m_session, cipherPriority );
    gnutls_compression_set_priority( *m_session, compPriority );
    gnutls_kx_set_priority( *m_session, kxPriority );
    gnutls_mac_set_priority( *m_session, macPriority );
    gnutls_credentials_set( *m_session, GNUTLS_CRD_CERTIFICATE, m_x509cred );

    gnutls_certificate_server_set_request( *m_session, GNUTLS_CERT_REQUEST );

    gnutls_dh_set_prime_bits( *m_session, m_dhBits );

    gnutls_transport_set_ptr( *m_session, (gnutls_transport_ptr_t)this );
    gnutls_transport_set_push_function( *m_session, pushFunc );
    gnutls_transport_set_pull_function( *m_session, pullFunc );

    m_valid = true;
    return true;
  }

  void GnuTLSServer::setCACerts( const StringList& cacerts )
  {
    m_cacerts = cacerts;

    StringList::const_iterator it = m_cacerts.begin();
    for( ; it != m_cacerts.end(); ++it )
      gnutls_certificate_set_x509_trust_file( m_x509cred, (*it).c_str(), GNUTLS_X509_FMT_PEM );
  }

  void GnuTLSServer::setClientCert( const std::string& clientKey, const std::string& clientCerts )
  {
    m_clientKey = clientKey;
    m_clientCerts = clientCerts;

    if( !m_clientKey.empty() && !m_clientCerts.empty() )
    {
      gnutls_certificate_set_x509_key_file( m_x509cred,
                                            m_clientCerts.c_str(),
                                            m_clientKey.c_str(),
                                            GNUTLS_X509_FMT_PEM );
    }
  }


  void GnuTLSServer::generateDH()
  {
    gnutls_dh_params_init( &m_dhParams );
    gnutls_dh_params_generate2( m_dhParams, m_dhBits );
    gnutls_rsa_params_init( &m_rsaParams );
    gnutls_rsa_params_generate2( m_rsaParams, 512 );
  }

  void GnuTLSServer::getCertInfo()
  {
    m_certInfo.status = CertOk;

    const char* info;
    info = gnutls_compression_get_name( gnutls_compression_get( *m_session ) );
    if( info )
      m_certInfo.compression = info;

    info = gnutls_mac_get_name( gnutls_mac_get( *m_session ) );
    if( info )
      m_certInfo.mac = info;

    info = gnutls_cipher_get_name( gnutls_cipher_get( *m_session ) );
    if( info )
      m_certInfo.cipher = info;

    info = gnutls_protocol_get_name( gnutls_protocol_get_version( *m_session ) );
    if( info )
      m_certInfo.protocol = info;

    m_valid = true;
  }

}

#endif // HAVE_GNUTLS
