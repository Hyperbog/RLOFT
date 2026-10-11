/* Source-only diagnostic controls, NOT the original game source. */
typedef unsigned int u32;
extern volatile u32 f0cc, f0c8, g2b684, g2b680;
void set_f0cc(u32 v) { f0cc = v; }
void set_f0c8(u32 v) { f0c8 = v; }
u32 get_2b684(void) { return g2b684; }
u32 get_2b680(void) { return g2b680; }
void call_f0c8(void) { if (f0c8) ((void (*)(void))f0c8)(); }
