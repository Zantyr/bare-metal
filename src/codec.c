// CODEC - small coding program

/*

codec is an interpreter for commands
it caches and spits out assembly and generally works
as an macro assembles

*/

struct TypeSpec {

};

struct FastSlot {
  char name[16];
  struct TypeSpec typeSpec;
};

struct CodecStruct {
  FastSlot fastSlots[8];
  List(Slot) slots;
};

// define all data as unions of Object
struct Object {};

// data is defined primarily by it's size
typedef union CodecInt {
  struct Object,
  struct CodecInt {
    TypeSpec type;
    int value;
  };
} CodecInt;

// struct CodecObject {}
