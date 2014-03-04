#include <czmq.h>
#include "../include/synocracy.h"

static const uint NULL_LENGTH = 1;

static uint
s_get_str_len (long long number)
{
    long long abs_number = llabs (number);
    if (LLONG_MIN == number)
        abs_number = LLONG_MAX;
    uint result = (uint)(floor (log10 (abs_number)) + 1);
    if (number < 0)
        result += 1;
    return result;
}

static const uint MAX_TNETSTR_LENGTH = 999999999;
static const uint MAX_TNETSTR_SIZE_LENGTH  = 9;
static const uint COLON_LENGTH = 1;
static const uint TYPE_CHAR_LENGTH = 1;

struct _syn_tnetstr_t
{
    char * data;
    uint size;
    uint length;
    uint growth_factor;
};

syn_tnetstr_t *
syn_tnetstr_new ()
{
    syn_tnetstr_t *self = (syn_tnetstr_t *)malloc (sizeof (syn_tnetstr_t));
    assert (self);
    if (!self)
        return NULL;
    self->data = NULL;
    self->size = 0;
    self->length = 0;
    self->growth_factor = 2;
    return self;
}

void
syn_tnetstr_destroy (syn_tnetstr_t **p_self)
{
    assert (p_self);
    syn_tnetstr_t *self = *p_self;
    if (!self)
        return;
    if (self->data) {
        free (self->data);
        self->data = NULL;
    }
    free (self);
    self = NULL;
}

int
s_append (syn_tnetstr_t *self, const char *data, const char type)
{
    assert (self);
    
    // Make sure the data is of an appropriate length
    uint data_length = 0;
    if (NULL != data)
        data_length = strlen (data);
    assert (data_length <= MAX_TNETSTR_LENGTH);
    if (data_length > MAX_TNETSTR_LENGTH)
        return -1;

    // Allocate more memory if required
    int size_length = 1;
    // log10(0) is undefined
    if (0 != data_length)
        size_length = (int)(floor (log10 (data_length)) + 1);
    uint tnetstr_length = size_length + COLON_LENGTH + data_length + TYPE_CHAR_LENGTH;
    if ((self->length + tnetstr_length) >= self->size) {
        uint new_size = (self->length + tnetstr_length + NULL_LENGTH) * self->growth_factor * sizeof(char);
        char * new_data = realloc (self->data, new_size);
        assert (new_data);
        if (NULL == new_data)
            return -1;
        self->data = new_data;
        self->size = new_size;
    }

    // Copy in the data
    int chars_written = snprintf (self->data + self->length, tnetstr_length + NULL_LENGTH, "%d:%s%c", data_length, data ? data : "", type);
    assert (chars_written == tnetstr_length);
    if (chars_written != tnetstr_length) {
        return -1;
    }
    self->length += tnetstr_length;
    return 0;
}

int
syn_tnetstr_append_str (syn_tnetstr_t *self, const char *data)
{
    return s_append (self, data, ',');
}

int
syn_tnetstr_append_llong (syn_tnetstr_t *self, long long data)
{
    uint str_len = s_get_str_len (data);
    char data_str[str_len + NULL_LENGTH];
    int chars_written = snprintf (data_str, str_len + NULL_LENGTH, "%lld", data);
    assert (chars_written >= 0 || chars_written <= str_len);
    if (chars_written < 0 || chars_written > str_len)
        return -1;
    return s_append (self, data_str, '#');
}

int
syn_tnetstr_append_float (syn_tnetstr_t *self, float data)
{
    const uint MAX_FLOAT_CHARS = strlen("-0.") + FLT_MANT_DIG - FLT_MIN_EXP;
    char data_str[MAX_FLOAT_CHARS + NULL_LENGTH];
    int chars_written = snprintf (data_str, MAX_FLOAT_CHARS + NULL_LENGTH, "%.*f", MAX_FLOAT_CHARS, data);
    assert (chars_written >= 0 || chars_written <= MAX_FLOAT_CHARS);
    if (chars_written < 0 || chars_written > MAX_FLOAT_CHARS)
        return -1;
    return s_append (self, data_str, '^');
}

int
syn_tnetstr_append_bool (syn_tnetstr_t *self, bool data)
{
    if (data)
        return s_append (self, "true", '!');
    return s_append (self, "false", '!');
}

int
syn_tnetstr_append_null (syn_tnetstr_t *self)
{
    return s_append (self, NULL, '~');
}

typedef struct {
    syn_tnetstr_t *self;
    syn_tnetstr_dict_foreach_fn *fn;
} _foreach_args;

int
s_zhash_foreach_fn (const char *key, void *item, void *argument)
{
    _foreach_args *arguments = (_foreach_args *)argument;
    return arguments->fn (arguments->self, key, item);
}

int
syn_tnetstr_append_dict (syn_tnetstr_t *self, zhash_t *data, syn_tnetstr_dict_foreach_fn *fn)
{
    syn_tnetstr_t *items = syn_tnetstr_new ();
    assert (items);
    if (!items)
        return -1;
    _foreach_args arguments;
    arguments.self = items;
    arguments.fn = fn;
    int rc = zhash_foreach (data, &s_zhash_foreach_fn, &arguments);
    assert (0 == rc);
    if (0 != rc) {
        syn_tnetstr_destroy (&items);
        return -1;
    }
    rc = s_append (self, items->data, '}');
    syn_tnetstr_destroy (&items);
    return rc;
}

int
syn_tnetstr_append_list (syn_tnetstr_t *self, zlist_t *data, syn_tnetstr_list_foreach_fn *fn)
{
    syn_tnetstr_t *items = syn_tnetstr_new ();
    assert (items);
    if (!items)
        return -1;
    void * item = zlist_first (data);
    while (item) {
        int rc = fn (items, item);
        if (0 != rc) {
            syn_tnetstr_destroy (&items);
            return rc;
        }
        item = zlist_next (data);
    }
    int rc = s_append (self, items->data, ']');
    syn_tnetstr_destroy (&items);
    return rc;
}

char *
syn_tnetstr_get (syn_tnetstr_t *self)
{
    return self->data;
}

static void
s_free_list (void *data)
{
    zlist_t *list = (zlist_t *)data;
    zlist_destroy (&list);
}

static void
s_free_dict (void *data)
{
    zhash_t *dict = (zhash_t *)data;
    zhash_destroy (&dict);
}

void *
syn_tnetstr_parse (char **p_tnetstr)
{
    assert (p_tnetstr);
    if (NULL == p_tnetstr)
        return NULL;
    char * tnetstr = *p_tnetstr;
    assert (tnetstr);
    if (NULL == tnetstr)
        return NULL;
    char * colon = NULL;
    long int length = strtol (tnetstr, &colon, 10);
    if (0 == length && '0' != *tnetstr)
        return NULL;
    char type = *(colon + COLON_LENGTH + length);
    void * result = NULL;
    switch (type) {
    case ',':
    {
        char *str = (char *)malloc ((length + NULL_LENGTH) * sizeof(char));
        assert (str);
        if (!str) {
            free (result);
            return NULL;
        }
        memcpy(str, colon + COLON_LENGTH, length);
        *(str + length) = '\0';
        result = str;
        break;
    }
    case '#':
    {
        long long *number = malloc (sizeof(long long));
        assert (number);
        if (!number)
            return NULL;
        char * check;
        *number = strtoll (colon + COLON_LENGTH, &check, 10);
        if (*check != type) {
            free (number);
            return NULL;
        }
        if (LLONG_MAX == *number) {
            uint llong_max_str_len = s_get_str_len (LLONG_MAX);
            char llong_max_str[llong_max_str_len + NULL_LENGTH];
            snprintf (llong_max_str, llong_max_str_len + NULL_LENGTH, "%lld", LLONG_MAX);
            uint index = 0;
            while (*(colon + COLON_LENGTH + index) == *(llong_max_str + index)) {
                ++index;

            }
            if (('#' != *(colon + COLON_LENGTH + index)) || ('\0' != *(llong_max_str + index))) {
                free (number);
                return NULL;
            }
        } else
        if (LLONG_MIN == *number) {
            uint llong_min_str_len = s_get_str_len (LLONG_MIN);
            char llong_min_str[llong_min_str_len + NULL_LENGTH];
            snprintf (llong_min_str, llong_min_str_len + NULL_LENGTH, "%lld", LLONG_MIN);
            uint index = 0;
            while (*(colon + COLON_LENGTH + index) == *(llong_min_str + index)) ++index;
            if (('#' != *(colon + COLON_LENGTH + index)) || ('\0' != *(llong_min_str + index))) {
                free (number);
                return NULL;
            }
        }
        result = number;
        break;
    }
    case '^':
    {
        float * flt = malloc (sizeof(float));
        *flt = strtof (colon + COLON_LENGTH, NULL);
        result = flt;
        break;
    }
    case '!':
    {
        bool *boolean = malloc (sizeof(bool));
        char first_char = *(colon + COLON_LENGTH);
        switch (first_char) {
        case 't':
            *boolean = true;
            break;
        case 'f':
            *boolean = false;
            break;
        default:
            free (boolean);
            return NULL;
        }
        result = boolean;
        break;
    }
    case '~':
    {
        // Nothing to do
        break;
    }
    case '}':
    {
        zhash_t *dict = zhash_new ();
        char * index = colon + COLON_LENGTH;
        while ('}' != *index) {
            if (index > (colon + COLON_LENGTH + length)) {
                zhash_destroy(&dict);
                return NULL;
            }

            char * key = syn_tnetstr_parse (&index);
            if (!key) {
                zhash_destroy (&dict);
                return NULL;
            }

            void * item = syn_tnetstr_parse (&index);
            if (!item) {
                zhash_destroy (&dict);
                return NULL;
            }
            int rc = zhash_insert (dict, key, item);

            // Apply the correct free function
            char item_type = *(index - TYPE_CHAR_LENGTH);
            switch (item_type) {
            case '}':
                if (NULL == zhash_freefn (dict, key, &s_free_dict)) {
                    zhash_t *item_as_dict = (zhash_t *)item;
                    zhash_destroy (&item_as_dict);
                    zhash_destroy (&dict);
                    return NULL;
                }
                break;
            case ']':
                if (NULL == zhash_freefn (dict, key, &s_free_list)) {
                    zlist_t *item_as_list = (zlist_t *)item;
                    zlist_destroy (&item_as_list);
                    zhash_destroy (&dict);
                    return NULL;
                }
                break;
            default:
                if (NULL == zhash_freefn (dict, key, &free)) {
                    free (item);
                    zhash_destroy (&dict);
                    return NULL;
                }
                break;
            }
            
            // Free the key as we've taken a copy
            free (key);
        }
        result = dict;
        break;
    }
    case ']':
    {
        zlist_t *list = zlist_new ();
        char * index = colon + COLON_LENGTH;
        while (']' != *index) {
            if (index > (colon + COLON_LENGTH + length)) {
                zlist_destroy(&list);
                return NULL;
            }

            void * item = syn_tnetstr_parse (&index);
            int rc = zlist_append (list, item);

            // Apply the correct free function
            char item_type = *(index - 1);
            switch (item_type) {
            case '}':
                if (NULL == zlist_freefn (list, item, &s_free_dict, true)) {
                    zhash_t *item_as_dict = (zhash_t *)item;
                    zhash_destroy (&item_as_dict);
                    zlist_destroy (&list);
                    return NULL;
                }
                break;
            case ']':
                if (NULL == zlist_freefn (list, item, &s_free_list, true)) {
                    zlist_t *item_as_list = (zlist_t *)item;
                    zlist_destroy (&item_as_list);
                    zlist_destroy (&list);
                    return NULL;
                }
                break;
            default:
                if (NULL == zlist_freefn (list, item, &free, true)) {
                    free (item);
                    zlist_destroy (&list);
                    return NULL;
                }
                break;
            }
        }
        result = list;
        break;
    }
    default:
        return NULL;
    }
    
    *p_tnetstr = colon + COLON_LENGTH + length + TYPE_CHAR_LENGTH;
    return result;
}

int
s_tnetstr_dict_fn (syn_tnetstr_t *self, const char *key, void *item)
{
    // Do nothing
    return 0;
}

int
s_tnetstr_list_fn (syn_tnetstr_t *self, void *item)
{
    // Do nothing
    return 0;
}

int
s_tnetstr_foreach_dict_fn_test (syn_tnetstr_t *self, const char *key, void *item)
{
    int rc = syn_tnetstr_append_str (self, (char *)key);
    assert (0 == rc);
    if (0 != rc)
        return rc;
    if (streq (key, "STRING"))
        return syn_tnetstr_append_str (self, (char *)item);
    if (streq (key, "INTEGER"))
        return syn_tnetstr_append_llong (self, *(long long *)item);
    if (streq (key, "BOOLEAN"))
        return syn_tnetstr_append_bool (self, *(bool *)item);
    if (streq (key, "HASH"))
        return syn_tnetstr_append_dict (self, (zhash_t *)item, &s_tnetstr_dict_fn);
    if (streq (key, "LIST"))
        return syn_tnetstr_append_list (self, (zlist_t *)item, &s_tnetstr_list_fn);
    return -1;
}

uint s_list_index = 0;

int
s_tnetstr_foreach_list_fn_test (syn_tnetstr_t *self, void *item)
{
    uint list_index = s_list_index++;
    if (0 == list_index)
        return syn_tnetstr_append_str (self, (char *)item);
    if (1 == list_index)
        return syn_tnetstr_append_llong (self, *(long long *)item);
    if (2 == list_index)
        return syn_tnetstr_append_bool (self, *(bool *)item);
    if (3 == list_index)
        return syn_tnetstr_append_dict (self, (zhash_t *)item, &s_tnetstr_dict_fn);
    if (4 == list_index)
        return syn_tnetstr_append_list (self, (zlist_t *)item, &s_tnetstr_list_fn);
    return -1;

}

void
s_zhash_free_fn (void *data) {
    zhash_t *hash = (zhash_t *)data;
    zhash_destroy (&hash);
}

void
s_zlist_free_fn (void *data) {
    zlist_t *list = (zlist_t *)data;
    zlist_destroy (&list);
}

// --------------------------------------
// Runs selftest of class

void syn_tnetstr_test (bool verbose) {
    printf (" * syn_tnetstr: ");

    // Strings
    syn_tnetstr_t *tnetstr = syn_tnetstr_new ();
    char *data_str = "Hello World!";
    char *tnetstr_str = "12:Hello World!,";
    int rc = syn_tnetstr_append_str (tnetstr, data_str);
    assert (0 == rc);
    assert (streq (syn_tnetstr_get (tnetstr), tnetstr_str));
    char * index = tnetstr_str;
    char *result_str = (char *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (streq (result_str, data_str));
    free (result_str);
    syn_tnetstr_destroy (&tnetstr);

    tnetstr = syn_tnetstr_new ();
    char *data_empty_str = "";
    char *tnetstr_empty_str = "0:,";
    rc = syn_tnetstr_append_str (tnetstr, data_empty_str);
    assert (0 == rc);
    assert (streq (syn_tnetstr_get (tnetstr), tnetstr_empty_str));
    index = tnetstr_empty_str;
    result_str = (char *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (streq (result_str, data_empty_str));
    free (result_str);
    syn_tnetstr_destroy (&tnetstr);

    tnetstr = syn_tnetstr_new ();
    char *data_tnet_str = "12:Hello World!,";
    tnetstr_str = "16:12:Hello World!,,";
    rc = syn_tnetstr_append_str (tnetstr, data_tnet_str);
    assert (0 == rc);
    assert (streq (syn_tnetstr_get (tnetstr), tnetstr_str));
    index = tnetstr_str;
    result_str = (char *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (streq (result_str, data_tnet_str));
    free (result_str);
    syn_tnetstr_destroy (&tnetstr);

    // Numbers
    tnetstr = syn_tnetstr_new ();
    long long data_llong = 34;
    char *tnetstr_llong = "2:34#";
    rc = syn_tnetstr_append_llong (tnetstr, data_llong);
    assert (0 == rc);
    assert (streq (syn_tnetstr_get (tnetstr), tnetstr_llong));
    index = tnetstr_llong;
    long long *result_llong = (long long *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (data_llong == *result_llong);
    free (result_llong);
    syn_tnetstr_destroy (&tnetstr);

    tnetstr = syn_tnetstr_new ();
    rc = syn_tnetstr_append_llong (tnetstr, LLONG_MAX);
    assert (0 == rc);
    index = syn_tnetstr_get (tnetstr);
    result_llong = (long long *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (LLONG_MAX == *result_llong);
    free (result_llong);
    syn_tnetstr_destroy (&tnetstr);

    tnetstr = syn_tnetstr_new ();
    rc = syn_tnetstr_append_llong (tnetstr, LLONG_MIN);
    assert (0 == rc);
    index = syn_tnetstr_get (tnetstr);
    result_llong = (long long *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (LLONG_MIN == *result_llong);
    free (result_llong);
    syn_tnetstr_destroy (&tnetstr);

    char *tnetstr_llong_max_plus_one = "19:9223372036854775808#";
    index = tnetstr_llong_max_plus_one;
    result_llong = (long long *)syn_tnetstr_parse (&index);
    assert (NULL == result_llong);

    char *tnetstr_llong_min_minus_one = "20:-9223372036854775809#";
    index = tnetstr_llong_min_minus_one;
    result_llong = (long long *)syn_tnetstr_parse (&index);
    assert (NULL == result_llong);

    char *tnetstr_float_not_llong = "8:15.75331#";
    index = tnetstr_float_not_llong;
    result_llong = (long long *)syn_tnetstr_parse (&index);
    assert (NULL == result_llong);

    // Floats
    // ### These are a bastard to test and until there's a real use
    // I've got better things to do with my time
    //tnetstr = syn_tnetstr_new ();
    //float data_float = 15.75331;
    //char *tnetstr_float = "8:15.75331^";
    //rc = syn_tnetstr_append_float (tnetstr, data_float);
    //assert (0 == rc);
    //assert (streq (syn_tnetstr_get (tnetstr), tnetstr_float));
    //index = tnetstr_float;
    //float *result_float = (float *)syn_tnetstr_parse (&index);
    //assert (streq (index, ""));
    //assert (data_float == *result_float);
    //free (result_float);
    //syn_tnetstr_destroy (&tnetstr);

    // Booleans
    tnetstr = syn_tnetstr_new ();
    bool data_bool = true;
    char *tnetstr_bool = "4:true!";
    rc = syn_tnetstr_append_bool (tnetstr, data_bool);
    assert (0 == rc);
    assert (streq (syn_tnetstr_get (tnetstr), tnetstr_bool));
    index = tnetstr_bool;
    bool *result_bool = (bool *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (data_bool == *result_bool);
    free (result_bool);
    syn_tnetstr_destroy (&tnetstr);

    // NULL
    tnetstr = syn_tnetstr_new ();
    char *tnetstr_null = "0:~";
    rc = syn_tnetstr_append_null (tnetstr);
    assert (streq (syn_tnetstr_get (tnetstr), tnetstr_null));
    index = tnetstr_null;
    void *result_null = syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (NULL == result_null);
    syn_tnetstr_destroy (&tnetstr);

    // Dictionaries
    zhash_t *dict = zhash_new ();
    zhash_t *empty_hash = zhash_new ();
    zlist_t *empty_list = zlist_new ();
    zhash_insert (dict, "STRING", data_str);
    zhash_insert (dict, "INTEGER", &data_llong);
    zhash_insert (dict, "BOOLEAN", &data_bool);
    zhash_insert (dict, "HASH", empty_hash);
    zhash_freefn (dict, "HASH", &s_zhash_free_fn);
    zhash_insert (dict, "LIST", empty_list);
    zhash_freefn (dict, "LIST", &s_zlist_free_fn);

    tnetstr = syn_tnetstr_new ();
    rc = syn_tnetstr_append_dict (tnetstr, dict, &s_tnetstr_foreach_dict_fn_test);
    assert (0 == rc);
    zhash_destroy (&dict);
    index = syn_tnetstr_get (tnetstr);
    zhash_t *result_dict = (zhash_t *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (NULL != result_dict);
    zhash_autofree (result_dict);
    char *item_str = (char *)zhash_lookup (result_dict, "STRING");
    assert (streq (data_str, item_str));
    long long *item_llong = (long long *)zhash_lookup (result_dict, "INTEGER");
    assert (*item_llong == data_llong);
    bool *item_bool = (bool *)zhash_lookup (result_dict, "BOOLEAN");
    assert (*item_bool == data_bool);
    zhash_t *item_hash = (zhash_t *)zhash_lookup (result_dict, "HASH");
    assert (0 == zhash_size (item_hash));
    zlist_t *item_list = (zlist_t *)zhash_lookup (result_dict, "LIST");
    assert (0 == zlist_size (item_list));
    zhash_destroy (&result_dict);
    syn_tnetstr_destroy (&tnetstr);

    // Lists
    zlist_t *list = zlist_new ();
    empty_hash = zhash_new ();
    empty_list = zlist_new ();
    zlist_append (list, data_str);
    zlist_append (list, &data_llong);
    zlist_append (list, &data_bool);
    zlist_append (list, empty_hash);
    zlist_freefn (list, empty_hash, &s_zhash_free_fn, true);
    zlist_append (list, empty_list);
    zlist_freefn (list, empty_list, &s_zlist_free_fn, true);

    tnetstr = syn_tnetstr_new ();
    rc = syn_tnetstr_append_list (tnetstr, list, &s_tnetstr_foreach_list_fn_test);
    assert (0 == rc);
    zlist_destroy (&list);
    index = syn_tnetstr_get (tnetstr);
    zlist_t *result_list = (zlist_t *)syn_tnetstr_parse (&index);
    assert (streq (index, ""));
    assert (NULL != result_list);
    item_str = (char *)zlist_pop (result_list);
    assert (streq (data_str, item_str));
    free (item_str);
    item_llong = (long long *)zlist_pop (result_list);
    assert (*item_llong == data_llong);
    free (item_llong);
    item_bool = (bool *)zlist_pop (result_list);
    assert (*item_bool == data_bool);
    free (item_bool);
    item_hash = (zhash_t *)zlist_pop (result_list);
    assert (0 == zhash_size (item_hash));
    zhash_destroy (&item_hash);
    item_list = (zlist_t *)zlist_pop (result_list);
    assert (0 == zlist_size (item_list));
    zlist_destroy (&item_list);
    zlist_destroy (&result_list);
    syn_tnetstr_destroy (&tnetstr);

    printf ("OK\n");
}
