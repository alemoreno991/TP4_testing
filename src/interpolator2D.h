/**
 * @file interpolator2D.h
 * @author Alejandro Moreno (amoreno@veng.com.ar)
 * @brief API para realizar interpolaciones en 2-D (dos dimensiones)
 * 
 * @version 2020-12-01 --------- Alejandro Moreno -------- v0.0
 *      - Declaración de \p Interpolator2DCreate
 *      - Declaración de \p Interpolator2DInitialize
 *      - Declaración de \p Interpolator2DCalculate
 *      - Declaración de \p Interpolator2DDestroy
 *      - Definición del tipo de dato \p interpolator_2d_t
 * 
 * 
 * @copyright Copyright (c) 2020
 * 
 * @warning Si bien no hay nada que impida modificar los campos de la 
 * estructura en forma directa. La idea fuerza de utilizar el enfoque
 * TDA (Tipo de Dato Abstracto) es que no se haga. En tal sentido,
 * la el funcionamiento interno de la API es quien se encarga de ello.
 * 
 */

/*=====[Avoid multiple inclusion - begin]====================================*/
#ifndef INTERPOLATOR_2D_H
#define INTERPOLATOR_2D_H

/******************************************************************************
 *  Inclusions of public function dependencies
 *****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_interp2d.h>
#include <gsl/gsl_spline2d.h>

/******************************************************************************
 *  Definition macros of public constants
 *****************************************************************************/

/******************************************************************************
 *  Public function-like macros
 *****************************************************************************/

/******************************************************************************
 *  Definitions of public data types
 *****************************************************************************/

/**
 * @struct interpolator_t interpolator.h "inc/interpolator.h"
 * @brief Contiene los parametros que se necesitan configurar para trabajar
 * con un TDA (Tipo de Dato Abstracto) interpolador. 
 * 
 */
typedef struct{

  double *x;
  double *y;
  double *z;
  
  uint32_t nx; /* x grid points */
  uint32_t ny; /* y grid points */
  
  const gsl_interp2d_type *T; 
  gsl_spline2d *spline;
  gsl_interp_accel *xacc;
  gsl_interp_accel *yacc;

} interpolator_2d_t;

/******************************************************************************
 *  Prototypes (declarations) of public functions
 *****************************************************************************/

/**
 * @brief Constructor del Interpolador 2D
 * 
 * @param self 
 * @param [in] nx Número de puntos en x 
 * @param [in] ny Número de puntos en y
 * @param [in] x  Valores equiespaciados de la serie x
 * @param [in] y  Valores equiespaciados de la serie y
 * @param [in] z  Valor de la función f(x,y)
 * @return true     Exito
 * @return false    Error
 * 
 * @warning Siempre checkear que se haya podido construir el TDA 
 */
bool Interpolator2DCreate( interpolator_2d_t *self, 
                           uint32_t nx, 
                           uint32_t ny );

/**
 * @brief Configura el interpolador con los datos que debe usar
 * para llevar adelante la interpolación
 * 
 * @param self 
 * @param [in] x Vector con los valores que adquiere x
 * @param [in] y Vector con los valores que adquiere y
 * @param [in] z Matriz con los valores que toma la función f(x,y) 
 * para cada (x,y)  
 * @return true     Exito
 * @return false    Error
 */
bool Interpolator2DInitialize( interpolator_2d_t *self,
                               gsl_vector *x,
                               gsl_vector *y,
                               gsl_matrix *z );

/**
 * @brief Calcula el valor de la función en los puntos (x,y) 
 * indicados por el usuario
 * 
 * @param self Estructura que contiene la información del TDA
 * @param [in] x Punto x en el que se desea interpolar la función
 * @param [in] y Punto y en el que se desea interpolar la función
 * @param [out] z Valor de la función f(x,y) interpolado 
 * @return true     Interpolación exitosa
 * @return false    Falla en la interpolación
 */
bool Interpolator2DCalculate( interpolator_2d_t *self,
                              double x,
                              double y,
                              double *z );

/**
 * @brief Destructor del interpolador
 * 
 * @param  self Estructura que contiene la información del TDA
 * @return true     TDA destruído satisfactoriamente
 * @return false    Destrucción fallida 
 */
bool Interpolator2DDestroy( interpolator_2d_t *self );

/*=====[Avoid multiple inclusion - end]======================================*/
#endif /* INTERPOLATOR_2D_H */
