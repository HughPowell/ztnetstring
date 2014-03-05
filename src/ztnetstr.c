/* =========================================================================
    ztnetstr - ztnetstring command-line service

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
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#include "czmq.h"
#include "../include/ztnetstr.h"

#define PRODUCT "ztnetstring service/1.0"
#define COPYRIGHT "Copyright (c) 2014 Hugh Powell"
#define NOWARRANTY \
"This is free software; see the source for copying conditions. There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"

int main (int argc, char *argv [])
{
    puts (PRODUCT);
    puts (COPYRIGHT);
    puts (NOWARRANTY);

    return 0;
}
