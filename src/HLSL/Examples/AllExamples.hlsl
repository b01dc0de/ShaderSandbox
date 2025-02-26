//////////////////////
// AllExamples.hlsl //
//////////////////////

#include "Examples/ExampleCommon.hlsl"
#if SELECT_EXAMPLE_SHADER == 1
    #include "Examples/Example1.hlsl"
#elif SELECT_EXAMPLE_SHADER == 2
    #include "Examples/Example2.hlsl"
#elif SELECT_EXAMPLE_SHADER == 3
    #include "Examples/Example3.hlsl"
#elif SELECT_EXAMPLE_SHADER == 4
    #include "Examples/Example4.hlsl"
#elif SELECT_EXAMPLE_SHADER == 5
    #include "Examples/Example5.hlsl"
#elif SELECT_EXAMPLE_SHADER == 6
    #include "Examples/Example6.hlsl"
#elif SELECT_EXAMPLE_SHADER == 7
    #include "Examples/Example7.hlsl"
#elif SELECT_EXAMPLE_SHADER == 8
    #include "Examples/Example8.hlsl"
#elif SELECT_EXAMPLE_SHADER == 9
    #include "Examples/Example9.hlsl"
#elif SELECT_EXAMPLE_SHADER == 0
    #include "Examples/Example10.hlsl"
#endif // SELECT_EXAMPLE_SHADER

