/*  =========================================================================
    ztnetstr.h - ztnetstring library

    -------------------------------------------------------------------------
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of ztnetstring, see http://github.com/HughPowell/ztnetstring.

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

#ifndef __ZTNETSTR_H_INCLUDED__
#define __ZTNETSTR_H_INCLUDED__

//  ZTNETSTR version macros for compile-time API detection

#define ZTNETSTR_VERSION_MAJOR 1
#define ZTNETSTR_VERSION_MINOR 0
#define ZTNETSTR_VERSION_PATCH 0

#define ZTNETSTR_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define ZTNETSTR_VERSION \
    ZTNETSTR_MAKE_VERSION(ZTNETSTR_VERSION_MAJOR, ZTNETSTR_VERSION_MINOR, ZTNETSTR_VERSION_PATCH)

#include <czmq.h>
#if CZMQ_VERSION < 20000
#   error "ztnetstring needs CZMQ/2.0.0 or later"
#endif

#include "ztns_impl.h"

#endif
