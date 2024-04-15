#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testWBCFunc.h"

#define MEASURE_PERF

#if defined(EMBEDDED)

void assert(int condition)
{
    if (!condition)
    {
        for ( ; ; ) ;
    }
}
#endif


int WBC( void )
{
    #if !defined(EMBEDDED)
        testWBCFunc();
    #endif

    #if defined(EMBEDDED)

    for (;;);

    #else

    return ( 0 );

    #endif
}



int process(int argc, char* argv[])
{
    
    WBC();
    return 0;
}

int main(int argc, char* argv[])
{
    return process(argc, argv);
}
