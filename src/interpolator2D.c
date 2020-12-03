/**
 * @file interpolator2D.c
 * @author Alejandro Moreno (amoreno@veng.com.ar)
 * @brief Acá se hace una breve descripción del archivo
 * 
 * @version 2020-12-01 --------- Alejandro Moreno -------- v0.0
 *      - Implementación de \p Interpolator2DCreate
 *      - Implementación de \p Interpolator2DInitialize
 *      - Implementación de \p Interpolator2DCalculate
 *      - Implementación de \p Interpolator2DDestroy
 *  
 * @copyright Copyright (c) 2020
 * 
 * @see https://www.gnu.org/software/gsl/doc/html/interp.html
 */

/******************************************************************************
 *  Inclusions (library's order: standard C, others, user header files)
 *****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <gsl/gsl_interp2d.h>
#include <gsl/gsl_spline2d.h>
#include <gsl/gsl_vector.h>
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
static const char MSG_ERROR_ALLOCATING_VECTOR[] = 
                { "Error al alocar memoria para vector del interpolador 2d" };

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

/******************************************************************************
 *  Prototypes (declarations) of private functions  
 *****************************************************************************/

/******************************************************************************
 *  Implementations of public functions
 *****************************************************************************/


/******************************************************************************
 *  Implementations of private functions
 *****************************************************************************/

bool Interpolator2DCreate( interpolator_2d_t *self, 
                           uint32_t nx, 
                           uint32_t ny )
{
    bool ret = true;

    self->x = NULL;
    self->y = NULL;
    self->z = NULL;

    if ( false == (self->x = (double *) malloc( nx * sizeof( double ) ) ) )
    {
        perror( MSG_ERROR_ALLOCATING_VECTOR );
        ret = false;
    }
    else if ( false == (self->y = (double *) malloc( ny * sizeof( double ))) )
    {
        perror( MSG_ERROR_ALLOCATING_VECTOR );
        ret = false;
    }
    else if ( false == (self->z = 
                            (double *) malloc( nx * ny * sizeof( double ))) )
    {
        perror( MSG_ERROR_ALLOCATING_VECTOR );
        ret = false;
    }
    else
    {
        ret = true;
    }
    
    /* Esto esta suponiendo que la GSL no falla en la allocación de memoria */
    if ( true == ret )
    {
        self->T = gsl_interp2d_bilinear;
        self->spline = gsl_spline2d_alloc( self->T, nx, ny);
        self->xacc   = gsl_interp_accel_alloc();
        self->yacc   = gsl_interp_accel_alloc();

        self->nx = nx;
        self->ny = ny;
    }
    else
    {
        if ( NULL != self->x )
        {
            free( self->x );
            self->x = NULL;
        }
        if ( NULL != self->y )
        {
            free( self->y );
            self->y = NULL;
        }
        if ( NULL != self->z )
        {
            free( self->z );
            self->z = NULL;            
        }
    }
    

    return ret;
}

bool Interpolator2DInitialize( interpolator_2d_t *self,
                               gsl_vector *x,
                               gsl_vector *y,
                               gsl_matrix *z )
{
    bool ret = true;
    uint32_t i, j;
    double value;
    double buffer;

    /* Cargo los valores del vector X,
     * checkeando que se estrictamente creciente */
    self->x[0] = gsl_vector_get( x, 0 );
    for ( i = 1; i < self->nx; i++)
    {
        if ( ( buffer = gsl_vector_get( x, i ) ) > self->x[i - 1] )
        {
            self->x[i] = buffer;
        }
        else
        {
            ret = false;
        }        
    }
    
    /* Cargo los valores del vector Y, 
     * checkeando que se estrictamente creciente */
    self->y[0] = gsl_vector_get( y, 0 );
    for ( i = 1; i < self->ny; i++)
    {
        if ( ( buffer = gsl_vector_get( y, i ) ) > self->y[i - 1] )
        {
            self->y[i] = buffer;
        }
        else
        {
            ret = false;
        }      
    }

    /* Si la carga de X e Y fue exitosa la inicialización */
    if ( true == ret )
    {
        for ( i = 0; i < self->ny; i++)
        {
            for ( j = 0; j < self->nx; j++)
            {
                /* set z grid values */
                value = gsl_matrix_get( z, i, j );        
                gsl_spline2d_set(self->spline, self->z, i, j, value );
            }
        }
        
        /* initialize interpolation */
        gsl_spline2d_init( self->spline, 
                            self->x, self->y, self->z, 
                            self->nx, self->ny);
    }

    return ret; 
}


bool Interpolator2DCalculate( interpolator_2d_t *self,
                              double x,
                              double y,
                              double *z )
{    
    bool ret;

    if ( ( x >= self->x[0] ) && ( x <= self->x[self->nx - 1] ) &&
         ( y >= self->y[0] ) && ( y <= self->y[self->ny - 1] )  )
    {
        *z = gsl_spline2d_eval( self->spline, x, y, self->xacc, self->yacc);
        ret = true;
    }
    else
    {
        ret = false;
    }
    
    return ret;
}

bool Interpolator2DDestroy( interpolator_2d_t *self )
{
    gsl_spline2d_free(self->spline);
    self->spline = NULL;
    
    gsl_interp_accel_free(self->xacc);
    self->xacc = NULL;
    
    gsl_interp_accel_free(self->yacc);
    self->yacc = NULL;

    free(self->x);
    self->x = NULL;

    free(self->y);
    self->y = NULL;

    free(self->z);
    self->z = NULL;

    return true; /* return bobo */
}