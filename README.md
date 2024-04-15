# Mystrium<br />
Code for Mystrium<br />

Benchmarking the deck function, MAX: <br />
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c<br />
gcc -mfpu=neon -c deckFunc.s -o deckFunc.o<br />
gcc -g -o output *.o<br />
./output<br />



Benchmarking the WBC, Mystrium<br />
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c<br />
gcc -mfpu=neon -c deckFunc.s -o deckFunc.o<br />
gcc -mfpu=neon -c Multimixer128.s -o Multimixer128.o<br />
gcc -g -o output *.o<br />
./output<br />



Benchmarking the WBC with key scheduling, Mystrium<br />
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c<br />
gcc -mfpu=neon -c deckFunc.s -o deckFunc.o<br />
gcc -mfpu=neon -c Multimixer128.s -o Multimixer128.o<br />
gcc -mfpu=neon -c keyFunc.s -o keyFunc.o<br />
gcc -g -o output *.o<br />
./output<br />


Benchmarking Adiantum<br />
gcc -march=armv7-a -marm -mfpu=neon -g *.c -c<br />
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c aes-cipher-core.S -o aes-cipher-core.o<br />
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c aes-neonbs-core.S -o aes-neonbs-core.o<br />
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c chacha-neon-core.S -o chacha-neon-core.o<br />
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c chacha-scalar.S -o chacha-scalar.o<br />
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c nh-neon-core.S -o nh-neon-core.o<br />
gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -c poly1305-armv4.S -o poly1305-armv4.o<br />
gcc -g -o output *.o<br />
./output<br />
