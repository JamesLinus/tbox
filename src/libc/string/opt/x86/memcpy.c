/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		memcpy.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_void_t* tb_memcpy(tb_void_t* s1, tb_void_t const* s2, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s1 && s2, TB_NULL);

#if 0
	tb_int_t d0, d1, d2;
	__tb_asm__ __tb_volatile__
	(
		" 	rep; 	movsl\n"
		" 	movl 	%4, %%ecx\n"
		" 	andl 	$3, %%ecx\n"
		/* jz is optional. avoids "rep; movsb" with ecx == 0,
		* but adds a branch, which is currently (2008) faster */
		" 	jz		1f\n"
		" 	rep; 	movsb\n"
		"1:\n"

		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (n / 4), "g" (n), "1" ((tb_size_t)s1), "2" ((tb_size_t)s2)
		: "memory"
	);
	return s1;
#else
	return memcpy(s1, s2, n);
#endif
}
