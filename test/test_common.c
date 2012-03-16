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

/* NOTE: this file is included inline inside of the test_binary.c,
 * test_notation.c, and test_xml.c files.  they use whatever serializer
 * deserializer functions are specified when the suites are initialized.
 */

extern FILE* tmpf;
extern llsd_serializer_t format;

#if 0
static llsd_type_t get_random_llsd_type( void )
{
	return (llsd_type_t)(rand() % LLSD_TYPE_COUNT);
}

static llsd_t* get_random_str( void )
{
	static uint8_t str[1024];
	int i;
	int len = (24 + (rand() % 1000));

	for ( i = 0; i < len; i++ )
	{
		/* get a random, printable ascii character */
		str[i] = (32 + (rand() % 94));
	}
	str[len] = '\0';
	return llsd_new( LLSD_STRING, str, FALSE );
}

static llsd_t* get_random_uri( void )
{
	static uint8_t uri[1024];
	int i;
	int len = (24 + (rand() % 1000));

	for ( i = 0; i < len; i++ )
	{
		/* get a random, printable ascii character */
		uri[i] = (32 + (rand() % 94));
	}
	uri[len] = '\0';
	return llsd_new( LLSD_URI, uri );
}

static llsd_t* get_random_bin( void )
{
	static uint8_t bin[1024];
	int i;
	int len = (rand() % 1024);

	for ( i = 0; i < len; i++ )
	{
		/* get a random byte*/
		bin[i] = (uint8_t)(rand() % 256);
	}
	return llsd_new( LLSD_BINARY, len, bin );
}

static llsd_t* get_random_uuid( void )
{
	static uint8_t bits[UUID_LEN];
	int i;

	for ( i = 0; i < UUID_LEN; i++ )
	{
		/* get a random byte*/
		bits[i] = (uint8_t)(rand() % 256);
	}
	return llsd_new( LLSD_UUID, bits );
}

/* forward declaration */
static llsd_t * get_random_map( uint32_t size );

static llsd_t * get_random_array( uint32_t size )
{
	uint32_t total = 0;
	uint32_t s = 0;
	llsd_type_t type_;

	/* create the array */
	llsd_t * arr = llsd_new( LLSD_ARRAY );

	/* now populate it with random data */
	while( total < size )
	{
		type_ = get_random_llsd_type();

		switch( type_ )
		{
			case LLSD_UNDEF:
				llsd_array_append( arr, llsd_new( type_ ) );
				total++;
				break;
			case LLSD_BOOLEAN:
				llsd_array_append( arr, llsd_new( type_, (rand() % 2) ) );
				total++;
				break;
			case LLSD_INTEGER:
				llsd_array_append( arr, llsd_new( type_, rand() ) );
				total++;
				break;
			case LLSD_REAL:
				llsd_array_append( arr, llsd_new( type_, 1.0 * rand() ) );
				total++;
				break;
			case LLSD_UUID:
				llsd_array_append( arr, get_random_uuid() );
				total++;
				break;
			case LLSD_STRING:
				llsd_array_append( arr, get_random_str() );
				total++;
				break;
			case LLSD_DATE:
				llsd_array_append( arr, llsd_new( type_, (1.0 * rand()) ) );
				total++;
				break;
			case LLSD_URI:
				llsd_array_append( arr, get_random_uri() );
				total++;
				break;
			case LLSD_BINARY:
				llsd_array_append( arr, get_random_bin() );
				total++;
				break;
			case LLSD_ARRAY:	
				s = (rand() % (size - total));
				llsd_array_append( arr, get_random_array( s ) );
				if ( s == 0 )
				{
					total++;
				}
				else
				{
					total += s;
				}
				break;
			case LLSD_MAP:
				s = (rand() % (size - total));
				llsd_array_append( arr, get_random_map( s ) );
				if ( s == 0 )
				{
					total++;
				}
				else
				{
					total += s;
				}
			break;
		}
	}

	return arr;
}

static llsd_t * get_random_map( uint32_t size )
{
	uint32_t total = 0;
	uint32_t s = 0;
	llsd_type_t type_;
	llsd_t * map;
	llsd_t * key;

	/* create the array */
	map = llsd_new( LLSD_MAP );

	/* now populate it with random data */
	while( total < size )
	{
		/* get a random type */
		type_ = get_random_llsd_type();

		/* get a random key */
		key = get_random_str();

		switch( type_ )
		{
			case LLSD_UNDEF:
				llsd_map_insert( map, key, llsd_new( type_ ) );
				total++;
				break;
			case LLSD_BOOLEAN:
				llsd_map_insert( map, key, llsd_new( type_, (rand() % 2) ) );
				total++;
				break;
			case LLSD_INTEGER:
				llsd_map_insert( map, key, llsd_new( type_, rand() ) );
				total++;
				break;
			case LLSD_REAL:
				llsd_map_insert( map, key, llsd_new( type_, 1.0 * rand() ) );
				total++;
				break;
			case LLSD_UUID:
				llsd_map_insert( map, key, get_random_uuid() );
				total++;
				break;
			case LLSD_STRING:
				llsd_map_insert( map, key, get_random_str() );
				total++;
				break;
			case LLSD_DATE:
				llsd_map_insert( map, key, llsd_new( type_, 1.0 * rand() ) );
				total++;
				break;
			case LLSD_URI:
				llsd_map_insert( map, key, get_random_uri() );
				total++;
				break;
			case LLSD_BINARY:
				llsd_map_insert( map, key, get_random_bin() );
				total++;
				break;
			case LLSD_ARRAY:	
				s = (rand() % (size - total));
				llsd_map_insert( map, key, get_random_array( s ) );
				if ( s == 0 )
				{
					total++;
				}
				else
				{
					total += s;
				}
				break;
			case LLSD_MAP:
				s = (rand() % (size - total));
				llsd_map_insert( map, key, get_random_map( s ) );
				if ( s == 0 )
				{
					total++;
				}
				else
				{
					total += s;
				}
				break;
		}
	}
	return map;
}

static llsd_t * get_random_llsd( uint32_t size, uint32_t seed )
{
	llsd_t * llsd;

	/* set the seed */
	srand( seed );

	if ( rand() % 2 )
	{
		return get_random_map( size );
	}
	else
	{
		return get_random_array( size );
	}
}



uint8_t const bits[UUID_LEN] = { 1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6 };
int8_t const * const str = T("Hello World!");
int8_t const * const url = T("http://www.ixquick.com");

static llsd_t * get_llsd( llsd_type_t type_ )
{
	llsd_t * llsd = NULL;

	/* construct the llsd */
	switch( type_ )
	{
		case LLSD_BOOLEAN:
		case LLSD_INTEGER:
			llsd = llsd_new( type_, 1 );
			break;

		case LLSD_REAL:
		case LLSD_DATE:
			llsd = llsd_new( type_, 1.0 );
			break;

		case LLSD_UUID:
			llsd = llsd_new( type_, bits );
			break;

		case LLSD_STRING:
			llsd = llsd_new( type_, str, strlen( str ) );
			break;

		case LLSD_URI:
			llsd = llsd_new( type_, url );
			break;

		case LLSD_BINARY:
			llsd = llsd_new( type_, UUID_LEN, bits );
			break;

		case LLSD_UNDEF:
		case LLSD_ARRAY:
		case LLSD_MAP:
			llsd = llsd_new( type_ );
			break;
	}

	CU_ASSERT_PTR_NOT_NULL( llsd );
	CU_ASSERT_EQUAL( type_, llsd_get_type( llsd ) );

	return llsd;
}
#endif



#if 0
static void test_newdel( void )
{
	int i;
	llsd_t* llsd;
	llsd_type_t type_;
	size_t heap_size;

	for ( type_ = LLSD_TYPE_FIRST; type_ < LLSD_TYPE_LAST; type_++ )
	{
		/* take a measure of the heap size */
		heap_size = get_heap_size();

		/* construct the llsd */
		llsd = get_llsd( type_ );
		CU_ASSERT_PTR_NOT_NULL( llsd );

		/* check the type */
		CU_ASSERT_EQUAL( type_, llsd_get_type( llsd ) );

		/* delete the llsd */
		llsd_delete( llsd );
		llsd = NULL;

		/* check the heap size again */
		CU_ASSERT_EQUAL( heap_size, get_heap_size() );
	}
}

#define BUF_SIZE (4096)
static void test_serialization( void )
{
	llsd_t* llsd;
	llsd_type_t type_;
	size_t heap_size;
	size_t s = 0;
	uint8_t * buf = UT(CALLOC( BUF_SIZE, sizeof(uint8_t) ));
	CU_ASSERT_PTR_NOT_NULL_FATAL( buf );

	for ( type_ = LLSD_TYPE_FIRST; type_ < LLSD_TYPE_LAST; type_++ )
	{
		heap_size = get_heap_size();

		tmpf = fmemopen( buf, BUF_SIZE, "w+b" );
		CU_ASSERT_PTR_NOT_NULL_FATAL( tmpf );

		/* construct the llsd */
		llsd = get_llsd( type_ );

		/* check the type */
		CU_ASSERT( type_ == llsd_get_type( llsd ) );

		/* serialize it to the file */
		s = llsd_format( llsd, format, tmpf, FALSE );
		fclose( tmpf );
		tmpf = NULL;

		/* delete the llsd */
		llsd_delete( llsd );
		llsd = NULL;

		CU_ASSERT_EQUAL( heap_size, get_heap_size() );

		/* check that the correct number of bytes were written */
		if ( expected_sizes[type_] != (s - data_offset) )
		{
			WARN("type: %s, expected: %d, actual: %d\n", llsd_get_type_string( type_ ), expected_sizes[type_], (s - data_offset) ); 
			CU_FAIL();
		}

		/* check that the expected data was written */
		CU_ASSERT( 0 == memcmp( &(buf[data_offset]), expected_data[ type_ ], (s - data_offset) ) );

		heap_size = get_heap_size();

		/* reopen the buffer */
		tmpf = fmemopen( buf, BUF_SIZE, "r+b" );
		CU_ASSERT_PTR_NOT_NULL_FATAL( tmpf );

		/* try to deserialize the llsd */
		llsd = llsd_parse( tmpf );
		CU_ASSERT_PTR_NOT_NULL_FATAL( llsd );

		/* check the type */
		CU_ASSERT( type_ == llsd_get_type( llsd ) );

		/* clean up */
		fclose( tmpf );
		tmpf = NULL;
		llsd_delete( llsd );
		llsd = NULL;
		memset( buf, 0, s );
		s = 0;

		CU_ASSERT_EQUAL( heap_size, get_heap_size() );
	}
}

static void test_random_serialize( void )
{
	const uint32_t seed = 0xDEADBEEF;
	const uint32_t size = 4096;
	llsd_t * llsd_out = NULL;
	llsd_t * llsd_in = NULL;

	/* get the initial heap size */
	size_t heap_size = get_heap_size();

	WARN(" >>>>>>>>>>>>>>>>>>\n " );

	/* generate a repeatable, random llsd object */
	llsd_out = get_random_llsd( size, seed );
	CU_ASSERT_PTR_NOT_NULL_FATAL( llsd_out );

	tmpf = fopen( "test.llsd", "w+b" );
	CU_ASSERT_PTR_NOT_NULL_FATAL( tmpf );
	
	llsd_format( llsd_out, format, tmpf, FALSE );
	fclose( tmpf );
	tmpf = NULL;

	WARN(" <<<<<<<<<<<<<<<<<<\n " );

	tmpf = fopen( "test.llsd", "r+b" );
	CU_ASSERT_PTR_NOT_NULL_FATAL( tmpf );

	llsd_in = llsd_parse( tmpf );
	CU_ASSERT_PTR_NOT_NULL_FATAL( llsd_in );

	fclose( tmpf );
	tmpf = NULL;

	/* make sure the two llsd structures are equivilent */
	CU_ASSERT( llsd_equal( llsd_out, llsd_in ) );

	llsd_delete( llsd_out );
	llsd_out = NULL;
	llsd_delete( llsd_in );
	llsd_in = NULL;

	CU_ASSERT_EQUAL( heap_size, get_heap_size() );
}
#endif

static CU_pSuite add_tests( CU_pSuite pSuite )
{
#if 0
	CHECK_PTR_RET( CU_add_test( pSuite, "new/delete of all types", test_newdel), NULL );
	CHECK_PTR_RET( CU_add_test( pSuite, "serialization of all types", test_serialization), NULL );
	CHECK_PTR_RET( CU_add_test( pSuite, "serialization of random llsd", test_random_serialize), NULL );
#endif
	return pSuite;
}

