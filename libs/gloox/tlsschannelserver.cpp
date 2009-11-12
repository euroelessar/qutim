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

#include "tlsschannelserver.h"

#ifdef HAVE_WINTLS

// #include <stdio.h> // just for debugging output
// #include <iostream>

namespace gloox
{
  SChannelServer::SChannelServer( TLSHandler* th )
    : SChannelBase( th, EmptyString )
  {
    //printf(">> SChannelServer::SChannelServer()\n");
  }

  SChannelServer::~SChannelServer()
  {
    m_handler = 0;
    cleanup();
    //printf(">> SChannelServer::~SChannelServer()\n");
  }

  bool SChannelServer::handshake()
  {
    if( !m_handler || !m_valid )
      return false;

    if( m_haveCredentialsHandle )
    {
      handshakeStage();
      return true;
    }

//     printf(">> SChannelServer::handshake()\n");
    SECURITY_STATUS error;
    ULONG return_flags;
    TimeStamp t;
    SecBuffer ibuf[2], obuf[1];
    SecBufferDesc ibufs, obufs;
    ULONG request = ISC_REQ_ALLOCATE_MEMORY
                    | ISC_REQ_CONFIDENTIALITY
                    | ISC_REQ_EXTENDED_ERROR
                    | ISC_REQ_INTEGRITY
                    | ISC_REQ_REPLAY_DETECT
                    | ISC_REQ_SEQUENCE_DETECT
                    | ISC_REQ_STREAM;


    /* acquire credentials */
    error = AcquireCredentialsHandle( 0,
                                      UNISP_NAME,
                                      SECPKG_CRED_INBOUND,
                                      0,
                                      &m_tlsCred,
                                      0,
                                      0,
                                      &m_credHandle,
                                      &t );
//     print_error(error, "SChannelServer::handshake() ~ AcquireCredentialsHandle()");
    if( error != SEC_E_OK )
    {
      cleanup();
      m_handler->handleHandshakeResult( this, false, m_certInfo );
      return false;
    }
    else
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

//       std::cout << "obuf[0].cbBuffer: " << obuf[0].cbBuffer << "\t" << obuf[0].BufferType << "\n";
//       std::cout << "ibuf[0].cbBuffer: " << ibuf[0].cbBuffer << "\t" << ibuf[0].BufferType << "\n";
//       std::cout << "ibuf[1].cbBuffer: " << ibuf[1].cbBuffer << "\t" << ibuf[1].BufferType << "\n";

      error = AcceptSecurityContext( &m_credHandle,
                                         0,
                                         &ibufs,
                                         request,
                                         0,
                                         &m_context,
                                         &obufs,
                                         &return_flags,
                                         &t );
//       print_error(error, "SChannelServer::handshake() ~ AcceptSecurityContext()");

      if( error == SEC_I_CONTINUE_NEEDED )
      {
//        std::cout << "obuf[0].cbBuffer: " << obuf[0].cbBuffer << "\t" << obuf[0].BufferType << "\n";
//         std::cout << "ibuf[0].cbBuffer: " << ibuf[0].cbBuffer << "\t" << ibuf[0].BufferType << "\n";
//         std::cout << "ibuf[1].cbBuffer: " << ibuf[1].cbBuffer << "\t" << ibuf[1].BufferType << "\n";
        m_cleanedup = false;
        m_buffer = EmptyString;
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

  void SChannelServer::handshakeStage()
  {
//     printf(" >> SChannelServer::handshakeStage\n");

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


//        std::cout << "obuf[0].cbBuffer: " << obuf[0].cbBuffer << "\t" << obuf[0].BufferType << "\n";
//        std::cout << "ibuf[0].cbBuffer: " << ibuf[0].cbBuffer << "\t" << ibuf[0].BufferType << "\n";
//        std::cout << "ibuf[1].cbBuffer: " << ibuf[1].cbBuffer << "\t" << ibuf[1].BufferType << "\n";


      /* negotiate security */
      error = AcceptSecurityContext( &m_credHandle,
                                         &m_context,
                                         &ibufs,
                                         request,
                                         0,
                                         0,
                                         &obufs,
                                         &a,
                                         &t );
//       print_error(error, "SChannelServer::handshakeStage() ~ AcceptSecurityContext()");

//       std::cout << "obuf[0].cbBuffer: " << obuf[0].cbBuffer << "\t" << obuf[0].BufferType << "\n";
//       std::cout << "ibuf[0].cbBuffer: " << ibuf[0].cbBuffer << "\t" << ibuf[0].BufferType << "\n";
//       std::cout << "ibuf[1].cbBuffer: " << ibuf[1].cbBuffer << "\t" << ibuf[1].BufferType << "\n";

      if( error == SEC_E_OK )
      {
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
          m_buffer = EmptyString;

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
//         print_error(error, "SChannelClient::handshakeStage() ~ InitializeSecurityContext()");
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

  void SChannelServer::privateCleanup()
  {
    if( m_cert )
      CertFreeCertificateContext( m_cert );
    if( m_store )
      CertCloseStore( m_store, 0 );
  }

}

#endif // HAVE_WINTLS
