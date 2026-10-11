/* Minimal compiler-generation candidates, NOT historical Ravenloft source.
   All external references intentionally remain unresolved in the OMF .OBJ. */
typedef unsigned long u32;
extern volatile u32 f0cc;
extern volatile u32 f0c8;
extern volatile u32 g2b684;
extern volatile u32 g2b680;

void __cdecl set_f0cc(u32 value) { f0cc = value; }
void __cdecl set_f0c8(u32 value) { f0c8 = value; }
u32 __cdecl get_2b684(void) { return g2b684; }
u32 __cdecl get_2b680(void) { return g2b680; }
void __cdecl call_f0c8(void) {
    if (f0c8 != 0UL)
        ((void (__cdecl *)(void))f0c8)();
}
