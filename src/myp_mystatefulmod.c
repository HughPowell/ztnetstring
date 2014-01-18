#include <czmq.h>
#include "../include/synocracy.h"

//  Structure of our class

struct _myp_mystatefulmod_t {
    void* myprop;               // Description of this property
};

//  Create a new myp_mystatefulmod instance
myp_mystatefulmod_t *
myp_mystatefulmod_new ()
{
    // Allocate new instance on heap
    // Initialise all properties in instance
    // Return instance reference
    return NULL;
}

//  Destroy a myp_mystatefulmod instance
void
myp_mystatefulmod_destroy (myp_mystatefulmod_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        myp_mystatefulmod_t *self = *self_p;
        // Free any properties allocated on heap
        // Destroy any child class instances
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------
//  Runs selftest of class
void myp_mystatefulmod_test(bool verbose) {
    printf (" * myp_mystatefulmod: ");
    // Conduct tests of every method
    printf ("OK\n");
}
