/*!The Treasure Box Library
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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		dns.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../libc/libc.h"
#include "../../stream/stream.h"
#include "../../network/network.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t tb_dns_local_init()
{
	// init
	tb_long_t size = 0;
	tb_char_t line[8192];

	/* try get list from "/etc/resolv.conf"
	 *
	 * # Generated by NetworkManager
 	 * nameserver 10.1.20.10
 	 * nameserver 8.8.8.8
	 *
	 */
	tb_gstream_t* gst = tb_gstream_init_from_url("/etc/resolv.conf");
	tb_assert_and_check_return_val(gst, TB_NULL);

	// open
	if (!tb_gstream_bopen(gst)) goto end;

	// read
	while ((size = tb_gstream_bread_line(gst, line, 8192)) > 0)
	{
		if (!tb_strnicmp(line, "nameserver", 10))
		{
			// seek to host
			tb_char_t const* p = line + 10;
			while (*p && !tb_isdigit(*p)) p++;
			tb_check_continue(*p);

			// add host
			tb_dns_list_adds(p);
		}
	}

end:
	// exit
	if (gst) tb_gstream_exit(gst);
}
tb_void_t tb_dns_local_exit()
{
}

