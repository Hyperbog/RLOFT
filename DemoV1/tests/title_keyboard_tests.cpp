#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/title_keyboard.hpp"
#include <cassert>
int main() {
    using rloft::TitleKeyboardModel;
    auto r=TitleKeyboardModel::key_down(0x28U,-1);
    assert(r.highlighted==0 && !r.activate);
    for(int n=1;n<4;++n){
        r=TitleKeyboardModel::key_down(0x28U,r.highlighted);
        assert(r.highlighted==n && !r.activate);
    }
    r=TitleKeyboardModel::key_down(0x28U,3);
    assert(r.highlighted==0);
    r=TitleKeyboardModel::key_down(0x26U,0);
    assert(r.highlighted==3);
    r=TitleKeyboardModel::key_down(0x24U,3);
    assert(r.highlighted==0);
    r=TitleKeyboardModel::key_down(0x23U,0);
    assert(r.highlighted==3);
    r=TitleKeyboardModel::key_down(0x0DU,-1);
    assert(!r.activate);
    r=TitleKeyboardModel::key_down(0x0DU,2);
    assert(r.activate && *r.activate==2);
    r=TitleKeyboardModel::key_down(0x0DU,2,true);
    assert(!r.activate);
    for(int n=0;n<4;++n){
        r=TitleKeyboardModel::key_down(static_cast<unsigned>('1'+n),0);
        assert(r.highlighted==n && r.activate && *r.activate==n);
        r=TitleKeyboardModel::key_down(0x61U+static_cast<unsigned>(n),0);
        assert(r.highlighted==n && r.activate && *r.activate==n);
        r=TitleKeyboardModel::key_down(0x31U+static_cast<unsigned>(n),0,true);
        assert(!r.activate);
    }
    r=TitleKeyboardModel::key_down('Q',3);
    assert(r.highlighted==3 && !r.activate);
    r=TitleKeyboardModel::key_down(0x26U,99);
    assert(r.highlighted==0);
    return 0;
}
