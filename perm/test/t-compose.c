/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 Fredrik Johansson

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "flint.h"
#include "perm.h"
#include "ulong_extras.h"

int main(void)
{
    int i;
    flint_rand_t state;
    flint_randinit(state);

    flint_printf("compose....");
    fflush(stdout);

    /* check (b^(-1))(b(a)) = a */
    for (i = 0; i < 10000; i++)
    {
        slong n, *a, *b, *binv, *c;

        n = n_randint(state, 100);

        a = _perm_init(n);
        b = _perm_init(n);
        binv = _perm_init(n);
        c = _perm_init(n);

        _perm_randtest(a, n, state);
        _perm_randtest(b, n, state);
        _perm_inv(binv, b, n);

        _perm_compose(c, b, a, n);
        _perm_compose(c, binv, c, n);

        if (!_perm_equal(a, c, n))
        {
            flint_printf("FAIL:\n");
            flint_printf("a: "); _perm_print(a, n); flint_printf("\n\n");
            flint_printf("b: "); _perm_print(b, n); flint_printf("\n\n");
            flint_printf("binv: "); _perm_print(binv, n); flint_printf("\n\n");
            flint_printf("c: "); _perm_print(c, n); flint_printf("\n\n");
            abort();
        }

        _perm_clear(a);
        _perm_clear(b);
        _perm_clear(binv);
        _perm_clear(c);
    }

    /* check aliasing with first argument */
    for (i = 0; i < 10000; i++)
    {
        slong n, *a, *b, *c;

        n = n_randint(state, 100);

        a = _perm_init(n);
        b = _perm_init(n);
        c = _perm_init(n);

        _perm_randtest(a, n, state);
        _perm_randtest(b, n, state);

        _perm_compose(c, b, a, n);
        _perm_compose(b, b, a, n);

        if (!_perm_equal(b, c, n))
        {
            flint_printf("FAIL:\n");
            flint_printf("a: "); _perm_print(a, n); flint_printf("\n\n");
            flint_printf("b: "); _perm_print(b, n); flint_printf("\n\n");
            flint_printf("c: "); _perm_print(c, n); flint_printf("\n\n");
            abort();
        }

        _perm_clear(a);
        _perm_clear(b);
        _perm_clear(c);
    }

    flint_randclear(state);
    flint_cleanup();
    flint_printf("PASS\n");
    return 0;
}
