#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testDeckFunc.h"

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


int Deck( void )
{
    #if !defined(EMBEDDED)
        testDeckFunc();
    #endif

    #if defined(EMBEDDED)

    for (;;);

    #else

    return ( 0 );

    #endif
}



int process(int argc, char* argv[])
{
    Deck();
    return 0;
}

int main(int argc, char* argv[])
{
    return process(argc, argv);
}
