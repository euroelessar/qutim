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

#include "tlsschannelclient.h"

#ifdef HAVE_WINTLS

// #include <stdio.h> // just for debugging output

namespace gloox
{
  SChannelClient::SChannelClient( TLSHandler* th, const std::string& server )
    : SChannelBase( th, server )
  {
    //printf(">> SChannelClient::SChannelClient()\n");
  }

  SChannelClient::~SChannelClient()
  {
    m_handler = 0;
    cleanup();
    //printf(">> SChannelClient::~SChannelClient()\n");
  }

  bool SChannelClient::handshake()
  {
    if( !m_handler )
      return false;

    if( m_haveCredentialsHandle )
    {
      handshakeStage();
      return true;
    }

//     printf(">> SChannelClient::handshake()\n");
    SECURITY_STATUS error;
    ULONG return_flags;
    TimeStamp t;
    SecBuffer obuf[1];
    SecBufferDesc obufs;
    SCHANNEL_CRED tlscred;
    ULONG request = ISC_REQ_ALLOCATE_MEMORY
                    | ISC_REQ_CONFIDENTIALITY
                    | ISC_REQ_EXTENDED_ERROR
                    | ISC_REQ_INTEGRITY
                    | ISC_REQ_REPLAY_DETECT
                    | ISC_REQ_SEQUENCE_DETECT
                    | ISC_REQ_STREAM
                    | ISC_REQ_MANUAL_CRED_VALIDATION;

    /* initialize TLS credential */
    memset( &tlscred, 0, sizeof( SCHANNEL_CRED ) );
    tlscred.dwVersion = SCHANNEL_CRED_VERSION;
    tlscred.grbitEnabledProtocols = SP_PROT_TLS1_CLIENT;
    /* acquire credentials */
    error = AcquireCredentialsHandle( 0,
                                      UNISP_NAME,
                                      SECPKG_CRED_OUTBOUND,
                                      0,
                                      &tlscred,
                                      0,
                                      0,
                                      &m_credHandle,
                                      &t );
//     print_error(error, "SChannelClient::handshake() ~ AcquireCredentialsHandle()");
    if( error != SEC_E_OK )
    {
      cleanup();
      m_handler->handleHandshakeResult( this, false, m_certInfo );
      return false;
    }
    else
    {
      /* initialize buffers */
      obuf[0].cbBuffer = 0;
      obuf[0].pvBuffer = 0;
      obuf[0].BufferType = SECBUFFER_TOKEN;
      /* initialize buffer descriptors */
      obufs.ulVersion = SECBUFFER_VERSION;
      obufs.cBuffers = 1;
      obufs.pBuffers = obuf;
      /* negotiate security */
      SEC_CHAR* hname = const_cast<char*>( m_server.c_str() );

      error = InitializeSecurityContextA( &m_credHandle,
                                         0,
                                         hname,
                                         request,
                                         0,
                                         0,
                                         0,
                                         0,
                                         &m_context,
                                         &obufs,
                                         &return_flags,
                                         0 );
//       print_error(error, "SChannelClient::handshake() ~ InitializeSecurityContext()");

      if( error == SEC_I_CONTINUE_NEEDED )
      {
        m_cleanedup = false;
        //std::cout << "obuf[1].cbBuffer: " << obuf[0].cbBuffer << "\n";
        std::string senddata( static_cast<char*>( obuf[0].pvBuffer ), obuf[0].cbBuffer );
        FreeContextBuffer( obuf[0].pvBuffer );
        m_haveCredentialsHandle = true;
        m_handler->handleEncryptedData( this, senddata );
        return true;
      }
      else
      {
        cleanup();
        m_handler->handleHandshakeResult( this, false, m_certInfo );
        return false;
      }
    }
  }

  void SChannelClient::handshakeStage()
  {
//     printf(" >> SChannelClient::handshakeStage\n");

    SECURITY_STATUS error;
    ULONG a;
    TimeStamp t;
    SecBuffer ibuf[2], obuf[1];
    SecBufferDesc ibufs, obufs;
    ULONG request = ISC_REQ_ALLOCATE_MEMORY
                    | ISC_REQ_CONFIDENTIALITY
                    | ISC_REQ_EXTENDED_ERROR
                    | ISC_REQ_INTEGRITY
                    | ISC_REQ_REPLAY_DETECT
                    | ISC_REQ_SEQUENCE_DETECT
                    | ISC_REQ_STREAM
                    | ISC_REQ_MANUAL_CRED_VALIDATION;

    SEC_CHAR* hname = const_cast<char*>( m_server.c_str() );

    do
    {
      /* initialize buffers */
      ibuf[0].cbBuffer = static_cast<unsigned long>( m_buffer.size() );
      ibuf[0].pvBuffer = static_cast<void*>( const_cast<char*>( m_buffer.c_str() ) );
      //std::cout << "Size: " << m_buffer.size() << "\n";
      ibuf[1].cbBuffer = 0;
      ibuf[1].pvBuffer = 0;
      obuf[0].cbBuffer = 0;
      obuf[0].pvBuffer = 0;

      ibuf[0].BufferType = SECBUFFER_TOKEN;
      ibuf[1].BufferType = SECBUFFER_EMPTY;
      obuf[0].BufferType = SECBUFFER_EMPTY;
      /* initialize buffer descriptors */
      ibufs.ulVersion = obufs.ulVersion = SECBUFFER_VERSION;
      ibufs.cBuffers = 2;
      obufs.cBuffers = 1;
      ibufs.pBuffers = ibuf;
      obufs.pBuffers = obuf;

      /*
       * std::cout << "obuf[0].cbBuffer: " << obuf[0].cbBuffer << "\t" << obuf[0].BufferType << "\n";
       * std::cout << "ibuf[0].cbBuffer: " << ibuf[0].cbBuffer << "\t" << ibuf[0].BufferType << "\n";
       * std::cout << "ibuf[1].cbBuffer: " << ibuf[1].cbBuffer << "\t" << ibuf[1].BufferType << "\n";
       */

      /* negotiate security */
      error = InitializeSecurityContextA( &m_credHandle,
                                         &m_context,
                                         hname,
                                         request,
                                         0,
                                         0,
                                         &ibufs,
                                         0,
                                         0,
                                         &obufs,
                                         &a,
                                         &t );
//       print_error(error, "SChannelClient::handshakeStage() ~ InitializeSecurityContext()");
      if( error == SEC_E_OK )
      {
        // EXTRA STUFF??
        if( ibuf[1].BufferType == SECBUFFER_EXTRA )
        {
          m_buffer.erase( 0, m_buffer.size() - ibuf[1].cbBuffer );
        }
        else
        {
          m_buffer = EmptyString;
        }
        setSizes();
        setCertinfos();

        m_secure = true;
        m_handler->handleHandshakeResult( this, true, m_certInfo );
        break;
      }
      else if( error == SEC_I_CONTINUE_NEEDED )
      {
        /*
         * std::cout << "obuf[0].cbBuffer: " << obuf[0].cbBuffer << "\t" << obuf[0].BufferType << "\n";
         * std::cout << "ibuf[0].cbBuffer: " << ibuf[0].cbBuffer << "\t" << ibuf[0].BufferType << "\n";
         * std::cout << "ibuf[1].cbBuffer: " << ibuf[1].cbBuffer << "\t" << ibuf[1].BufferType << "\n";
         */

        // STUFF TO SEND??
        if( obuf[0].cbBuffer != 0 && obuf[0].pvBuffer != 0 )
        {
          std::string senddata( static_cast<char*>(obuf[0].pvBuffer), obuf[0].cbBuffer );
          FreeContextBuffer( obuf[0].pvBuffer );
          m_handler->handleEncryptedData( this, senddata );
        }
        // EXTRA STUFF??
        if( ibuf[1].BufferType == SECBUFFER_EXTRA )
        {
          m_buffer.erase( 0, m_buffer.size() - ibuf[1].cbBuffer );
          // Call again if we aren't sending anything (otherwise the server will not send anything back
          // and this function won't get called again to finish the processing).  This is needed for
          // NT4.0 which does not seem to process the entire buffer the first time around
          if( obuf[0].cbBuffer == 0 )
            handshakeStage();
        }
        else
        {
          m_buffer = EmptyString;
        }
        return;
      }
      else if( error == SEC_I_INCOMPLETE_CREDENTIALS )
      {
        handshakeStage();
      }
      else if( error == SEC_E_INCOMPLETE_MESSAGE )
      {
        break;
      }
      else
      {
        cleanup();
        m_handler->handleHandshakeResult( this, false, m_certInfo );
        break;
      }
    }
    while( true );
  }

}

#endif // HAVE_WINTLS
