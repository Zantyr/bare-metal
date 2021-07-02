#define MNEMOBYTE 1
#define MNEMOINT  2
#define MNEMOREG  3
#define MNEMOADDR 4
 
#define MNEMOHOWMUCH 16

typedef struct Mnemo {
    char mnemo[6];
    char spec[4];
    char argOneType;
    char argTwoType;
    char patternLen;
    char *pattern;
    char argOnePos;
    char argTwoPos;
} Mnemo;

Mnemo allMnemonics[MNEMOHOWMUCH] = {
    {"mov", "a", MNEMOBYTE, 0, 2, "\xb0\x00", 1, 0},
    {"mov", "b", MNEMOBYTE, 0, 2, "\xb3\x00", 0, 0},
}

// FStream
// write(int howMuch, char* source);

void assemble(Mnemo* array, int howMuch, FStream out) {
    char buffer[12];
    for(int i = 0; i < howMuch; i++){
        Mnemo mnemo = array[i];
        out.write(mnemo.patternLen, buffer);
        for(int j=0; j < mnemo.patternLen, j++){

        }
    }
}




add with carry
add
call
clear carry
cmpsb
cmpsw - compare words
cmp - compare (registers?)
inc
dec
hlt
idiv, imul
<int, iter>
jmp
j??
mv
lodsb
lodsw
movsb - moves between memory
mul, div - unsigned
nop
or
pop


Little endian:
0x12345678 == <78563412>
int == 32bit

mv  eax, <int>    0xb8<int>
and
