/*  =========================================================================
    ztns_impl.h - library for parsing/serializing to/from tnetstrings
    defined at http://tnetstrings.org
    
    -------------------------------------------------------------------------
    Copyright (c) 2014 - 
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of ztnetstrings, <description>

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

#ifndef __ZTNS_IMPL_H__
#define __ZTNS_IMPL_H__

#ifdef __cplusplus
extern "C" {
#endif

struct zhash_t;
struct zlist_t;

// Opaque class structure
typedef struct _ztns_t ztns_t;

CZMQ_EXPORT ztns_t *
    ztns_new ();

CZMQ_EXPORT void
    ztns_destroy (ztns_t **p_self);

CZMQ_EXPORT int
    ztns_append_str (ztns_t *self, const char *data);

CZMQ_EXPORT int
    ztns_append_llong (ztns_t *self, long long data);

CZMQ_EXPORT int
    ztns_append_float (ztns_t *self, float data);

CZMQ_EXPORT int
    ztns_append_bool (ztns_t *self, bool data);

CZMQ_EXPORT int
    ztns_append_null (ztns_t *self);

typedef int (ztns_dict_foreach_fn) (ztns_t *self, const char *key, void *item);

CZMQ_EXPORT int
    ztns_append_dict (ztns_t *self, zhash_t *data, ztns_dict_foreach_fn *fn);

typedef int (ztns_list_foreach_fn) (ztns_t *self, void *item);

CZMQ_EXPORT int
    ztns_append_list (ztns_t *self, zlist_t *data, ztns_list_foreach_fn *fn);

CZMQ_EXPORT char *
    ztns_get (ztns_t *self);

// Parse the given tnetstring
// Returns NULL if the tnetstring is malformed
CZMQ_EXPORT void *
    ztns_parse (char **p_tnetstr);

// Self test of this class
void
    ztns_test (bool verbose);

#ifdef __cplusplus
}
#endif

#endif
