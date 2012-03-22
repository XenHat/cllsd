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

#include <stdint.h>

#include "macros.h"
#include "llsd.h"

int8_t const * const llsd_type_strings[LLSD_TYPE_COUNT] =
{
	T("UNDEF"),
	T("BOOLEAN"),
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

int8_t const * const llsd_bin_enc_type_strings[LLSD_BIN_ENC_COUNT] =
{
	T("NONE"),
	T("base16"),
	T("base64"),
	T("base85")
};


