/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

#include "llsd_const.h"
#include "macros.h"

int8_t const * const llsd_type_strings[LLSD_TYPE_COUNT] =
{
	T("UNDEF"),
	T("TRUE"),
	T("FALSE"),
	T("INTEGER"),
	T("REAL"),
	T("UUID"),
	T("STRING"),
	T("DATE"),
	T("URI"),
	T("BINARY"),
	T("ARRAY"),
	T("MAP")
};

/* constants */
llsd_t const undefined =
{
	.type_ = LLSD_UNDEF,
	.value.bool_ = FALSE
};

static uint8_t bits[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
llsd_uuid_t const zero_uuid = 
{ 
	.dyn_bits = FALSE,
	.dyn_str = FALSE,
	.bits = bits,
	.str = NULL
};

llsd_string_t const false_string = 
{
	.dyn_str = FALSE,
	.dyn_esc = FALSE,
	.key_esc = FALSE,
	.str_len = 5,
	.str = "false",
	.esc_len = 0,
	.esc = NULL
};
llsd_string_t const true_string = 
{
	.dyn_str = FALSE,
	.dyn_esc = FALSE,
	.key_esc = FALSE,
	.str_len = 4,
	.str = "true",
	.esc_len = 0,
	.esc = NULL
};

static uint8_t zero_data [] = { '0' };
llsd_binary_t const false_binary =
{
	.dyn_data = FALSE,
	.dyn_enc = FALSE,
	.encoding = LLSD_NONE,
	.data_size = 1,
	.data = zero_data,
	.enc_size = 0,
	.enc = NULL
};
static uint8_t one_data[] = { '1' };
llsd_binary_t const true_binary =
{
	.dyn_data = FALSE,
	.dyn_enc = FALSE,
	.encoding = LLSD_NONE,
	.data_size = 1,
	.data = one_data,
	.enc_size = 0,
	.enc = NULL
};
llsd_binary_t const empty_binary =
{
	.dyn_data = FALSE,
	.dyn_enc = FALSE,
	.encoding = LLSD_NONE,
	.data_size = 0,
	.data = NULL,
	.enc_size = 0,
	.enc = NULL
};
llsd_uri_t const empty_uri = 
{
	.dyn_uri = FALSE,
	.dyn_esc = FALSE,
	.uri_len = 0,
	.uri = "",
	.esc_len = 0,
	.esc = NULL
};
llsd_date_t const empty_date =
{
	.use_dval = TRUE,
	.dyn_str = FALSE,
	.len = 0,
	.dval = 0.0,
	.str = NULL
};
llsd_array_t const empty_array =
{
	.array = {
		.pfn = NULL,
		.num_nodes = 0,
		.buffer_size = 0,
		.data_head = -1,
		.free_head = -1,
		.node_buffer = NULL
	}
};
llsd_map_t const empty_map =
{
	.ht = {
		.khfn = NULL,
		.kefn = NULL,
		.kdfn = NULL,
		.vdfn = NULL,
		.prime_index = 0,
		.num_tuples = 0,
		.initial_capacity = 0,
		.load_factor = 0.0f,
		.tuples = NULL
	}
};
