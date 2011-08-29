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
 * \file		memset.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * implemention 
 */
#if defined(TB_CONFIG_ARCH_x86)
# 	include "opt/x86/memset.c"
#elif defined(TB_CONFIG_ARCH_ARM)
# 	include "opt/arm/memset.c"
#elif defined(TB_CONFIG_ARCH_SH4)
# 	include "opt/sh4/memset.c"
#else
tb_void_t* tb_memset(tb_void_t* s, tb_size_t c, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s, TB_NULL);

	__tb_register__ tb_byte_t* p = s;

	while (n) 
	{
		*p++ = (tb_byte_t)c;
		--n;
	}

	return s;
}

#endif