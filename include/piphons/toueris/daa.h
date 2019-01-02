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
#ifndef PIPHONS_TOUERIS_DAA_H
#define PIPHONS_TOUERIS_DAA_H

#include <piphons/daa.h>

namespace Toueris {

  /**
   * @class DaaToueris
   * @brief DAA for Toueris project
   */
  class Daa : public Piphons::Daa {
    public:
      Daa (int ringPin, int offhookLed, int offhookSetPin,  int offhookResetPin);

    protected:
      class Private;
      Daa (Private &dd);

    private:
      PIMP_DECLARE_PRIVATE (Daa)
  };

}
/* ========================================================================== */
#endif /* PIPHONS_TOUERIS_DAA_H defined */
