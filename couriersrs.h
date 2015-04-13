/* ---------------------------------------------------------------------------
 *  couriersrs - Doing SRS forwarding with courier
 *  Copyright (C) 2007  Matthias Wimmer <m@tthias.eu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * ---------------------------------------------------------------------------
 */

#ifndef COURIERSRS_H
#define COURIERSRS_H

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stddef.h>
#include <time.h>

extern "C" {	// yes, this is bad ... it should be in srs2.h itself
#include <srs2.h>
}

#include <popt.h>

#ifndef N_
#   define N_(n) (n)
#endif

#endif // COURIERSRS_H
