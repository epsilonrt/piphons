/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the libdaadtmf Library.
 *
 * The libdaadtmf Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libdaadtmf Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libdaadtmf Library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DAA_TOUERIS_H
#define DAA_TOUERIS_H

#include "daadtmf.h"

/**
 * @class DaaToueris
 * @brief DAA for Toueris project
 */
class DaaToueris : public Daa {
  public:
    DaaToueris (int ringPin, int offhookLed, int offhookSetPin,  int offhookResetPin);

  protected:
    class Private;
    DaaToueris (Private &dd);

  private:
    PIMP_DECLARE_PRIVATE (DaaToueris)
};

/* ========================================================================== */
#endif /* DAA_TOUERIS_H defined */
