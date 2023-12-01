#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "timing.h"
#include "testDeckFunc.h"
#define Many 1
typedef cycles_t (* measurePerf)(cycles_t, unsigned int);


#define xstr(s) str(s)
#define str(s) #s



static inline uint64_t now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return (uint64_t) ts.tv_sec * 1000000000 + ts.tv_nsec;
}


static uint64_t measureDeckNeon(uint64_t dtMin, unsigned int inputByteLen, unsigned int outputByteLen, uint16_t L){
    
    ALIGN(64) uint8_t input[inputByteLen+32];
    memset(input, 0x3B, sizeof(input)-32);
    
    ALIGN(64) uint8_t key[inputByteLen+32];
    memset(key, 0x3B, sizeof(key));
    
    ALIGN(64) uint8_t output[outputByteLen];
    memset(output, 0x45, sizeof(output));
    
    ALIGN(64) uint8_t savestate[outputByteLen]; 
    memset(savestate, 0x67, sizeof(output)); 
    
    {
        dtMin = now(); 
        if (L < (uint16_t)(0x0080)){
            uint8_t Ltr = (uint8_t) L;
            memcpy(input+(sizeof(input)-32),&Ltr,1);
            memset(input+(sizeof(input)-31),0x00,31);
        }else if(((uint16_t)(0x007F) < L )&&( L < (uint16_t)(0x4000))){// ((uint16_t)(0x007F) < L < (uint16_t)(0x4000)){
            uint8_t Ltr1 = ((uint8_t)(L&0x7F)) ^ 0x80;
            uint8_t Ltr2 = (uint8_t)(L>>7);
            memcpy(input+(sizeof(input)-32),&Ltr2,1);
            memcpy(input+(sizeof(input)-31),&Ltr1,1);
            memset(input+(sizeof(input)-30),0x00,30);
        } else if ((uint16_t)(0x3FFF)<L){
            uint8_t Ltr1 = ((uint8_t)(L&0x7F)) ^ 0x80;
            uint8_t Ltr2 = (uint8_t)(L>>7);
            uint8_t Ltr3 = ((uint8_t)(Ltr2&0x7F)) ^ 0x80;
            uint8_t Ltr4 = (uint8_t)(L>>14);
            memcpy(input+(sizeof(input)-32),&Ltr4,1);
            memcpy(input+(sizeof(input)-31),&Ltr3,1);
            memcpy(input+(sizeof(input)-30),&Ltr1,1);
            memset(input+(sizeof(input)-29),0x00,29);
        } else {
            printf("This size is not supported");
        }
	    deckFunction(input, key, output, savestate, (size_t)(outputByteLen), (size_t)(inputByteLen+32)); 
        dtMin=now()-dtMin;
        return dtMin;
    }
}

void testDeckOneBlockOut( void )
{
    FILE *fp;
    fp = fopen("DeckBenchmarking_WithPadding_OneBlockOut.csv","w");
    if (fp == NULL){
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
    uint32_t len1;   
    uint32_t len2;  
    uint64_t calibration = 0;
    int inputBytes[] = {32,64,128,256,384,512,640,768,896,1024,1152,1280,1408,1536,1664,1792,1920,2048,2176,2304,2432,2560,2688,2816,2944,3072,3200,3328,3456,3584,3712,3840,3968,4096,4160,4224,4288,4352,4416,4480,4544,4608,4672,4736,4800,4928,5056,5184,5312,5440,5568,5696,5824,5952,6080,6208,6336,6464,6592,6720,6848,6976,7104,7232,7360,7488,7616,7744,7872,8000,8128,8192,8521,8832,9152,9472,9792,10112,10432,10752,11072,11392,11712,12032,12352,12672,12992,13312,13632,15232,16384,16832,18432,20032,21632,23232,24832,26432,28032,29632,31232,32768};//{32,4160,32768};  
    uint16_t paddings[] = {0x0020,0x0040,0x0080,0x0100,0x0180,0x0200,0x0280,0x0300,0x0380,0x0400,0x0480,0x0500,0x0580,0x0600,0x0680,0x0700,0x0780,0x0800,0x0880,0x0900,0x0980,0x0a00,0x0a80,0x0b00,0x0b80,0x0c00,0x0c80,0x0d00,0x0d80,0x0e00,0x0e80,0x0f00,0x0f80,0x1000,0x1040,0x1080,0x10c0,0x1100,0x1140,0x1180,0x11c0,0x1200,0x1240,0x1280,0x12c0,0x1340,0x13c0,0x1440,0x14c0,0x1540,0x15c0,0x1640,0x16c0,0x1740,0x17c0,0x1840,0x18c0,0x1940,0x19c0,0x1a40,0x1ac0,0x1b40,0x1bc0,0x1c40,0x1cc0,0x1d40,0x1dc0,0x1e40,0x1ec0,0x1f40,0x1fc0,0x2000,0x2149,0x2280,0x23c0,0x2500,0x2640,0x2780,0x28c0,0x2a00,0x2b40,0x2c80,0x2dc0,0x2f00,0x3040,0x3180,0x32c0,0x3400,0x3540,0x3b80,0x4000,0x41c0,0x4800,0x4e40,0x5480,0x5ac0,0x6100,0x6740,0x6d80,0x73c0,0x7a00,0x8000}; //{0x0020,0x1040,0x8000}
    int outputBlocks[] = {1};
    fprintf(fp,"%d,output bytes\n",64*outputBlocks[0]);
    fprintf(fp,"input # bytes\n");
    for (len1=0; len1 < 102; len1=len1+1){ //102
        for(len2=0; len2 < 1; len2=len2+1) {
            fprintf(fp,"%d,",inputBytes[len1]);
            uint64_t MinT = 10000000000000;
            for(int i = 0; i <1000000; i++){ //<=1000000
                calibration = measureDeckNeon(calibration,inputBytes[len1],64*outputBlocks[len2],paddings[len1]);
                if (calibration < MinT){
                    MinT=calibration;
                }
            }
            fprintf(fp,"%9"PRId64" ,%s, %6.3f, %s/byte\n", MinT, getTimerUnit(), MinT*1.0/(inputBytes[len1]), getTimerUnit());
            printf("%d,input bytes,%d,output bytes ",inputBytes[len1],64*outputBlocks[len2]);
            printf("%9"PRId64" ,%s, %6.3f, %s/byte\n", MinT, getTimerUnit(), MinT*1.0/(inputBytes[len1]), getTimerUnit());
        }
        
    }
    printf("DeckBenchmarking_WithPadding_OneBlockOut is complete.\n");
    fclose(fp);
}

void testDeckOneBlockIn( void )
{
    FILE *fp;
    fp = fopen("DeckBenchmarking_WithPadding_OneBlockIn.csv","w");
    if (fp == NULL){
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
    uint32_t len1;   
    uint32_t len2;  
    uint64_t calibration = 0;
    int inputBytes[] = {32};  
    uint64_t paddings[] = {0x0020};
    int outputBlocks[] = {1,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,33,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,65,80,96,97,112,128,129,144,160,161,176,192,193,208,224,225,240,256,257,272,288,289,304,320,321,336};
    fprintf(fp,"%d,input bytes\n",inputBytes[0]);
    fprintf(fp,"output # bytes\n");
    for (len1=0; len1 < 1; len1=len1+1){ 
        for(len2=0; len2 < 60; len2=len2+1) { 
            fprintf(fp,"%d,",64*outputBlocks[len2]);
            uint64_t MinT = 10000000000000;
            for(int i = 0; i <1000000; i++){ //<=1000000
                calibration = measureDeckNeon(calibration,inputBytes[len1],64*outputBlocks[len2], paddings[len1]);
                if (calibration < MinT){
                    MinT=calibration;
                }
            }
            fprintf(fp,"%9"PRId64" ,%s, %6.3f, %s/byte\n", MinT, getTimerUnit(), MinT*1.0/(64*outputBlocks[len2]), getTimerUnit());
            printf("%d,input bytes,%d,output bytes ",inputBytes[len1],64*outputBlocks[len2]);
            printf("%9"PRId64" ,%s, %6.3f, %s/byte\n", MinT, getTimerUnit(), MinT*1.0/(64*outputBlocks[len2]), getTimerUnit());
        }
        
    }
    printf("DeckBenchmarking_WithPadding_OneBlockIn is complete.\n");
    fclose(fp);
}


void testDeckFunc(void)
{
    testDeckOneBlockOut();
    testDeckOneBlockIn();
}



