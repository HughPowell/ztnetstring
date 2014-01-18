/*  =========================================================================
    synocracy.h - Synocracy library

    -------------------------------------------------------------------------
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of Synocracy, see http://www.synocracy.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTA-
    BILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see http://www.gnu.org/licenses/.
    =========================================================================
*/

#ifndef __SYNOCRACY_H_INCLUDED__
#define __SYNOCRACY_H_INCLUDED__

//  SYNOCRACY version macros for compile-time API detection

#define SYNOCRACY_VERSION_MAJOR 1
#define SYNOCRACY_VERSION_MINOR 0
#define SYNOCRACY_VERSION_PATCH 0

#define SYNOCRACY_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define SYNOCRACY_VERSION \
    SYNOCRACY_MAKE_VERSION(SYNOCRACY_VERSION_MAJOR, SYNOCRACY_VERSION_MINOR, SYNOCRACY_VERSION_PATCH)

#include <czmq.h>
#if CZMQ_VERSION < 20000
#   error "Synocracy needs CZMQ/2.0.0 or later"
#endif

#include "mongoose.h"
#include "myp_mystatefulmod.h"
#include "myp_mystatelessmod.h"

#endif
