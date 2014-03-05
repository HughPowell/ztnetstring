/*  =========================================================================
    tnetstrings.h - library for parsing/serializing to/from tnetstrings
    defined at http://tnetstrings.org
    
    -------------------------------------------------------------------------
    Copyright (c) 2014 - http://synocracy.org
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of Synocracy, <description>
    http://synocracy.org

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

#ifndef __SYN_TNETSTR_H__
#define __SYN_TNETSTR_H__

#ifdef __cplusplus
extern "C" {
#endif

struct zhash_t;
struct zlist_t;

// Opaque class structure
typedef struct _syn_tnetstr_t syn_tnetstr_t;

CZMQ_EXPORT syn_tnetstr_t *
    syn_tnetstr_new ();

CZMQ_EXPORT void
    syn_tnetstr_destroy (syn_tnetstr_t **p_self);

CZMQ_EXPORT int
    syn_tnetstr_append_str (syn_tnetstr_t *self, const char *data);

CZMQ_EXPORT int
    syn_tnetstr_append_llong (syn_tnetstr_t *self, long long data);

CZMQ_EXPORT int
    syn_tnetstr_append_float (syn_tnetstr_t *self, float data);

CZMQ_EXPORT int
    syn_tnetstr_append_bool (syn_tnetstr_t *self, bool data);

CZMQ_EXPORT int
    syn_tnetstr_append_null (syn_tnetstr_t *self);

typedef int (syn_tnetstr_dict_foreach_fn) (syn_tnetstr_t *self, const char *key, void *item);

CZMQ_EXPORT int
    syn_tnetstr_append_dict (syn_tnetstr_t *self, zhash_t *data, syn_tnetstr_dict_foreach_fn *fn);

typedef int (syn_tnetstr_list_foreach_fn) (syn_tnetstr_t *self, void *item);

CZMQ_EXPORT int
    syn_tnetstr_append_list (syn_tnetstr_t *self, zlist_t *data, syn_tnetstr_list_foreach_fn *fn);

CZMQ_EXPORT char *
    syn_tnetstr_get (syn_tnetstr_t *self);

// Parse the given tnetstring
// Returns NULL if the tnetstring is malformed
CZMQ_EXPORT void *
    syn_tnetstr_parse (char **p_tnetstr);

// Self test of this class
void
    syn_tnetstr_test (bool verbose);

#ifdef __cplusplus
}
#endif

#endif
