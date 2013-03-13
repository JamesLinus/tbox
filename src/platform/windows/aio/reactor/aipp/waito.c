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
 * @file		waito.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the waito reactor type
typedef struct __tb_aipp_reactor_waito_t
{
	// the reactor base
	tb_aipp_reactor_t 		base;

	// the waito handles
	tb_vector_t* 			hdls;

}tb_aipp_reactor_waito_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aipp_reactor_waito_addo(tb_aipp_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aipp_reactor_waito_t* rtor = (tb_aipp_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hdls && reactor->aipp && reactor->aipp->hash, tb_false);

	// check
	tb_assert_and_check_return_val(tb_hash_size(reactor->aipp->hash) < MAXIMUM_WAIT_OBJECTS, tb_false);

	// add hdl
	tb_vector_insert_tail(rtor->hdls, handle);

	// ok
	return tb_true;
}
static tb_bool_t tb_aipp_reactor_waito_seto(tb_aipp_reactor_t* reactor, tb_handle_t handle, tb_size_t etype, tb_aioo_t const* obj)
{
	// ok
	return tb_true;
}
static tb_bool_t tb_aipp_reactor_waito_delo(tb_aipp_reactor_t* reactor, tb_handle_t handle)
{
	tb_aipp_reactor_waito_t* rtor = (tb_aipp_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hdls, tb_false);

	// find hdl
	tb_size_t itor = tb_iterator_head(rtor->hdls);
	tb_size_t tail = tb_iterator_tail(rtor->hdls);
	for (; itor != tail; itor = tb_iterator_next(rtor->hdls, itor))
	{
		tb_handle_t hdl = (tb_handle_t)tb_iterator_item(rtor->hdls, itor);
		if (hdl == handle) break;
	}
	tb_assert_and_check_return_val(itor != tail, tb_false);
	
	// del hdl
	tb_vector_remove(rtor->hdls, itor);

	// ok
	return tb_true;
}
static tb_long_t tb_aipp_reactor_waito_wait(tb_aipp_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	tb_aipp_reactor_waito_t* rtor = (tb_aipp_reactor_waito_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hdls && reactor->aipp && reactor->aipp->hash, -1);

	// hdls
	tb_handle_t*	hdls = (tb_handle_t*)tb_vector_data(rtor->hdls);
	tb_assert_and_check_return_val(hdls, -1);

	// hdlm
	tb_size_t 		hdlm = tb_vector_size(rtor->hdls);
	tb_check_return_val(hdlm, 0);

	// wait
	tb_long_t 		hdli = WaitForMultipleObjects((DWORD)hdlm, (HANDLE const*)hdls, FALSE, timeout >= 0? timeout : INFINITE);
	tb_assert_and_check_return_val(hdli != WAIT_FAILED, -1);

	// timeout?
	tb_check_return_val(hdli != WAIT_TIMEOUT, 0);

	// error?
	tb_check_return_val(hdli >= WAIT_OBJECT_0, -1);

	// has more event?
	tb_size_t objn = 0;
	while (hdli < WAIT_OBJECT_0 + hdlm && objn < objm)
	{
		// get object
		hdli -= WAIT_OBJECT_0;
		tb_aioo_t* o = tb_hash_get(reactor->aipp->hash, hdls[hdli]);
		tb_assert_and_check_return_val(o, -1);

		// add object
		objs[objn] = *o;

		// next
		hdli++;
		objn++;

		// end?
		tb_check_break(hdli < hdlm);

		// wait next
		hdls += hdli;
		hdlm -= hdli;
		hdli = WaitForMultipleObjects((DWORD)hdlm, (HANDLE const*)hdls, FALSE, 0);
		tb_assert_and_check_return_val(hdli != WAIT_FAILED, -1);

		// no events?
		tb_check_break(hdli != WAIT_TIMEOUT && hdli >= WAIT_OBJECT_0);
	}

	// ok
	return objn;
}
static tb_void_t tb_aipp_reactor_waito_exit(tb_aipp_reactor_t* reactor)
{
	tb_aipp_reactor_waito_t* rtor = (tb_aipp_reactor_waito_t*)reactor;
	if (rtor)
	{
		// exit hdls
		if (rtor->hdls) tb_vector_exit(rtor->hdls);

		// free it
		tb_free(rtor);
	}
}

static tb_aipp_reactor_t* tb_aipp_reactor_waito_init(tb_aipp_t* aipp)
{
	// check
	tb_assert_and_check_return_val(aipp && aipp->maxn, tb_null);
	tb_assert_and_check_return_val(aipp->type == TB_AIOO_OTYPE_FILE, tb_null);
	tb_assert_static(sizeof(tb_handle_t) == sizeof(HANDLE));

	// alloc reactor
	tb_aipp_reactor_waito_t* rtor = tb_malloc0(sizeof(tb_aipp_reactor_waito_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aipp = aipp;
	rtor->base.exit = tb_aipp_reactor_waito_exit;
	rtor->base.addo = tb_aipp_reactor_waito_addo;
	rtor->base.seto = tb_aipp_reactor_waito_seto;
	rtor->base.delo = tb_aipp_reactor_waito_delo;
	rtor->base.wait = tb_aipp_reactor_waito_wait;

	// init hdls
	rtor->hdls = tb_vector_init(tb_align8((aipp->maxn >> 3) + 1), tb_item_func_ptr());
	tb_assert_and_check_goto(rtor->hdls, fail);

	// ok
	return (tb_aipp_reactor_t*)rtor;

fail:
	if (rtor) tb_aipp_reactor_waito_exit(rtor);
	return tb_null;
}

