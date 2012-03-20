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
#include <stdlib.h>
#include <sys/uio.h>
#include <expat.h>

#include "debug.h"
#include "macros.h"
#include "array.h"
#include "llsd.h"
#include "llsd_util.h"
#include "llsd_xml.h"

static llsd_bin_enc_t llsd_bin_enc_from_attr( char const * attr )
{
	llsd_bin_enc_t enc = LLSD_BASE64;
	CHECK_PTR_RET( attr, LLSD_BASE64 );
	CHECK_RET( attr[0] == 'b', LLSD_BASE64 );

	/* base64, base16, base85 */
	switch( attr[4] )
	{
		case '6':
			return LLSD_BASE64;
		case '1':
			return LLSD_BASE16;
		case '8':
			return LLSD_BASE85;
	}
	return LLSD_BASE64;
}

static llsd_type_t llsd_type_from_tag( char const * tag )
{
	CHECK_PTR_RET( tag, LLSD_TYPE_INVALID );
	switch( tag[0] )
	{
		case 'a':
			return LLSD_ARRAY;
		/* boolean, binary */
		case 'b':
			switch ( tag[1] )
			{
				case 'o':
					return LLSD_BOOLEAN;
				case 'i':
					return LLSD_BINARY;
			}
		case 'd':
			return LLSD_DATE;
		case 'i':
			return LLSD_INTEGER;
		case 'k':
			return LLSD_KEY;
		case 'l':
			return LLSD_LLSD;
		case 'm':
			return LLSD_MAP;
		case 'r':
			return LLSD_REAL;
		case 's':
			return LLSD_STRING;
		/* undef, uri, uuid */
		case 'u':
			switch ( tag[1] )
			{
				case 'n':
					return LLSD_UNDEF;
				case 'r':
					return LLSD_URI;
				case 'u':
					return LLSD_UUID;
			}
	}
	return LLSD_TYPE_INVALID;
}

typedef struct context_s
{
	array_t * containers;
	array_t * params;
	char * buf;
	size_t len;
} context_t;

static void XMLCALL llsd_xml_start_tag( void * data, char const * el, char const ** attr )
{
	context_t * ctx = (context_t*)data;
	llsd_t * cur_container = (llsd_t*)array_get_tail( ctx->containers );
	llsd_t * key = NULL;
	llsd_t * llsd = NULL;
	llsd_t * container = NULL;
	llsd_bin_enc_t enc = LLSD_BASE64;
	llsd_type_t t;
	int size = 0;

	/* get the type */
	t = llsd_type_from_tag( el );

	if ( ( t == LLSD_LLSD ) || ( t == LLSD_KEY ) )
		return;

	
	switch( t )
	{
		case LLSD_LLSD:
		case LLSD_KEY:
		case LLSD_UNDEF:
		case LLSD_BOOLEAN:
		case LLSD_INTEGER:
		case LLSD_REAL:
		case LLSD_UUID:
		case LLSD_DATE:
		case LLSD_STRING:
		case LLSD_URI:
			return;
		case LLSD_BINARY:
			/* try to get the encoding attribute if there is one */
			if ( strncmp( attr[0], "encoding", 9 ) == 0 )
			{
				enc = llsd_bin_enc_from_attr( attr[1] );
			}
			llsd = llsd_new( LLSD_TYPE_INVALID );
			llsd->type_ = LLSD_BINARY;
			llsd->binary_.encoding = enc;
			/* put it on the params stack */
			array_push_tail( ctx->params, (void*)llsd );
			return;
		case LLSD_ARRAY:
			/* try to get the size attribute if there is one */
			if ( strncmp( attr[0], "size", 5 ) == 0 )
			{
				size = atoi( attr[1] );
			}

			/* create the new array container */
			container = llsd_new_array( size );
			break;
		case LLSD_MAP:
			/* try to get the size attribute if there is one */
			if ( strncmp( attr[0], "size", 5 ) == 0 )
			{
				size = atoi( attr[1] );
			}

			/* create the new map container */
			container = llsd_new_map( size );
			break;
	}

	if ( cur_container != NULL )
	{
		if ( llsd_is_array( cur_container ) )
		{
			/* add the new container to the current one */
			llsd_array_append( cur_container, container );
		}
		else if ( llsd_is_map( cur_container ) )
		{
			CHECK_MSG( array_size( ctx->params ) > 0, "no key on params stack\n" );

			/* get the key */
			key = (llsd_t*)array_pop_tail( ctx->params );

			/* add the new map to the current one */
			llsd_map_insert( cur_container, key, container );
		}
		else
		{
			WARN("invalid container type\n");
		}
	}

	/* push it onto the container stack */
	array_push_tail( ctx->containers, (void*)container );
}

static void XMLCALL llsd_xml_end_tag( void * data, char const * el )
{
	context_t * ctx = (context_t*)data;
	llsd_t * container = (llsd_t*)array_get_tail( ctx->containers );
	llsd_t * key = NULL;
	llsd_t * llsd = NULL;
	const char * value = ctx->buf;
	int32_t v1 = 0;
	double v2 = 0;
	int len = 0;
	llsd_bin_enc_t enc;
	llsd_type_t t;

	/* get the type */
	t = llsd_type_from_tag( el );

	if ( t == LLSD_LLSD )
		return;

	switch( t )
	{
		case LLSD_KEY:
			llsd = llsd_new( LLSD_TYPE_INVALID );
			llsd->type_ = LLSD_STRING;
			/* take ownership of the key str */
			llsd->uuid_.str = (uint8_t*)value;
			llsd->uuid_.dyn_str = TRUE;
			ctx->buf = NULL;
			array_push_tail( ctx->params, (void*)llsd );
			return;
		case LLSD_UNDEF:
			llsd = llsd_new( LLSD_UNDEF );
			break;
		case LLSD_BOOLEAN:
			if ( (value == NULL) ||
				 (memcmp( value, "false", len ) == 0) ||
				 (memcmp( value, "0", len ) == 0) )
			{
				llsd = llsd_new_boolean( FALSE );
			}
			else
			{
				llsd = llsd_new_boolean( TRUE );
			}
			break;
		case LLSD_INTEGER:
			v1 = atoi( value );
			llsd = llsd_new_integer( v1 );
			break;
		case LLSD_REAL:
			v2 = strtod( value, NULL );
			llsd = llsd_new_real( v2 );
			break;
		case LLSD_UUID:
			llsd = llsd_new( LLSD_TYPE_INVALID );
			llsd->type_ = LLSD_UUID;
			/* take ownership of the UUID str */
			llsd->uuid_.str = (uint8_t*)value;
			llsd->uuid_.dyn_str = TRUE;
			ctx->buf = NULL;
			break;
		case LLSD_DATE:
			llsd = llsd_new( LLSD_TYPE_INVALID );
			llsd->type_ = LLSD_DATE;
			/* take ownership of the date str */
			llsd->date_.str = (uint8_t*)value;
			llsd->date_.dyn_str = TRUE;
			ctx->buf = NULL;
			llsd->date_.use_dval = FALSE;
			llsd->date_.dval = 0.0;
			llsd->date_.len = strlen( llsd->date_.str ) + 1;
			break;
		case LLSD_STRING:
			llsd = llsd_new( LLSD_TYPE_INVALID );
			llsd->type_ = LLSD_STRING;
			/* take ownership of the string str */
			llsd->string_.str = (uint8_t*)value;
			llsd->string_.str_len = strlen( value ) + 1;
			llsd->string_.dyn_str = TRUE;
			break;
		case LLSD_URI:
			llsd = llsd_new( LLSD_TYPE_INVALID );
			llsd->type_ = LLSD_URI;
			/* take ownership of the uri str */
			llsd->uri_.uri = (uint8_t*)value;
			llsd->uri_.uri_len = strlen( value ) + 1;
			llsd->uri_.dyn_uri = TRUE;
			break;
		case LLSD_BINARY:
			llsd = (llsd_t*)array_pop_tail( ctx->params );
			/* take ownership of the encoded binary str */
			llsd->binary_.enc = (uint8_t*)value;
			llsd->binary_.enc_size = strlen( value ) + 1;
			llsd->binary_.dyn_enc = TRUE;
			break;
		case LLSD_ARRAY:
		case LLSD_MAP:
			/* done filling the container so remove it from the containers stack */
			array_pop_tail( ctx->containers );
			CHECK_MSG( ctx->buf == NULL, "context buffer has data when it shouldn't\n" );
			return;
	}

	/* add the completed llsd object to the current container */
	if ( container != NULL )
	{
		if ( llsd_get_type( container ) == LLSD_ARRAY )
		{
			llsd_array_append( container, llsd );
		}
		else
		{
			key = (llsd_t*)array_pop_tail( ctx->params );
			llsd_map_insert( container, key, llsd );
		}
	}
	else
	{
		/* standalone llsd object, no outer container */
		array_push_tail( ctx->containers, (void*)llsd );
	}

	/* clean up the buffer if needed */
	if ( ctx->buf != NULL )
	{
		FREE( ctx->buf );
		ctx->buf = NULL;
	}
}

static void XMLCALL llsd_xml_data_handler( void * data, char const * s, int len )
{
	context_t * ctx = (context_t*)data;
	CHECK_PTR( s );
	CHECK( len > 0 );

	/* copy the node data into the context buffer */
	if ( ctx->buf != NULL )
	{
		ctx->buf = REALLOC( ctx->buf, ctx->len + len );
		MEMCPY( &(ctx->buf[ctx->len]), s, len );
		ctx->len += len;
	}
	else
	{
		ctx->buf = CALLOC( len, sizeof(uint8_t) );
		MEMCPY( ctx->buf, s, len );
		ctx->len = len;
	}
}

#define XML_BUF_SIZE (4096)
llsd_t * llsd_parse_xml( FILE * fin )
{
	XML_Parser p;
	int done;
	size_t len = 0;
	static uint8_t buf[XML_BUF_SIZE];
	llsd_t * llsd = NULL;
	context_t ctx;
	CHECK_PTR_RET( fin, NULL );

	/* create the parser */
	p = XML_ParserCreate("UTF-8");
	CHECK_PTR_RET_MSG( p, NULL, "failed to create expat parser\n" );

	/* set the tag handlers */
	XML_SetElementHandler( p, llsd_xml_start_tag, llsd_xml_end_tag );
	XML_SetCharacterDataHandler( p, llsd_xml_data_handler );

	/* set up the stack for context */
	ctx.containers = array_new( 8, NULL );
	ctx.params = array_new( 3, NULL );
	ctx.buf = NULL;
	ctx.len = 0;
	XML_SetUserData( p, (void*)(&ctx) );

	/* read the file and parse it */
	do
	{
		len = fread( buf, sizeof(uint8_t), XML_BUF_SIZE, fin );
		done = (len < XML_BUF_SIZE);

		if ( XML_Parse( p, buf, len, done ) == XML_STATUS_ERROR )
		{
			DEBUG( "%s\n", XML_ErrorString(XML_GetErrorCode(p)) );
		}

	} while ( !done );

	if ( array_size( ctx.containers ) != 1 )
	{
		DEBUG( "too many llsd objects left after parsing XML\n" );
	}

	/* get the resulting LLSD object */
	llsd = array_pop_tail( ctx.containers );

	/* free up the memeory */
	if ( ctx.buf != NULL )
	{
		FREE( ctx.buf );
	}
	array_delete( ctx.containers );
	array_delete( ctx.params );

	/* free the parser */
	XML_ParserFree( p );

	return NULL;
}

#define XML_UNDEF_LEN (9)
#define XML_UNDEFC_LEN (0)
#define XML_BOOLEAN_LEN (9)
#define XML_BOOLEANC_LEN (10)
#define XML_INTEGER_LEN (9)
#define XML_INTEGERC_LEN (10)
#define XML_REAL_LEN (6)
#define XML_REALC_LEN (7)
#define XML_UUID_LEN (6)
#define XML_UUIDC_LEN (7)
#define XML_STRING_LEN (14)
#define XML_STRINGC_LEN (9)
#define XML_DATE_LEN (6)
#define XML_DATEC_LEN (7)
#define XML_URI_LEN (11)
#define XML_URIC_LEN (6)
#define XML_BINARY_LEN (14)
#define XML_BINARYC_LEN (9)
#define XML_ARRAY_LEN (13)
#define XML_ARRAYC_LEN (8)
#define XML_MAP_LEN (11)
#define XML_MAPC_LEN (6)
#define XML_KEY_LEN (5)
#define XML_KEYC_LEN (6)
static uint8_t const * const lxkey		= "<key>";
static uint8_t const * const lxkeyc		= "</key>\n";
static uint8_t const * const lxtabs		= "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

static uint8_t const * const lxstart[LLSD_TYPE_COUNT] =
{
	"<undef />",
	"<boolean>",
	"<integer>",
	"<real>",
	"<uuid>",
	"<string size=\"",
	"<date>",
	"<uri size=\"",
	"<binary size=\"",
	"<array size=\"",
	"<map size=\""
};

#define XML_SIZEC_LEN (2)
static uint8_t const * const lxstartc = "\">\n";

static uint8_t const * const lxend[LLSD_TYPE_COUNT] =
{
	"\n",
	"</boolean>\n",
	"</integer>\n",
	"</real>\n",
	"</uuid>\n",
	"</string>\n",
	"</date>\n",
	"</uri>\n",
	"</binary>\n",
	"</array>\n",
	"</map>\n"
};

static uint32_t lxstartlen[LLSD_TYPE_COUNT] =
{
	XML_UNDEF_LEN,
	XML_BOOLEAN_LEN,
	XML_INTEGER_LEN,
	XML_REAL_LEN,
	XML_UUID_LEN,
	XML_STRING_LEN,
	XML_DATE_LEN,
	XML_URI_LEN,
	XML_BINARY_LEN,
	XML_ARRAY_LEN,
	XML_MAP_LEN
};

static uint32_t lxendlen[LLSD_TYPE_COUNT] =
{
	XML_UNDEFC_LEN,
	XML_BOOLEANC_LEN,
	XML_INTEGERC_LEN,
	XML_REALC_LEN,
	XML_UUIDC_LEN,
	XML_STRINGC_LEN,
	XML_DATEC_LEN,
	XML_URIC_LEN,
	XML_BINARYC_LEN,
	XML_ARRAYC_LEN,
	XML_MAPC_LEN
};

static uint32_t indent = 1;

#define indent_xml(p,f) ( p ? fwrite(lxtabs, sizeof(uint8_t), ((indent<=255)?indent:255), f) : 0 );

static size_t llsd_write_xml_start_tag( llsd_type_t t, FILE * fout, int pretty, int size)
{
	static uint8_t buf[32];
	int sz = 0;
	size_t num = 0;
	CHECK_RET( (t >= LLSD_TYPE_FIRST) && (t < LLSD_TYPE_LAST), 0 );

	switch ( t )
	{
		case LLSD_UNDEF:
		case LLSD_BOOLEAN:
		case LLSD_INTEGER:
		case LLSD_REAL:
		case LLSD_UUID:
		case LLSD_DATE:
			return fwrite( lxstart[t], sizeof(uint8_t), lxstartlen[t], fout );
		case LLSD_STRING:
		case LLSD_URI:
		case LLSD_BINARY:
			sz = snprintf(buf, 32, "%d", size);
			num += fwrite( lxstart[t], sizeof(uint8_t), lxstartlen[t], fout );
			num += fwrite( buf, sizeof(uint8_t), sz, fout );
			num += fwrite( lxstartc, sizeof(uint8_t), XML_SIZEC_LEN, fout );
			return num;
		case LLSD_ARRAY:
		case LLSD_MAP:
			sz = snprintf(buf, 32, "%d", size);
			num += fwrite( lxstart[t], sizeof(uint8_t), lxstartlen[t], fout );
			num += fwrite( buf, sizeof(uint8_t), sz, fout );
			num += fwrite( lxstartc, sizeof(uint8_t), XML_SIZEC_LEN + (pretty ? 1 : 0), fout );
			return num;
	}
	return num;
}

static size_t llsd_write_xml_end_tag( llsd_type_t t, FILE * fout, int pretty )
{
	CHECK_RET( (t >= LLSD_TYPE_FIRST) && (t < LLSD_TYPE_LAST), 0 );
	return fwrite( lxend[t], sizeof(uint8_t), lxendlen[t] + (pretty ? 1 : 0), fout );
}


size_t llsd_format_xml( llsd_t * llsd, FILE * fout, int pretty )
{
	size_t num = 0;
	unsigned long start = ftell( fout );

	llsd_itr_t itr;
	llsd_string_t s;
	llsd_t *k, *v;
	llsd_type_t t = llsd_get_type( llsd );

	switch ( t )
	{
		case LLSD_UNDEF:
		case LLSD_BOOLEAN:
		case LLSD_INTEGER:
		case LLSD_REAL:
		case LLSD_UUID:
		case LLSD_DATE:
			s = llsd_as_string( llsd );
			num += indent_xml( pretty, fout );
			num += llsd_write_xml_start_tag( t, fout, pretty, 0 );
			num += fwrite( s.str, sizeof(uint8_t), s.str_len, fout );
			num += llsd_write_xml_end_tag( t, fout, pretty );
			DEBUG( "%*s%s %lu - %lu\n", indent * 4, " ", llsd_get_type_string( t ), start, ftell( fout ) - 1 );
			return num;
	
		case LLSD_STRING:
		case LLSD_URI:
		case LLSD_BINARY:
			s = llsd_as_string( llsd );
			num += indent_xml( pretty, fout );
			num += llsd_write_xml_start_tag( t, fout, pretty, s.str_len );
			num += fwrite( s.str, sizeof(uint8_t), s.str_len, fout );
			num += llsd_write_xml_end_tag( t, fout, pretty );
			DEBUG( "%*s%s %lu - %lu\n", indent * 4, " ", llsd_get_type_string( t ), start, ftell( fout ) - 1 );
			return num;

		case LLSD_ARRAY:
		case LLSD_MAP:
			num += indent_xml( pretty, fout );
			num += llsd_write_xml_start_tag( t, fout, pretty, llsd_get_size( llsd ) );
			DEBUG( "%*s%s (%d)\n", indent * 4, " ", llsd_get_type_string( t ), llsd_get_size( llsd ) );
			indent++;
			itr = llsd_itr_begin( llsd );
			for ( ; itr != llsd_itr_end( llsd ); itr = llsd_itr_next( llsd, itr ) )
			{
				llsd_itr_get( llsd, itr, &v, &k );
				if ( k != NULL )
				{
					s = llsd_as_string( k );
					num += indent_xml( pretty, fout );
					num += fwrite( lxkey, sizeof(uint8_t), XML_KEY_LEN, fout );
					num += fwrite( s.str, sizeof(uint8_t), s.str_len, fout );
					num += fwrite( lxkeyc, sizeof(uint8_t), XML_KEYC_LEN + (pretty ? 1 : 0), fout );
				}
				num += llsd_format_xml( v, fout, pretty );
			}
			indent--;
			num += indent_xml( pretty, fout );
			num += llsd_write_xml_end_tag( t, fout, pretty );
			DEBUG( "%*s%s %lu - %lu\n", indent * 4, " ", llsd_get_type_string( t ), start, ftell( fout ) - 1 );
			break;
	}
	return num;

}

size_t llsd_get_xml_zero_copy_size( llsd_t * llsd, int pretty )
{
	FAIL( "XML serialization doesn't support zero copy\n" );
	return 0;
}

size_t llsd_format_xml_zero_copy( llsd_t * llsd, struct iovec * v, int pretty )
{
	FAIL( "XML serialization doesn't support zero copy\n" );
	return 0;
}

