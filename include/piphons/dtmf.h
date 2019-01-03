/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libpiphons Library.
 *
 * The libpiphons Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libpiphons Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libpiphons Library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PIPHONS_DTMF_H
#define PIPHONS_DTMF_H

#include <array>
#include <piphons/global.h>

namespace Piphons {

  /**
   * @class Dtmf decoder
   * @brief DTMF Ic compatible with the MT8870 (HT9170 ...)
   * 
   * The decoder is connected to the board Pi by a DV pin which indicates that 
   * a DTMF code has been detected and 4 data pins D0 to D3 corresponding to 
   * the code of the key pressed.
   * 
   * http://www.holtek.com/documents/10179/116711/9170v111.pdf
   */
  class Dtmf {
  public:
      /**
       * @brief type of key management function
       */
      typedef void (*DtmfHandler) (Dtmf * ctrl);


      Dtmf (int d0Pin, int d1Pin, int d2Pin, int d3Pin, int dvPin);
      Dtmf (const std::array<int,5> & dPin);
      Dtmf (const Dtmf & other);
      void swap (Dtmf &other);
      Dtmf& operator= (const Dtmf &other);
      ~Dtmf();

      void setKeyHandler (DtmfHandler handler);
      std::size_t available() const;
      int read();

      bool open();
      bool isOpen() const;
      void close();

    protected:
      class Private;
      Dtmf (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Dtmf)
  };

}
/* ========================================================================== */
#endif /* PIPHONS_DTMF_H defined */
