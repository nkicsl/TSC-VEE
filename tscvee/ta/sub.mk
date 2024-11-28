global-incdirs-y += include
global-incdirs-y += stc
srcs-y += tscvee_ta.c
srcs-y += ./src/sha3.c
srcs-y += ./src/uint256.c

# To remove a certain compiler flag, add a line like this
# cflags-template_ta.c-y += -Wno-strict-prototypes