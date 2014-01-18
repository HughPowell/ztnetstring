/*  =========================================================================
    <name> - <description>
    
    -------------------------------------------------------------------------
    Copyright (c) <year> - <company name> - <website>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of <project name>, <description>
    <website>

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANT-
    ABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not see http://www.gnu.org/licenses.
    =========================================================================
*/

#ifndef __MYSTATEFULMOD_H__
#define __MYSTATEFULMOD_H__

#ifdef __cplusplus
extern "C" {
#endif

// Opaque class structure
typedef struct _myp_mystatefulmod_t myp_mystatefulmod_t;

// Create a new <class name> instance
CZMQ_EXPORT myp_mystatefulmod_t *
    myp_mystatefulmod_new (void);

// Destroy a <class name> instance
CZMQ_EXPORT void
    myp_mystatefulmod_destroy (myp_mystatefulmod_t **self_p);

// Self test of this class
void
    myp_mystatefulmod_test(bool verbose);

#ifdef __cplusplus
}
#endif

#endif
