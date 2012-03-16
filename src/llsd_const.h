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

#ifndef LLSD_CONST_H
#define LLSD_CONST_H

#include <stdint.h>

typedef enum llsd_type_e
{
	LLSD_UNDEF,
	LLSD_BOOLEAN_TRUE,
	LLSD_BOOLEAN_FALSE,
	LLSD_INTEGER,
	LLSD_REAL,
	LLSD_UUID,
	LLSD_STRING,
	LLSD_DATE,
	LLSD_URI,
	LLSD_BINARY,
	LLSD_ARRAY,
	LLSD_MAP,

	LLSD_TYPE_LAST,
	LLSD_TYPE_FIRST = LLSD_UNDEF,
	LLSD_TYPE_COUNT = LLSD_TYPE_LAST - LLSD_TYPE_FIRST,
	LLSD_TYPE_INVALID

} llsd_type_t;

typedef enum llsd_serializer_s
{
	LLSD_ENC_XML,
	LLSD_ENC_BINARY,

	LLSD_ENC_LAST,
	LLSD_ENC_FIRST = LLSD_ENC_XML,
	LLSD_ENC_COUNT = LLSD_ENC_LAST - LLSD_ENC_FIRST

} llsd_serializer_t;

typedef enum llsd_bin_enc_s
{
	LLSD_NONE,
	LLSD_BASE16,
	LLSD_BASE64,

	LLSD_BIN_ENC_LAST,
	LLSD_BIN_ENC_FIRST = LLSD_NONE,
	LLSD_BIN_END_COUNT = LLSD_BIN_ENC_LAST - LLSD_BIN_ENC_FIRST

} llsd_bin_enc_t;

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

/* LLSD types */
#define UUID_LEN (16)
#define UUID_STR_LEN (36)

extern int8_t const * const llsd_type_strings[LLSD_TYPE_COUNT];

#define TYPE_TO_STRING( t ) (((t >= LLSD_TYPE_FIRST) && (t < LLSD_TYPE_LAST)) ? \
							 llsd_type_strings[t] : T("INVALID") )

#endif /* LLSD_CONST_H */
