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

    printf("max error sin to cos %f\n", compare_math_implementations(-3.0, 3.0, 0.00001, sin, cos));
    printf("max error sin to sin %f\n", compare_math_implementations(-3.0, 3.0, 0.00001, sin, sin));
    printf("max error sin to 0 %f\n", compare_math_implementations(-3.0, 3.0, 0.00001, sin, zero));

    printf("max error my_sqrt to sqrt %f\n", compare_math_implementations(-10.0, 10.0, 0.0001, sqrt, my_sqrt));

    print_profiler();
    return 0;
}

