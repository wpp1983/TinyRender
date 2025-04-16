
///
#define BX_MACRO_BLOCK_BEGIN for(;;) {
#define BX_MACRO_BLOCK_END break; }
#define BX_NOOP(...) BX_MACRO_BLOCK_BEGIN BX_MACRO_BLOCK_END