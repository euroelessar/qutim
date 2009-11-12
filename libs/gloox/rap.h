/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef RAP_H__
#define RAP_H__


#include "stanzaextension.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief This is an implementation of XEP-0168 as a StanzaExtension.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class GLOOX_API RAP : public StanzaExtension
  {

    public:
      /**
       * Constructs a new object from the given Tag.
       * @param tag The Tag to parse.
       */
      RAP( const Tag* tag = 0 );

      /**
       * Constructs a new object with the given namespace and priority.
       * @param ns The application namespace.
       * @param num The priority for the application namespace.
       */
      RAP( const std::string& ns, int num );

      /**
       * Virtual Destructor.
       */
      virtual ~RAP();

      /**
       * Indicates whether this is the primary resource for the given namespace.
       * @return @b True if the server flagged this resource as primary for the given namespace, @b false otherwise.
       */
      bool primary() const { return m_primary; }

      // reimplemented from StanzaExtension
      virtual const std::string& filterString() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new RAP( tag );
      }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* clone() const
      {
        return new RAP();
      }

    private:
      std::string m_ns;
      int m_num;
      bool m_primary;

  };

}

#endif // RAP_H__
