#include "MouseDispatch.hpp"
namespace rloft::input::mouse_dispatch {
void Mailbox::publish(Sample sample) noexcept {
    std::lock_guard<std::mutex> guard(mutex_);
    if(sample.buttons>3u || sample.x>0x27Eu || sample.y>0x18Fu) {
        invalid_=true;
        return;
    }
    if(sample.buttons!=latest_.buttons) {
        if(count_==kCapacity)invalid_=true;
        else {
            edges_[write_]=sample;
            write_=(write_+1u)%kCapacity;
            ++count_;
        }
    }
    latest_=sample;
}
void Mailbox::cancel_buttons() noexcept {
    std::lock_guard<std::mutex> guard(mutex_);
    // Focus loss must not replay undelivered click-down events.
    read_=write_=count_=0u;
    latest_.buttons=0u;
}
bool Mailbox::consume(Sample& sample) noexcept {
    std::lock_guard<std::mutex> guard(mutex_);
    if(invalid_)return false; // Fail closed if an edge was lost.
    if(count_) {
        sample=edges_[read_];
        read_=(read_+1u)%kCapacity;
        --count_;
    }else sample=latest_;
    return true;
}
}
