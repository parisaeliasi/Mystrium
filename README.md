# Mystrium
Code for Mystrium

Benchmarking the deck function, MAX: 
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c
gcc -mfpu=neon -c deckFunc.s -o deckFunc.o
gcc -g -o output *.o
./output



Benchmarking the WBC, Mystrium
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c
gcc -mfpu=neon -c deckFunc.s -o deckFunc.o
gcc -mfpu=neon -c Multimixer128.s -o Multimixer128.o
gcc -g -o output *.o
./output



Benchmarking the WBC with key scheduling, Mystrium
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c
gcc -mfpu=neon -c deckFunc.s -o deckFunc.o
gcc -mfpu=neon -c Multimixer128.s -o Multimixer128.o
gcc -mfpu=neon -c keyFunc.s -o keyFunc.o
gcc -g -o output *.o
./output


Benchmarking Adiantum
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c aes-cipher-core.S -o aes-cipher-core.o
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c aes-neonbs-core.S -o aes-neonbs-core.o
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c chacha-neon-core.S -o chacha-neon-core.o
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c chacha-scalar.S -o chacha-scalar.o
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c nh-neon-core.S -o nh-neon-core.o
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c poly1305-armv4.S -o poly1305-armv4.o
gcc -g -o output *.o
./output
