/**
 * @file test_interpolator2d.c
 * @author Alejandro Moreno (amoreno@veng.com.ar)
 * @brief A continuación se implementan diversos testeos asociados a la
 * API de interpolación bi-dimensional
 * 
 */

/******************************************************************************
 *  Inclusions (library's order: standard C, others, user header files)
 *****************************************************************************/
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include "unity.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include "interpolator2D.h"

/******************************************************************************
 *  Inclusions of private function dependencies
 *****************************************************************************/


/******************************************************************************
 *  Definitions of private data types (order: Enums, Typedefs)
 *****************************************************************************/


/******************************************************************************
 *  Definition macros of private constants
 *****************************************************************************/
#define DEG2RAD( x ) ( x * ( ( 2 * 3.14159265358979323846 ) / 360 ) )

static const uint32_t NUM_X_POINTS  = 2;
static const uint32_t NUM_Y_POINTS  = 2;

static const double LONGITUDE_INITIAL = DEG2RAD( -180 );
static const double LATITUDE_INITIAL  = DEG2RAD( -90  );
static const double LONGITUDE_FINAL   = DEG2RAD(  180 );
static const double LATITUDE_FINAL    = DEG2RAD(  90  );

static const double STEP_LONGITUDE    = DEG2RAD(  2.5 );
static const double STEP_LATITUDE     = DEG2RAD(  2.5 );

static const double EPSILON  = 1e-4;
static const double EPSILON1 = 1e-3; 
static const double EPSILON2 = 1e-2; 

/******************************************************************************
 *  Private function-like macros
 *****************************************************************************/

/******************************************************************************
 *  Definitions of external public global variables
 *****************************************************************************/

/******************************************************************************
 *  Definitions of public global variables
 *****************************************************************************/

/******************************************************************************
 *  Definitions of private global variables
 *****************************************************************************/
static gsl_vector *x, *y;
static gsl_matrix *f_xy;

/******************************************************************************
 *  Prototypes (declarations) of private functions  
 *****************************************************************************/

/**
 * @brief Función generadora de datos para validar el funcionamiento de la API
 * de interpolación bi-dimensional
 * 
 * @param lon 
 * @param lat 
 * @return double Valor de la función evaluada en (lon, lat)
 */
 static double _Function1( double lon, double lat );
 
 /**
 * @brief Función generadora de datos para validar el funcionamiento de la API
 * de interpolación bi-dimensional
 * 
 * @param lon 
 * @param lat 
 * @return double Valor de la función evaluada en (lon, lat)
 */
 static double _Function2( double lon, double lat ); 

/**
 * @brief Función que valida la interpolación realizada
 * 
 * @param value Valor obtenido mediante la interpolación
 * @param analytic Valor esperado
 * @param error_limit Limite de error admisible
 * @return true     Validación exitosa
 * @return false    Error
 */
 static bool _ValidateInterpolation( double value, double analytic, double error_limit );

/******************************************************************************
 *  Implementations of public functions
 *****************************************************************************/

void setUp(void)
{
    x = gsl_vector_alloc( NUM_X_POINTS );
    y = gsl_vector_alloc( NUM_Y_POINTS );
    f_xy = gsl_matrix_alloc( NUM_X_POINTS, NUM_Y_POINTS);

    gsl_vector_set( x, 0, 0 );
    gsl_vector_set( x, 1, 1 );

    gsl_vector_set( y, 0, 0 );
    gsl_vector_set( y, 1, 1 );
}

void tearDown(void)
{
    gsl_vector_free( x );
    gsl_vector_free( y );
    gsl_matrix_free( f_xy );
}

/*===========================================================================*/
void test_Interpolator2DCreateDestroy(void)
{
    interpolator_2d_t interpolador;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );
    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
}


/*===========================================================================*/
void test_Interpolator2DCreateInitializeDestroy(void)
{
    interpolator_2d_t interpolador;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );
    TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
}


/*===========================================================================*/
void test_Interpolator2DCalculateFunction1SingleInterpolation(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;

    lat1 = LATITUDE_INITIAL;
    lat2 = lat1 + STEP_LATITUDE;

    lon  = ( lon2 + lon1 ) / 2;
    lat  = ( lat2 + lat1 ) / 2;

    gsl_vector_set( x, 0, lon1 );
    gsl_vector_set( x, 1, lon2 );

    gsl_vector_set( y, 0, lat1 );
    gsl_vector_set( y, 1, lat2 );

    gsl_matrix_set( f_xy, 0, 0, _Function1( lon1, lat1) );
    gsl_matrix_set( f_xy, 0, 1, _Function1( lon2, lat1) );
    gsl_matrix_set( f_xy, 1, 0, _Function1( lon1, lat2) );
    gsl_matrix_set( f_xy, 1, 1, _Function1( lon2, lat2) );

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );
    TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
    TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
    TEST_ASSERT( _ValidateInterpolation( value, _Function1( lon, lat ), EPSILON ) );
    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
    
}


/*===========================================================================*/
void test_Interpolator2DCalculateFunction1MultipleInterpolationsLongitude(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;
    uint32_t i, limit;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;

    lat1 = LATITUDE_INITIAL;
    lat2 = lat1 + STEP_LATITUDE;

    limit = (LONGITUDE_FINAL - LONGITUDE_INITIAL) / STEP_LONGITUDE - 2;
    for ( i = 0; i < limit; i++)
    {
        lon  = ( lon2 + lon1 ) / 2;
        lat  = ( lat2 + lat1 ) / 2;

        gsl_vector_set( x, 0, lon1 );
        gsl_vector_set( x, 1, lon2 );

        gsl_vector_set( y, 0, lat1 );
        gsl_vector_set( y, 1, lat2 );

        gsl_matrix_set( f_xy, 0, 0, _Function1( lon1, lat1) );
        gsl_matrix_set( f_xy, 0, 1, _Function1( lon2, lat1) );
        gsl_matrix_set( f_xy, 1, 0, _Function1( lon1, lat2) );
        gsl_matrix_set( f_xy, 1, 1, _Function1( lon2, lat2) );

        TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
        TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
        TEST_ASSERT( _ValidateInterpolation( value, _Function1( lon, lat ), EPSILON ) );

        lon1 += STEP_LONGITUDE;
        lon2 += STEP_LONGITUDE;
    }

    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
   
}

/*===========================================================================*/
void test_Interpolator2DCalculateFunction1MultipleInterpolationsLatitude(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;
    uint32_t i, limit;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;

    lat1 = LATITUDE_INITIAL;
    lat2 = lat1 + STEP_LATITUDE;

    limit = ( LATITUDE_FINAL - LATITUDE_INITIAL ) / STEP_LATITUDE - 2;
    for ( i = 0; i < limit; i++)
    {
        lon  = ( lon2 + lon1 ) / 2;
        lat  = ( lat2 + lat1 ) / 2;

        gsl_vector_set( x, 0, lon1 );
        gsl_vector_set( x, 1, lon2 );

        gsl_vector_set( y, 0, lat1 );
        gsl_vector_set( y, 1, lat2 );

        gsl_matrix_set( f_xy, 0, 0, _Function1( lon1, lat1) );
        gsl_matrix_set( f_xy, 0, 1, _Function1( lon2, lat1) );
        gsl_matrix_set( f_xy, 1, 0, _Function1( lon1, lat2) );
        gsl_matrix_set( f_xy, 1, 1, _Function1( lon2, lat2) );

        TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
        TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
        TEST_ASSERT( _ValidateInterpolation( value, _Function1( lon, lat ), EPSILON ) );

        lat1 += STEP_LATITUDE;
        lat2 += STEP_LATITUDE;
    }

    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
   
}

/*===========================================================================*/
void test_Interpolator2DCalculateFunction1MultipleInterpolations(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;
    uint32_t i, j, limit_lat, limit_lon;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;
    
    limit_lon = (LONGITUDE_FINAL - LONGITUDE_INITIAL) / STEP_LONGITUDE - 2;
    for ( j = 0; j < limit_lon; j++)
    {
        lat1 = LATITUDE_INITIAL;
        lat2 = lat1 + STEP_LATITUDE;

        limit_lat = ( LATITUDE_FINAL - LATITUDE_INITIAL ) / STEP_LATITUDE - 2;
        for ( i = 0; i < limit_lat; i++)
        {
            lon  = ( lon2 + lon1 ) / 2;
            lat  = ( lat2 + lat1 ) / 2;

            gsl_vector_set( x, 0, lon1 );
            gsl_vector_set( x, 1, lon2 );

            gsl_vector_set( y, 0, lat1 );
            gsl_vector_set( y, 1, lat2 );

            gsl_matrix_set( f_xy, 0, 0, _Function1( lon1, lat1) );
            gsl_matrix_set( f_xy, 0, 1, _Function1( lon2, lat1) );
            gsl_matrix_set( f_xy, 1, 0, _Function1( lon1, lat2) );
            gsl_matrix_set( f_xy, 1, 1, _Function1( lon2, lat2) );

            TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
            TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
            TEST_ASSERT( _ValidateInterpolation( value, _Function1( lon, lat ), EPSILON1 ) );

            lat1 += STEP_LATITUDE;
            lat2 += STEP_LATITUDE;
        }

        lon1 += STEP_LONGITUDE;
        lon2 += STEP_LONGITUDE;
    }

    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
   
}

/*===========================================================================*/
void test_Interpolator2DCalculateFunction2SingleInterpolation(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;

    lat1 = LATITUDE_INITIAL;
    lat2 = lat1 + STEP_LATITUDE;

    lon  = ( lon2 + lon1 ) / 2;
    lat  = ( lat2 + lat1 ) / 2;

    gsl_vector_set( x, 0, lon1 );
    gsl_vector_set( x, 1, lon2 );

    gsl_vector_set( y, 0, lat1 );
    gsl_vector_set( y, 1, lat2 );

    gsl_matrix_set( f_xy, 0, 0, _Function2( lon1, lat1) );
    gsl_matrix_set( f_xy, 0, 1, _Function2( lon2, lat1) );
    gsl_matrix_set( f_xy, 1, 0, _Function2( lon1, lat2) );
    gsl_matrix_set( f_xy, 1, 1, _Function2( lon2, lat2) );

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );
    TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
    TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
    TEST_ASSERT( _ValidateInterpolation( value, _Function2( lon, lat ), EPSILON ) );
    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
    
}

/*===========================================================================*/
void test_Interpolator2DCalculateFunction2MultipleInterpolationsLongitude(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;
    uint32_t i, limit;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;

    lat1 = LATITUDE_INITIAL;
    lat2 = lat1 + STEP_LATITUDE;

    limit = (LONGITUDE_FINAL - LONGITUDE_INITIAL) / STEP_LONGITUDE - 2;
    for ( i = 0; i < limit; i++)
    {
        lon  = ( lon2 + lon1 ) / 2;
        lat  = ( lat2 + lat1 ) / 2;

        gsl_vector_set( x, 0, lon1 );
        gsl_vector_set( x, 1, lon2 );

        gsl_vector_set( y, 0, lat1 );
        gsl_vector_set( y, 1, lat2 );

        gsl_matrix_set( f_xy, 0, 0, _Function2( lon1, lat1) );
        gsl_matrix_set( f_xy, 0, 1, _Function2( lon2, lat1) );
        gsl_matrix_set( f_xy, 1, 0, _Function2( lon1, lat2) );
        gsl_matrix_set( f_xy, 1, 1, _Function2( lon2, lat2) );

        TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
        TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
        TEST_ASSERT( _ValidateInterpolation( value, _Function2( lon, lat ), EPSILON1 ) );

        lon1 += STEP_LONGITUDE;
        lon2 += STEP_LONGITUDE;
    }

    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
   
}

/*===========================================================================*/
void test_Interpolator2DCalculateFunction2MultipleInterpolationsLatitude(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;
    uint32_t i, limit;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;

    lat1 = LATITUDE_INITIAL;
    lat2 = lat1 + STEP_LATITUDE;

    limit = ( LATITUDE_FINAL - LATITUDE_INITIAL ) / STEP_LATITUDE - 2;
    for ( i = 0; i < limit; i++)
    {
        lon  = ( lon2 + lon1 ) / 2;
        lat  = ( lat2 + lat1 ) / 2;

        gsl_vector_set( x, 0, lon1 );
        gsl_vector_set( x, 1, lon2 );

        gsl_vector_set( y, 0, lat1 );
        gsl_vector_set( y, 1, lat2 );

        gsl_matrix_set( f_xy, 0, 0, _Function2( lon1, lat1) );
        gsl_matrix_set( f_xy, 0, 1, _Function2( lon2, lat1) );
        gsl_matrix_set( f_xy, 1, 0, _Function2( lon1, lat2) );
        gsl_matrix_set( f_xy, 1, 1, _Function2( lon2, lat2) );

        TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
        TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
        TEST_ASSERT( _ValidateInterpolation( value, _Function2( lon, lat ), EPSILON1 ) );

        lat1 += STEP_LATITUDE;
        lat2 += STEP_LATITUDE;
    }

    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
   
}

/*===========================================================================*/
void test_Interpolator2DCalculateFunction2MultipleInterpolations(void)
{
    interpolator_2d_t interpolador;
    double lon1, lon2, lat1, lat2;
    double lon, lat;
    double value;
    uint32_t i, j, limit_lat, limit_lon;

    TEST_ASSERT( Interpolator2DCreate( &interpolador, NUM_X_POINTS, NUM_Y_POINTS ) );

    lon1 = LONGITUDE_INITIAL;
    lon2 = lon1 + STEP_LONGITUDE;
    
    limit_lon = (LONGITUDE_FINAL - LONGITUDE_INITIAL) / STEP_LONGITUDE - 2;
    for ( j = 0; j < limit_lon; j++)
    {
        lat1 = LATITUDE_INITIAL;
        lat2 = lat1 + STEP_LATITUDE;

        limit_lat = ( LATITUDE_FINAL - LATITUDE_INITIAL ) / STEP_LATITUDE - 2;
        for ( i = 0; i < limit_lat; i++)
        {
            lon  = ( lon2 + lon1 ) / 2;
            lat  = ( lat2 + lat1 ) / 2;

            gsl_vector_set( x, 0, lon1 );
            gsl_vector_set( x, 1, lon2 );

            gsl_vector_set( y, 0, lat1 );
            gsl_vector_set( y, 1, lat2 );

            gsl_matrix_set( f_xy, 0, 0, _Function2( lon1, lat1) );
            gsl_matrix_set( f_xy, 0, 1, _Function2( lon2, lat1) );
            gsl_matrix_set( f_xy, 1, 0, _Function2( lon1, lat2) );
            gsl_matrix_set( f_xy, 1, 1, _Function2( lon2, lat2) );

            TEST_ASSERT( Interpolator2DInitialize( &interpolador, x, y, f_xy ) );
            TEST_ASSERT( Interpolator2DCalculate( &interpolador, lon, lat, &value ) );
            TEST_ASSERT( _ValidateInterpolation( value, _Function2( lon, lat ), EPSILON2 ) );

            lat1 += STEP_LATITUDE;
            lat2 += STEP_LATITUDE;
        }

        lon1 += STEP_LONGITUDE;
        lon2 += STEP_LONGITUDE;
    }

    TEST_ASSERT( Interpolator2DDestroy( &interpolador ) );
   
}


/******************************************************************************
 *  Implementations of private functions
 *****************************************************************************/

double _Function1( double lon, double lat )
{
    return lon * exp( -( lon * lon ) - ( lat * lat ) );
}

double _Function2( double lon, double lat )
{
    return 2 * ( sin(3 * lon)*cos(3 * lat) + sin(1 * lon) * cos(1 * lat) );
}


bool _ValidateInterpolation( double value, double analytic, double error_limit )
{
    if ( fabs( value - analytic) < error_limit )
    {
        return true;
    }

    return false;
}