/**
 * @file test_interpolator2D.h
 * @author Alejandro Moreno (amoreno@veng.com.ar)
 * @brief Aqui se agrupa la implementación de la batería de testeos 
 * correspondientes a las funciones geometricas auxiliares
 *
 */

/*=====[Avoid multiple inclusion - begin]====================================*/
#ifndef TEST_INTERPOLATOR2D_H
#define TEST_INTERPOLATOR2D_H

/******************************************************************************
 *  Inclusions of public function dependencies
 *****************************************************************************/

/******************************************************************************
 *  Definition macros of public constants
 *****************************************************************************/                                                                        

/******************************************************************************
 *  Public function-like macros
 *****************************************************************************/

/******************************************************************************
 *  Definitions of public data types
 *****************************************************************************/

/******************************************************************************
 *  Prototypes (declarations) of public functions
 *****************************************************************************/

/**
 * @brief Función que inicializa lo que se necesita para correr los casos de 
 * prueba
 * 
 * @return int 
 */
int InitSuiteInterpolator2D(void);

/**
 * @brief Función que destruye todo lo que se inicializó para poder correr los
 * casos de prueba 
 * 
 * @return int 
 */
int CleanSuiteInterpolator2D(void);

/**
 * @brief Testeo que verifica la correcta construcción y destrucción del 
 * "objeto"
 * 
 */
void TEST_Interpolator2DCreateDestroy(void);


/**
 * @brief Testeo que verifica la correcta construcción, inicialización y
 *  destrucción del "objeto"
 * 
 */
void TEST_Interpolator2DCreateInitializeDestroy(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 1, en un punto.
 * 
 */
void TEST_Interpolator2DCalculateFunction1SingleInterpolation(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 1, en multiples
 * puntos, para la coordenada de longitud.
 * 
 */
void TEST_Interpolator2DCalculateFunction1MultipleInterpolationsLongitude(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 1, en multiples
 * puntos, para la coordenada de latitud.
 * 
 */
void TEST_Interpolator2DCalculateFunction1MultipleInterpolationsLatitude(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 1, en multiples
 * puntos.
 * 
 */
void TEST_Interpolator2DCalculateFunction1MultipleInterpolations(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 2, en un punto.
 * 
 */
void TEST_Interpolator2DCalculateFunction2SingleInterpolation(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 2, en multiples
 * puntos, para la coordenada de longitud.
 * 
 */
void TEST_Interpolator2DCalculateFunction2MultipleInterpolationsLongitude(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 2, en multiples
 * puntos, para la coordenada de latitude.
 * 
 */
void TEST_Interpolator2DCalculateFunction2MultipleInterpolationsLatitude(void);

/**
 * @brief Testeo la interpolación bidimensional de la función 2, en multiples
 * puntos.
 * 
 */
void TEST_Interpolator2DCalculateFunction2MultipleInterpolations(void);

/*=====[Avoid multiple inclusion - end]======================================*/
#endif /* TESTS_INTERPOLATOR2D_H */