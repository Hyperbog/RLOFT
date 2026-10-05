
// Batch 139: verified CODE.2 3968E..39836 command-line vector construction.
struct Runtime139Boundary { u32 (*alloc)(u32)=nullptr; };

[[nodiscard]] u32 function_39752(Machine& m,u32 src,u32 argv,u32* endp) noexcept {
    u32 p=src,count=0u;
    for(;;){
        u8 ch=*dg(m,p);
        while(ch==0x20u||ch==0x09u){++p;ch=*dg(m,p);}
        if(ch==0u)break;
        u32 token=p,out=p;
        if(ch==0x22u){
            ++p; token=p; out=p;
            for(;;){
                ch=*dg(m,p); if(ch==0x22u||ch==0u)break;
                if(ch==0x5Cu){const u8 n=*dg(m,p+1u);if(n==0x22u||n==0x5Cu)++p;}
                const u32 next=p+1u; if(argv!=0u){*dg(m,out)=*dg(m,p);++out;} p=next;
            }
        }else{
            token=p; out=p;
            for(;;){
                ch=*dg(m,p); if(ch==0u||ch==0x20u||ch==0x09u)break;
                if(ch==0x5Cu&&*dg(m,p+1u)==0x22u)++p;
                const u32 next=p+1u; if(argv!=0u){*dg(m,out)=*dg(m,p);++out;} p=next;
            }
        }
        const u32 next=p+1u; ++count;
        if(argv!=0u){store_le32(dg(m,argv+(count-1u)*4u),token);ch=*dg(m,p);*dg(m,out)=0u;if(ch==0u)break;}
        else if(*dg(m,p)==0u)break;
        p=next;
    }
    if(endp)*endp=p; return count;
}
static_assert(0x39837u-0x39752u==229u);

void function_3968e(Machine& m,Runtime139Boundary& b) noexcept {
    const u32 source=load_le32(dg(m,0x12638u)); u32 endp=0u;
    const u32 parsed=function_39752(m,source,0u,&endp); const u32 argc=parsed+1u;
    const u32 text_bytes=endp-source+1u; const u32 vector_bytes=argc*4u;
    const u32 base=b.alloc?b.alloc(text_bytes+vector_bytes+4u):0u;
    if(base==0u){store_le32(dg(m,0x10600u),0u);store_le32(dg(m,0x105FCu),0u);store_le32(dg(m,0x2BD00u),argc);store_le32(dg(m,0x2BD04u),0u);return;}
    for(u32 i=0;i<text_bytes;++i)*dg(m,base+i)=*dg(m,source+i);
    const u32 vector=base+text_bytes; store_le32(dg(m,0x10600u),vector);
    store_le32(dg(m,vector),load_le32(dg(m,0x1263Cu)));
    (void)function_39752(m,base,vector+4u,&endp);
    store_le32(dg(m,vector+vector_bytes),0u); store_le32(dg(m,0x105FCu),argc);
    store_le32(dg(m,0x2BD00u),argc); store_le32(dg(m,0x2BD04u),vector);
}
static_assert(0x39752u-0x3968Eu==196u);
