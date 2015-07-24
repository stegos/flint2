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

    Copyright (C) 2015 Vladimir Glazachev
   
******************************************************************************/

#include "aprcl.h"

ulong
_R_value(const fmpz_t n)
{
    ulong bits = fmpz_bits(n);

    if (bits <= 17) return 6;
    if (bits <= 31) return 12;
    if (bits <= 54) return 36;
    if (bits <= 68) return 72;
    if (bits <= 101) return 180;
    if (bits <= 127) return 360;
    if (bits <= 152) return 720;
    if (bits <= 204) return 1260;
    if (bits <= 268) return 2520;
    if (bits <= 344) return 5040;
    if (bits <= 525) return 27720;
    if (bits <= 650) return 55440;
    if (bits <= 774) return 110880;
    if (bits <= 1035) return 166320;
    if (bits <= 1566) return 720720;
    if (bits <= 2082) return 1663200;
    if (bits <= 3491) return 8648640;
    return 6983776800;
}

void _jacobi_config_reduce_s(aprcl_config conf, const fmpz_t n)
{
    slong i;
    fmpz_t new_s, new_s2, p;

    fmpz_init(p);
    fmpz_init(new_s2);
    fmpz_init_set(new_s, conf->s);

    for (i = conf->qs->num - 1; i >= 0; i--)
    {
        fmpz_pow_ui(p, conf->qs->p + i, conf->qs->exp[i]);
        fmpz_fdiv_q(new_s, conf->s, p);

        fmpz_mul(new_s2, new_s, new_s);
        if (fmpz_cmp(new_s2, n) > 0)
        {
            fmpz_set(conf->s, new_s);
            conf->qs_used[i] = 0;
        }
        else
        {
            conf->qs_used[i] = 1;
        }
    }

    fmpz_clear(p);
    fmpz_clear(new_s2);
    fmpz_clear(new_s);
}

void _jacobi_config_update(aprcl_config conf)
{
    ulong prime = 2;

    fmpz_set_ui(conf->s, 1);
    fmpz_factor_clear(conf->qs);
    fmpz_factor_init(conf->qs);
    conf->qs->sign = 1;

    _fmpz_factor_append_ui(conf->qs, prime, p_power_in_q(conf->R, prime) + 2);
    fmpz_mul_ui(conf->s, conf->s, n_pow(prime, p_power_in_q(conf->R, prime) + 2));

    prime = 3;
    while (2 * (prime - 1) <= conf->R)
    {
        if ((conf->R % (prime - 1)) == 0)
        {
            _fmpz_factor_append_ui(conf->qs, prime, p_power_in_q(conf->R, prime) + 1);
            fmpz_mul_ui(conf->s, conf->s, n_pow(prime, p_power_in_q(conf->R, prime) + 1));
        }
        prime++;
        while (n_is_prime(prime) == 0)
            prime++;
    }

    if (n_is_prime(conf->R + 1))
    {
        _fmpz_factor_append_ui(conf->qs, conf->R + 1, 1);
        fmpz_mul_ui(conf->s, conf->s, conf->R + 1);
    }
}

/* Computes s = \prod q^(k + 1) ; q - prime, q - 1 | R; q^k | R and q^(k + 1) not | R */
void jacobi_config_init(aprcl_config conf, const fmpz_t n)
{
    ulong i;
    fmpz_init(conf->s);
    fmpz_factor_init(conf->qs);
    conf->R = _R_value(n);
    _jacobi_config_update(conf);

    n_factor_init(&conf->rs);
    n_factor(&conf->rs, conf->R, 1);

    conf->qs_used = (int *) flint_malloc(sizeof(int) * conf->qs->num);
    _jacobi_config_reduce_s(conf, n);
}

void jacobi_config_clear(aprcl_config conf)
{
    fmpz_clear(conf->s);
    fmpz_factor_clear(conf->qs);
    flint_free(conf->qs_used);
}

