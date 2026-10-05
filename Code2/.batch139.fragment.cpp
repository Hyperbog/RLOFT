
// Batch 139: verified CODE.2 3968E..39836 command-line vector construction.
struct Runtime139Boundary {
    u32 (*alloc)(u32)=nullptr;
};

// 39752..39836: count or split one DOS command-line byte string. When argv is
// zero the source is only scanned; otherwise tokens are compacted in place and
// their addresses are published. Quotes delimit a token without being copied;
// backslash escapes quote in either mode and also escapes backslash in quoted
// mode. endp receives the delimiter/NUL address where the scan stopped.
[[nodiscard]] u32 function_39752(Machine& m,u32 src,u32 argv,u32 endp) noexcept {
    u32 p=src,count=0u;
    for(;;){
        u8 ch=*dg(m,p);
        while(ch==0x20u||ch==0x09u){++p;ch=*dg(m,p);}
        if(ch==0u)break;
        u32 token=p,out=p;
        if(ch==0x22u){
            ++p; token=p; out=p;
            for(;;){
                ch=*dg(m,p);
                if(ch==0x22u||ch==0u)break;
                if(ch==0x5Cu){
                    const u8 next=*dg(m,p+1u);
                    if(next==0x22u||next==0x5Cu)++p;
                }
                const u32 nextp=p+1u;
                if(argv!=0u){*dg(m,out)=*dg(m,p);++out;}
                p=nextp;
            }
        }else{
            token=p; out=p;
            for(;;){
                ch=*dg(m,p);
                if(ch==0u||ch==0x20u||ch==0x09u)break;
                if(ch==0x5Cu&&*dg(m,p+1u)==0x22u)++p;
                const u32 nextp=p+1u;
                if(argv!=0u){*dg(m,out)=*dg(m,p);++out;}
                p=nextp;
            }
        }
        const u32 next=p+1u;
        ++count;
        if(argv!=0u){
            store_le32(dg(m,argv+(count-1u)*4u),token);
            ch=*dg(m,p);
            *dg(m,out)=0u;
            if(ch==0u)break;
        }else if(*dg(m,p)==0u)break;
        p=next;
    }
    store_le32(dg(m,endp),p);
    return count;
}
static_assert(0x39837u-0x39752u==229u);

// 3968E..39751: duplicate the published command line, build argv immediately
// after it, prefix argv with 1263C, terminate the vector, and publish argc/argv
// through 2BD00/2BD04. Allocation failure publishes a null vector/count.
void function_3968e(Machine& m,Runtime139Boundary& b) noexcept {
    const u32 source=load_le32(dg(m,0x12638u));
    u32 endp=0u;
    const u32 parsed=function_39752(m,source,0u,0x12654u);
    endp=load_le32(dg(m,0x12654u));
    const u32 argc=parsed+1u;
    const u32 text_bytes=endp-source+1u;
    const u32 vector_bytes=argc*4u;
    const u32 total=text_bytes+vector_bytes+4u;
    const u32 base=b.alloc?b.alloc(total):0u;
    if(base==0u){
        store_le32(dg(m,0x10600u),0u);
        store_le32(dg(m,0x105FCu),0u);
        store_le32(dg(m,0x2BD00u),argc);
        store_le32(dg(m,0x2BD04u),0u);
        return;
    }
    for(u32 i=0;i<text_bytes;++i)*dg(m,base+i)=*dg(m,source+i);
    const u32 vector=base+text_bytes;
    store_le32(dg(m,0x10600u),vector);
    store_le32(dg(m,vector),load_le32(dg(m,0x1263Cu)));
    const u32 args=vector+4u;
    (void)function_39752(m,base,args,0x12654u);
    store_le32(dg(m,vector+vector_bytes),0u);
    store_le32(dg(m,0x105FCu),argc);
    store_le32(dg(m,0x2BD00u),argc);
    store_le32(dg(m,0x2BD04u),vector);
}
static_assert(0x39752u-0x3968Eu==196u);
