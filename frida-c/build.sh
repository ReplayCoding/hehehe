g++ \
  -fpermissive \
  -Wpointer-arith \
  -Wall \
  -shared \
  -fPIC \
  -Isource-sdk-2013/mp/src/mathlib \
  -Isource-sdk-2013/mp/src/public \
  -Isource-sdk-2013/mp/src/common \
  -Isource-sdk-2013/mp/src/public/tier0 \
  -Isource-sdk-2013/mp/src/public/tier1 \
  -Isource-sdk-2013/mp/src/tier1 \
  -Isource-sdk-2013/mp/src \
  frida-gum-example.cpp \
  -o \
  test.so \
  -L. \
  -lfrida-gum
