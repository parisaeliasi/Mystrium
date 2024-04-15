#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "timing.h"
#include <arm_neon.h> // Include ARM NEON intrinsics header
#include "testWBCFunc.h"

typedef cycles_t (* measurePerf)(cycles_t, unsigned int);


#define xstr(s) str(s)
#define str(s) #s


static inline uint64_t now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return (uint64_t) ts.tv_sec * 1000000000 + ts.tv_nsec;
}


static uint64_t WBC_512(uint64_t dtMin){
    
    ALIGN(64) uint8_t MK[64];
    memset(MK, 0xC2, 63);
    memset(MK+63,0x3F,1);
    
    ALIGN(64) uint8_t Key[640];                  
    
    ALIGN(64) uint8_t VR[64];                  
    memset(VR, 0x3B, sizeof(VR));
    ALIGN(64) uint8_t UR[384];                 
    memset(UR, 0x2A, sizeof(UR));
    ALIGN(64) uint8_t UL[64];                  
    memset(UL, 0x49, sizeof(UL));
    
    ALIGN(64) uint8_t keyHK1[64];           
    //memset(keyHK1, 0xA5, sizeof(keyHK1));
    ALIGN(64) uint8_t UL1[64];                      // UL1  = UL xor output of first hash function.
    
    //We assume W is 63 bytes. Maximum size of W is 63 bytes.
    ALIGN(64) uint8_t W[64];                        // Tweak is always upto 63 byte. initialize tweak. 
    memset(W, 0x82, sizeof(W)-1);                   // 63 bytes of W is filled with 0x82 and the last byte is the encoding thish says we have 63 bytes.   
    W[64] = 0x3F;                                   // 63 in binary... and we have just one block of length encoding so 0x3F, not 0xBF
    
    ALIGN(64) uint8_t FK1_input[544];               // This is W(63bytes)+encoding(1byte)+UL(64bytes)+UR(384bytes)+encoding(32bytes = 2 bytes (384+64=448 --> 0000 0001 1100 0000 -> 0000 0011 1100 0000 : 03 C0) +30 bytes zero)   
    ALIGN(64) uint8_t keyFK1[544];                  // key to the first deck function 
    //memset(keyFK1, 0xA5, sizeof(keyFK1)); 
    ALIGN(64) uint8_t FK1_output[64];
    memset(FK1_output, 0x15, sizeof(FK1_output));

    
    ALIGN(64) uint8_t FK2_input[160];                // W+encoding (64bytes) ; FK1_output(64bytes) + encoding (32bytes = 1bytes of 0x40 + 31 bytes of zero)
    ALIGN(64) uint8_t keyFK2[160];                   // key to the second deck function 
    //memset(keyFK2, 0xA6, sizeof(keyFK2)); 
    ALIGN(64) uint8_t FK2_output[448];
    memset(FK2_output, 0x16, sizeof(FK2_output));

    ALIGN(64) uint8_t XLXR_P[480];
    ALIGN(64) uint8_t XLXR[448];
    
    ALIGN(64) uint8_t keyHK2[480];           
    //memset(keyHK2, 0xA7, sizeof(keyHK2));
    ALIGN(64) uint8_t YR[64];                  
    memset(YR, 0xBB, sizeof(YR));
    
    ALIGN(64) uint8_t FK1_savestate[640];
    memset(FK1_savestate, 0x25, sizeof(FK1_savestate));
    ALIGN(64) uint8_t FK2_savestate[640];
    memset(FK2_savestate, 0x26, sizeof(FK2_savestate));
    {
        dtMin = now();
        keyFunction(MK, Key, (size_t)(640));
        memcpy(keyHK1,Key,64);
        memcpy(keyFK1,Key,544);
        memcpy(keyFK2,Key,160);
        memcpy(keyHK2,Key,480);
	    Multimixer128field(VR, keyHK1, UL1, (size_t)64, UL);        // this generates 64bytes output HK1  void Multimixer128field(const uint8_t *input, const uint8_t *key, const uint8_t *output, size_t inputLen, const uint8_t *inputT);
        memcpy(FK1_input,W,64);                                     // FK1_input = W(63bytes) + encoding(1byte)
        memcpy(FK1_input+64,UL1,64);                                // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes)
        memcpy(FK1_input+128,UR,384);                               // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes)
        memset(FK1_input+512,0x03,1);                               // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes) + encoding (1stbyte) 
        memset(FK1_input+513,0xC0,1);                               // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes) + encoding (1st&2ndbyte) 
        memset(FK1_input+514,0x00,30);                              // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes) + encoding (03C0 + 30 bytes of zero)
        deckFunction(FK1_input, keyFK1, FK1_output, FK1_savestate,(size_t)(64), (size_t)(544), VR); //void deckFunction(const uint8_t *input, const uint8_t *key, const uint8_t *output, const uint8_t *savestate, size_t outputLen, size_t inputLen, const uint8_t *inputV);
        memcpy(FK2_input,W,64);                                     // FK2_input = W(63bytes) + encoding(1byte)
        memcpy(FK2_input+64,FK1_output,64);                         // FK2_input = W(63bytes) + encoding(1byte) +  FK1_output(64bytes) 
        memset(FK2_input+128,0x40,1);                               // FK2_input = W(63bytes) + encoding(1byte) +  FK1_output(64bytes) + encoding (32bytes = 1bytes of 0x40 
        memset(FK2_input+129,0x00,31);                              // FK2_input = W(63bytes) + encoding(1byte) +  FK1_output(64bytes) + encoding (32bytes = 1bytes of 0x40 + 31 bytes of zero)
        memcpy(XLXR,UL1,64);
        memcpy(XLXR+64,UR,384); 
        deckFunction(FK2_input, keyFK2, FK2_output, FK2_savestate,(size_t)(448), (size_t)(160), XLXR); 
        memcpy(XLXR_P,XLXR,448);
        memset(XLXR_P+448,0x03,1);
        memset(XLXR_P+449,0xC0,1);
        memset(XLXR_P+450,0x00,30);
        Multimixer128field(XLXR_P, keyHK2, YR, (size_t)480,FK2_input);
        dtMin=now()-dtMin;
        return dtMin;
    }
}


static uint64_t WBC_4096(uint64_t dtMin){
    
    ALIGN(64) uint8_t MK[64];
    memset(MK, 0xC2, 63);
    memset(MK+63,0x3F,1);
    
    ALIGN(64) uint8_t Key[4160];
    
    ALIGN(64) uint8_t VR[64];                  
    memset(VR, 0x3B, sizeof(VR));
    ALIGN(64) uint8_t UR[3968];                 
    memset(UR, 0x2A, sizeof(UR));
    ALIGN(64) uint8_t UL[64];                  
    memset(UL, 0x49, sizeof(UL));
    
    ALIGN(64) uint8_t keyHK1[64];           
    //memset(keyHK1, 0xA5, sizeof(keyHK1));
    ALIGN(64) uint8_t UL1[64];                      // UL1  = UL xor output of first hash function.
    
    //We assume W is 63 bytes. Maximum size of W is 63 bytes.
    ALIGN(64) uint8_t W[64];                        // Tweak is always upto 63 byte. initialize tweak. 
    memset(W, 0x82, sizeof(W)-1);                   // 63 bytes of W is filled with 0x82 and the last byte is the encoding thish says we have 63 bytes.   
    W[64] = 0x3F;                                   // 63 in binary... and we have just one block of length encoding so 0x3F, not 0xBF
    
    ALIGN(64) uint8_t FK1_input[4128];               // This is W(63bytes)+encoding(1byte)+UL(64bytes)+UR(3968bytes)+encoding(32bytes = 2 bytes (3968+64=4032 --> 0000 1111 1100 0000 -> 0001 1111 1100 0000 : 1F C0) +30 bytes zero)   
    ALIGN(64) uint8_t keyFK1[4128];                  // key to the first deck function 
    //memset(keyFK1, 0xA5, sizeof(keyFK1)); 
    ALIGN(64) uint8_t FK1_output[64];
    memset(FK1_output, 0x15, sizeof(FK1_output));

    
    ALIGN(64) uint8_t FK2_input[160];                // W+encoding (64bytes) ; FK1_output(64bytes) + encoding (32bytes = 1bytes of 0x40 + 31 bytes of zero)
    ALIGN(64) uint8_t keyFK2[160];                   // key to the second deck function 
    //memset(keyFK2, 0xA6, sizeof(keyFK2)); 
    ALIGN(64) uint8_t FK2_output[4032];
    memset(FK2_output, 0x16, sizeof(FK2_output));

    ALIGN(64) uint8_t XLXR_P[4064];
    ALIGN(64) uint8_t XLXR[4032];
    
    ALIGN(64) uint8_t keyHK2[4064];           
    //memset(keyHK2, 0xA7, sizeof(keyHK2));
    ALIGN(64) uint8_t YR[64];                  
    memset(YR, 0xBB, sizeof(YR));
    
    ALIGN(64) uint8_t FK1_savestate[4160];
    memset(FK1_savestate, 0x25, sizeof(FK1_savestate));
    ALIGN(64) uint8_t FK2_savestate[4160];
    memset(FK2_savestate, 0x26, sizeof(FK2_savestate));
    {
        dtMin = now();
        keyFunction(MK, Key, (size_t)(4160));
        memcpy(keyHK1,Key,64);
        memcpy(keyFK1,Key,4128);
        memcpy(keyFK2,Key,160);
        memcpy(keyHK2,Key,4064);
	    Multimixer128field(VR, keyHK1, UL1, (size_t)64, UL);        // this generates 64bytes output HK1  void Multimixer128field(const uint8_t *input, const uint8_t *key, const uint8_t *output, size_t inputLen, const uint8_t *inputT);
        memcpy(FK1_input,W,64);                                     // FK1_input = W(63bytes) + encoding(1byte)
        memcpy(FK1_input+64,UL1,64);                                // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes)
        memcpy(FK1_input+128,UR,384);                               // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes)
        memset(FK1_input+512,0x1F,1);                               // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes) + encoding (1stbyte) 
        memset(FK1_input+513,0xC0,1);                               // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes) + encoding (1st&2ndbyte) 
        memset(FK1_input+514,0x00,30);                              // FK1_input = W(63bytes) + encoding(1byte) + UL(64bytes) + UR(384bytes) + encoding (03C0 + 30 bytes of zero)
        deckFunction(FK1_input, keyFK1, FK1_output, FK1_savestate,(size_t)(64), (size_t)(4128), VR); //void deckFunction(const uint8_t *input, const uint8_t *key, const uint8_t *output, const uint8_t *savestate, size_t outputLen, size_t inputLen, const uint8_t *inputV);
        memcpy(FK2_input,W,64);                                     // FK2_input = W(63bytes) + encoding(1byte)
        memcpy(FK2_input+64,FK1_output,64);                         // FK2_input = W(63bytes) + encoding(1byte) +  FK1_output(64bytes) 
        memset(FK2_input+128,0x40,1);                               // FK2_input = W(63bytes) + encoding(1byte) +  FK1_output(64bytes) + encoding (32bytes = 1bytes of 0x40 
        memset(FK2_input+129,0x00,31);                              // FK2_input = W(63bytes) + encoding(1byte) +  FK1_output(64bytes) + encoding (32bytes = 1bytes of 0x40 + 31 bytes of zero)
        memcpy(XLXR,UL1,64);
        memcpy(XLXR+64,UR,3968); 
        deckFunction(FK2_input, keyFK2, FK2_output, FK2_savestate,(size_t)(4032), (size_t)(160), XLXR); 
        memcpy(XLXR_P,XLXR,4032);
        memset(XLXR_P+4032,0x1F,1);
        memset(XLXR_P+4033,0xC0,1);
        memset(XLXR_P+4034,0x00,30);
        Multimixer128field(XLXR_P, keyHK2, YR, (size_t)4064,FK2_input);
        dtMin=now()-dtMin;
        return dtMin;
    }
}


void testWBC_512( void )
{
    
    FILE *fp;
    fp = fopen("wbc512.csv","w");
    if (fp == NULL){
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    uint64_t calibration = 0;
    uint64_t MinT = 10000000000000;
    for(int i = 0; i <1000000; i++){ //<=1000000
        calibration = WBC_512(calibration);
        if (calibration < MinT){
            MinT=calibration;
        }
    }
    fprintf(fp,"%8d,input bytes, %9"PRId64" ,%s, %6.3f, %s/byte\n",512, MinT, getTimerUnit(), MinT*1.0/(512), getTimerUnit());
    fclose(fp);
    printf("%8d,input bytes, %9"PRId64" ,%s, %6.3f, %s/byte\n",512, MinT, getTimerUnit(), MinT*1.0/(512), getTimerUnit());
}

void testWBC_4096( void )
{
    
    FILE *fp;
    fp = fopen("wbc4096.csv","w");
    if (fp == NULL){
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    uint64_t calibration = 0;
    uint64_t MinT = 10000000000000;
    for(int i = 0; i <1000000; i++){ //<=1000000
        calibration = WBC_4096(calibration);
        //printf("%8d,input bytes, %9"PRId64" ,%s, %6.3f, %s/byte\n",4096, calibration, getTimerUnit(), calibration*1.0/(4096), getTimerUnit());
        if (calibration < MinT){
            MinT=calibration;
        }
    }
    fprintf(fp,"%8d,input bytes, %9"PRId64" ,%s, %6.3f, %s/byte\n",4096, MinT, getTimerUnit(), MinT*1.0/(4096), getTimerUnit());
    fclose(fp);
    printf("%8d,input bytes, %9"PRId64" ,%s, %6.3f, %s/byte\n",4096, MinT, getTimerUnit(), MinT*1.0/(4096), getTimerUnit());
}


void testWBCFunc(void)
{
    testWBC_512();
    testWBC_4096();
}



