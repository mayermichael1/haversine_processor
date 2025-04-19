#include <stdio.h>
#include <math.h>

#include "profiler.h"
#include "types.h"
#include "calc.h"


f64 zero(f64 x)
{
    return 0;
}

s32 
main (s32 argc, u8** argv )
{
    printf("sin(0.03) = %+.9f ( 0.0299955)\n", sin(0.03));
    printf("sin(3.41) = %+.9f (-0.26519615)\n", sin(3.41));
    printf("sin(0.22) = %+.9f ( 0.21822962)\n", sin(0.22));
    printf("sin(2.10) = %+.9f ( 0.86320937)\n", sin(2.10));
    printf("sin(1.60) = %+.9f ( 0.9995736)\n", sin(1.60));
    printf("sin(0.41) = %+.9f ( 0.39860933)\n", sin(0.41));
    printf("sin(2.54) = %+.9f ( 0.56595623)\n", sin(2.54));
    printf("sin(4.75) = %+.9f (-0.99929279)\n", sin(4.75));
    printf("sin(4.12) = %+.9f (-0.82960917)\n", sin(4.12));
    printf("sin(5.37) = %+.9f (-0.7914547)\n", sin(5.37));

    printf("\n");
    printf("max error sin to cos %f\n", compare_math_implementations(-3.0, 3.0, 0.00001, sin, cos));
    printf("max error sin to sin %f\n", compare_math_implementations(-3.0, 3.0, 0.00001, sin, sin));
    printf("max error sin to 0 %f\n", compare_math_implementations(-3.0, 3.0, 0.00001, sin, zero));

    printf("\n");
    printf("max error my_sqrt to sqrt %f\n", compare_math_implementations(-10.0, 10.0, 0.0001, sqrt, my_sqrt));

    printf("\n");
    printf("max error sin_half to sin (0 to PI): %f\n", compare_math_implementations(0, PI, 0.0001, sin_half, sin));
    printf("max error sin_half to sin (-PI to PI): %f\n", compare_math_implementations(-PI, PI, 0.0001, sin_half, sin));

    printf("\n");
    printf("max error sin_quarter to sin (0 to PI): %f\n", compare_math_implementations(0, PI, 0.0001,sin_quarter, sin));
    printf("max error sin_quarter to sin (-PI to PI): %f\n", compare_math_implementations(-PI, PI, 0.0001, sin_quarter, sin));

    printf("\n");
    printf("max error cos_quarter to cos (0 to PI/2): %f\n", compare_math_implementations(0, PI/2, 0.0001,cos_quarter, cos));
    printf("max error cos_quarter to cos (-PI/2 to PI/2): %f\n", compare_math_implementations(-PI/2, PI/2, 0.0001, cos_quarter, cos));

    printf("my_sin(-PI/2): %f\n", sin_half(-PI/2));
    printf("\n");

    sin_taylor_test();
    printf("\n");
    sin_taylor_horner_test();
    printf("\n");
    sin_taylor_horner_fmadd_test();
    printf("\n");
    sin_coefficient_array_test();
    printf("\n");
    asin_coefficient_array_test();

    printf("\n");

    print_profiler();
    return 0;
}

