#include "calc.h"
#include "stdio.h"
#include <float.h>
#include "x86intrin.h"

#define DOMAIN_MIN_(name) min_ ##name
#define DOMAIN_MIN(name) DOMAIN_MIN_(name)

#define DOMAIN_MAX_(name) max_ ##name
#define DOMAIN_MAX(name) DOMAIN_MAX_(name)

#define EXPAND_MATH_FUNCTION(name, x) name(x)

#define DOMAIN_CHECK(name) \
    static f64 DOMAIN_MIN(name) = DBL_MAX; \
    static f64 DOMAIN_MAX(name) = -DBL_MAX; \
    f64 domain_##name (f64 x) { \
        f64 result = 0; \
        if (x < DOMAIN_MIN(name)){ DOMAIN_MIN(name) = x; } \
        else if (x > DOMAIN_MAX(name)){DOMAIN_MAX(name) = x;} \
        result = EXPAND_MATH_FUNCTION(name, x); \
        return result; \
    }

DOMAIN_CHECK(sin)
DOMAIN_CHECK(cos)
DOMAIN_CHECK(asin)
DOMAIN_CHECK(sqrt)

f64 
compare_math_implementations (f64 from, f64 to, f64 interval, 
    math_function_pointer reference, math_function_pointer compare_to)
{
    f64 max_error = 0;

    for (f64 i = from; i < to; i+=interval)
    {
        f64 difference = reference(i)  - compare_to(i);
        if (fabs(difference) > max_error)
        {
            max_error = fabs(difference);
        }
    }
    return max_error;
}
inline f64
square (f64 x)
{
    f64 result = 0;
    result = x * x;
    return result;
}


f64 
reference_haversine ( coordinate coord1, coordinate coord2, f64 earth_radius)
{
    f64 (*sinref)(f64 x) = sin;
    f64 delta_lat = degrees_to_radians(coord2.latitude - coord1.latitude); 
    f64 delta_lon = degrees_to_radians(coord2.longitude - coord1.longitude);

    f64 lat1 = degrees_to_radians(coord1.latitude);
    f64 lat2 = degrees_to_radians(coord2.latitude);

#if 1
    f64 a = square(domain_sin(delta_lat/2.0)) +
            domain_cos(lat1) * domain_cos(lat2) * square(domain_sin(delta_lon/2.0)); 

    f64 c = 2.0 * domain_asin(domain_sqrt(a));
#endif

#if 0
    f64 a = square(sin(delta_lat/2.0)) +
            cos(lat1) * cos(lat2) * square(sin(delta_lon/2.0)); 

    f64 c = 2.0 * asin(sqrt(a));

#endif

    f64 result = earth_radius * c;

    return result;
}

void
print_math_function_domains ()
{
    printf("sin : \tmin: %f\tmax: %f\n", min_sin, max_sin);
    printf("cos : \tmin: %f\tmax: %f\n", min_cos, max_cos);
    printf("sqrt: \tmin: %f\tmax: %f\n", min_sqrt, max_sqrt);
    printf("asin: \tmin: %f\tmax: %f\n", min_asin, max_asin);
}

f64 
my_sqrt (f64 x)
{
    f64 result = 0;
    __m128d wide_input = _mm_set_sd(x);
    __m128d wide_zero = _mm_set_sd(0);
    __m128d wide_result = _mm_sqrt_sd(wide_zero, wide_input);
    result = _mm_cvtsd_f64(wide_result);
    return result;
}

f64 
sin_half (f64 x)
{
    f64 abs_x = fabs(x);
    f64 result = (4 * abs_x) / PI - (4 * abs_x * abs_x) / (PI * PI);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64 
sin_quarter (f64 x)
{
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }
    f64 result = (4 * abs_x) / PI - (4 * abs_x * abs_x) / (PI * PI);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64
cos_quarter (f64 x)
{
    x += PI/2;
    f64 result = sin_quarter(x);
    return result;
}

f64 
factorial (u64 x)
{
    f64 result = 1;
    for (u64 i = x; i > 0; --i)
    {
        result *= i;
    }
    return result;
}

f64 
sin_taylor_series_coefficient (u64 factor)
{
    f64 sign = -1;
    if (((factor-1) / 2) % 2 == 0)
    {
        sign = 1;
    }
    f64 result = sign / (f64)factorial(factor);
    return result;
}

f64 
sin_coefficient_function(f64 x, f64 *coefficients, u8 count)
{
    __m128d result = _mm_set_sd(0);
    __m128d x2 = _mm_set_sd(x * x);

    for (u8 i = count-1; i >= 0; --i)
    {
        __m128d coefficient = _mm_set_sd(coefficients[i]);
        result = _mm_fmadd_sd(result, x2, coefficient);
    }
    
    f64 result_f64 = _mm_cvtsd_f64(result);
    result_f64 *= x;
    return result_f64;
}

f64
sin_taylor_series_horner_fmadd_function(f64 x, u8 factor)
{
    __m128d result = _mm_set_sd(0);
    __m128d x2 = _mm_set_sd(x * x);

    for (u8 current_factor = factor; current_factor >= 1; current_factor-=2)
    {
        __m128d coefficient = _mm_set_sd(sin_taylor_series_coefficient(current_factor));
        result = _mm_fmadd_sd(result, x2, coefficient);
    }
    
    f64 result_f64 = _mm_cvtsd_f64(result);
    result_f64 *= x;
    return result_f64;
}

f64
sin_taylor_series_function (f64 x, u8 factor)
{
    f64 result = 0;
    u8 current_factor = 1;
    f64 x2 = x * x;

    while (current_factor <= factor)
    {
        result += x * sin_taylor_series_coefficient(current_factor);
        x = x * x2;
        current_factor += 2;
    }
    return result;
}

f64 sin_taylor_series (f64 x, u8 factor)
{    
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }

    f64 result = sin_taylor_series_function(abs_x, factor);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64 
sin_coefficient_array (f64 x, f64 *coefficients, u8 count)
{    
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }

    f64 result = sin_coefficient_function(abs_x, coefficients, count);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}


f64
asin_coefficient_array(f64 x, f64 *coefficients, u8 count)
{
    __m128d result = _mm_set_sd(0);
    __m128d x2 = _mm_set_sd(x * x);

    for (u8 i = count-1; i >= 0; --i)
    {
        __m128d coefficient = _mm_set_sd(coefficients[i]);
        result = _mm_fmadd_sd(result, x2, coefficient);
    }
    
    f64 result_f64 = _mm_cvtsd_f64(result);
    result_f64 *= x;
    return result_f64;
}

f64
sin_taylor_series_horner_function (f64 x, u8 factor)
{
    f64 result = 0;
    f64 x2 = x * x;

    for (u8 current_factor = factor; current_factor >= 1; current_factor-=2)
    {
        result *= x2;
        result += sin_taylor_series_coefficient(current_factor);
    }
    result *= x;
    return result;
}


f64
sin_taylor_series_horner (f64 x, u8 factor)
{    
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }

    f64 result = sin_taylor_series_horner_function(abs_x, factor);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64
sin_taylor_series_horner_fmadd (f64 x, u8 factor)
{    
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }

    f64 result = sin_taylor_series_horner_fmadd_function(abs_x, factor);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

void 
sin_taylor_test()
{

    for (u8 factor = 3; factor < 29; factor+=2)
    {
        f64 max_error = 0;
        f64 x_at_max = 0;

        for (f64 x = -PI; x < PI; x+=0.000001)
        {
            f64 difference = sin(x)  - sin_taylor_series(x, factor);
            if (fabs(difference) > max_error)
            {
                max_error = fabs(difference);
                x_at_max = x;
            }
        }
        printf(
            "sin to sin_taylor_series_[%i](-PI, PI, 0.000001) max error: %.20f at %.20f\n",
            factor,
            max_error,
            x_at_max
        );
    }

}

void 
sin_taylor_horner_test()
{

    for (u8 factor = 3; factor < 29; factor+=2)
    {
        f64 max_error = 0;
        f64 x_at_max = 0;

        for (f64 x = -PI; x < PI; x+=0.000001)
        {
            f64 difference = sin(x)  - sin_taylor_series_horner(x, factor);
            if (fabs(difference) > max_error)
            {
                max_error = fabs(difference);
                x_at_max = x;
            }
        }
        printf(
            "sin to sin_taylor_series_horner[%i](-PI, PI, 0.000001) max error: %.20f at %.20f\n",
            factor,
            max_error,
            x_at_max
        );
    }

}

void 
sin_taylor_horner_fmadd_test()
{

    for (u8 factor = 3; factor < 29; factor+=2)
    {
        f64 max_error = 0;
        f64 x_at_max = 0;

        for (f64 x = -PI; x < PI; x+=0.000001)
        {
            f64 difference = sin(x)  - sin_taylor_series_horner_fmadd(x, factor);
            if (fabs(difference) > max_error)
            {
                max_error = fabs(difference);
                x_at_max = x;
            }
        }
        printf(
            "sin to sin_taylor_series_fmadd[%i](-PI, PI, 0.000001) max error: %.20f at %.20f\n",
            factor,
            max_error,
            x_at_max
        );
    }

}

#include "sine_coefficients.cpp"

void
sin_coefficient_array_test()
{
    printf("Taylor series with pre-computed coefficients:\n");     
    for (u8 factor_index = 0; factor_index <= 16; factor_index++)
    {
        f64 max_error = 0;
        f64 x_at_max = 0;

        for (f64 x = -PI; x < PI; x+=0.000001)
        {
            u8 count = factor_index + 1;
            f64 difference = sin(x) - sin_coefficient_array(
                x, 
                SineRadiansC_Taylor,
                count  
            );
            if (fabs(difference) > max_error)
            {
                max_error = fabs(difference);
                x_at_max = x;
            }
        }
        u8 factor = factor_index * 2 + 1;
        printf(
            "sin to sin_taylor_series_coefficients[%i](-PI, PI, 0.000001) max error: %.20f at %.20f\n",
            factor,
            max_error,
            x_at_max
        );
    }

    printf("Math for the Working Programmer coefficients:\n");

    for(u8 max_count = 2; max_count <= 11; ++max_count)
    {
        f64 max_error = 0;
        f64 x_at_max = 0;

        for (f64 x = -PI; x < PI; x+=0.000001)
        {
            f64 difference = sin(x) - sin_coefficient_array(
                x, 
                SineRadiansC_MFTWP[max_count],
                max_count
            );
            if (fabs(difference) > max_error)
            {
                max_error = fabs(difference);
                x_at_max = x;
            }
        }
        printf(
            "sin to sin_coefficients[%i](-PI, PI, 0.000001) max error: %.20f at %.20f\n",
            max_count,
            max_error,
            x_at_max
        );
    }
}

#include "./arcsine_coefficients.cpp"

f64
asin_approximated(f64 x)
{
    __m128d result = _mm_set_sd(0);
    __m128d x2 = _mm_set_sd(x * x);
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][17]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][16]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][15]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][14]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][13]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][12]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][11]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][10]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][9]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][8]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][7]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][6]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][5]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][4]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][3]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][2]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][1]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(ArcsineRadiansC_MFTWP[18][0]));
    f64 result_f64 = _mm_cvtsd_f64(result);
    result_f64 *= x;
    return result_f64;
}

f64 
asin_core(f64 x)
{
    f64 value = 0;
    if(x <= 1/my_sqrt(2.0))
    {
        value = asin_approximated(x);
    }
    else
    {
        value = PI/2.0 - asin_approximated(my_sqrt(1-(x*x)));
    }
    return value;
}

f64 
sin_core(f64 x)
{
    f64 abs_x = fabs(x);
    if(abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }
    __m128d result = _mm_set_sd(0);
    __m128d x2 = _mm_set_sd(abs_x * abs_x);

    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][8]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][7]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][6]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][5]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][4]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][3]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][2]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][1]));
    result = _mm_fmadd_sd(result, x2, _mm_set_sd(SineRadiansC_MFTWP[9][0]));
    
    f64 result_f64 = _mm_cvtsd_f64(result);
    result_f64 *= abs_x;

    if(x < 0.0)
    {
        result_f64 *= -1;
    }
    return result_f64;
}

f64
cos_core(f64 x)
{
    x += PI/2;
    f64 result = sin_core(x);
    return result;
}

f64 
sqrt_core(f64 x)
{
    f64 result = 0;
    __m128d wide_input = _mm_set_sd(x);
    __m128d wide_zero = _mm_set_sd(0);
    __m128d wide_result = _mm_sqrt_sd(wide_zero, wide_input);
    result = _mm_cvtsd_f64(wide_result);
    return result;
}

void
asin_coefficient_array_test()
{
    printf("Taylor series with pre-computed coefficients:\n");     
    for (u8 factor_index = 0; factor_index <= 28; factor_index++)
    {
        f64 max_error = 0;
        f64 x_at_max = 0;

        for (f64 x = 0; x < (1/sqrt(2)); x+=0.000001)
        {
            u8 count = factor_index + 1;
            //TODO: change this calculation
            f64 difference = asin(x) - asin_coefficient_array(
                x, 
                ArcsineRadiansC_Taylor,
                count  
            );
            if (fabs(difference) > max_error)
            {
                max_error = fabs(difference);
                x_at_max = x;
            }
        }
        u8 factor = factor_index * 2 + 1;
        printf(
            "asin to asin_taylor_series_coefficients[%i](0, 1/sqrt(2), 0.000001) max error: %.20f at %.20f\n",
            factor,
            max_error,
            x_at_max
        );
    }

    printf("Math for the Working Programmer coefficients:\n");

    for(u8 max_count = 2; max_count <= 22; ++max_count)
    {
        f64 max_error = 0;
        f64 x_at_max = 0;

        for (f64 x = 0; x < (1/sqrt(2)); x+=0.000001)
        {
            f64 difference = asin(x) - asin_coefficient_array(
                x, 
                ArcsineRadiansC_MFTWP[max_count],
                max_count
            );
            if (fabs(difference) > max_error)
            {
                max_error = fabs(difference);
                x_at_max = x;
            }
        }
        printf(
            "asin to asin_coefficients[%i](0, 1/sqrt(2), 0.000001) max error: %.20f at %.20f\n",
            max_count,
            max_error,
            x_at_max
        );
    }

    printf("handwritten acos approximation: \n");

    f64 max_error = 0;
    f64 x_at_max = 0;

    for (f64 x = 0; x < (1/sqrt(2)); x+=0.000001)
    {
        f64 difference = asin(x) - asin_approximated(x);
        if (fabs(difference) > max_error)
        {
            max_error = fabs(difference);
            x_at_max = x;
        }
    } printf(
        "asin to asin_approximated(0, 1/sqrt(2), 0.000001) max error: %.20f at %.20f\n",
        max_error,
        x_at_max
    );
}

void
asin_coefficient_array_test_extended()
{
    printf("handwritten asin extended to 0 to 1:\n");

    f64 max_error = 0;
    f64 x_at_max = 0;

    for (f64 x = 0; x < 1; x+=0.000001)
    {
        f64 difference = asin(x) - asin_core(x);
        if (fabs(difference) > max_error)
        {
            max_error = fabs(difference);
            x_at_max = x;
        }
    }
    printf(
        "asin to asin_core(0, 1/sqrt(2), 0.000001) max error: %.20f at %.20f\n",
        max_error,
        x_at_max
    ); 
}

void 
test_core_functions()
{
    printf("asin to asin_core(0, 1, 0.000001) max error: %.20f\n",compare_math_implementations(0, 1, 0.000001, asin, asin_core));
    printf("sqrt to sqrt_core(0, 1, 0.000001) max error: %.20f\n",compare_math_implementations(0, 1, 0.000001, sqrt, sqrt_core));
    printf("sin to sin_core(-PI, PI, 0.000001) max error: %.20f\n",compare_math_implementations(-PI, PI, 0.000001, sin, sin_core));
    printf("cos to cos_core(-PI/2, PI/2, 0.000001) max error: %.20f\n",compare_math_implementations(-PI/2, PI/2, 0.000001, cos, cos_core));
}
