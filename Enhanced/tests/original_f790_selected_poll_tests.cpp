#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_selected_poll.hpp"
#include <cassert>
#include <cstdint>

int main() {
    using Poll=rloft::OriginalF790SelectedPoll;
    static_assert(Poll::kSourceE8==0x130deU,"original E8 site");
    static_assert(*rloft::OriginalF790CallerCatalog::arguments_at(0x130deU)==
                  rloft::OriginalF790ArgumentOrigin::SelectedKnownZero,
                  "source selected/known-zero E8");
    Poll loop;
    assert(loop.previous_selected()==-1);
    assert(!loop.poll_130a9(true,7,12)); // done: never F790
    assert(loop.previous_selected()==-1);
    assert(!loop.poll_130a9(false,-1,12)); // initial old -1
    auto event=loop.poll_130a9(false,3,12);
    assert(event && event->e8_offset==0x130deU);
    assert(event->item_id==3 && !event->known);
    assert(loop.previous_selected()==3);
    assert(!loop.poll_130a9(false,3,12));
    assert(!loop.poll_130a9(true,5,12));
    assert(loop.previous_selected()==3);
    event=loop.poll_130a9(false,20,12);
    assert(event && event->item_id==12);
    assert(loop.previous_selected()==12);
    // A raw 20 is still different from ESI=12; original redraws
    // on EACH poll, not only when RAW 9E33 changes.
    for(int i=0;i<1000;++i) {
        event=loop.poll_130a9(false,20,12);
        assert(event && event->item_id==12 && !event->known);
        assert(loop.previous_selected()==12);
    }
    assert(!loop.poll_130a9(false,12,12));
    event=loop.poll_130a9(false,-2,12);
    assert(event && event->item_id==-2); // original has no low clamp
    assert(loop.previous_selected()==-2);
    event=loop.poll_130a9(false,-1,12);
    assert(event && event->item_id==-1);
    assert(!loop.poll_130a9(false,-1,12));
    loop.enter_loop_130a0();
    assert(!loop.poll_130a9(false,-1,12));
    assert(loop.poll_130a9(false,0,12)->item_id==0);
}
