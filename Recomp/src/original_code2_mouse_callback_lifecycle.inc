// Original CODE.2 1A064/33A7C callback bridge on ONE retained guest.
// Compiles inside the canonical owner; host validator is independently linked.
void original_20440_mouse_interrupt_39148(u32 intr,u32 regs,
                                           u32 output,u32 segments) noexcept {
    if(!rloft::input::mouse_lifecycle::callback_frame_1a064(
           current->data.data(),current->data.size(),
           intr,regs,output,segments) ||
       !rloft::input::mouse_lifecycle::may_install(
           current->original_20440_mouse_started,
           current->original_20440_mouse_callback_installs))
        stop("ORIGINAL_1A064_INT33_CALLBACK_FRAME_CHANGED");
    rloft::guest_continuation::Reply reply{};
    const rloft::guest_continuation::Request request{
        rloft::guest_continuation::Service::Original20440MouseCallback,
        intr,regs,output,segments,0u};
    if(!current->live_continuation->suspend(request,reply))
        stop("ORIGINAL_1A064_INT33_CALLBACK_SHUTDOWN");
    if(!rloft::input::mouse_lifecycle::accepted_reply(
           current->original_20440_mouse_started,
           current->original_20440_mouse_callback_installs,reply.ax))
        stop("ORIGINAL_1A064_INT33_CALLBACK_HOST_FAILED");
    code2::store_le32(current->data.data()+output,reply.ax);
    ++current->original_20440_mouse_callback_installs;
}
u32 original_20440_mouse_interrupt_result(u32 out) noexcept {
    if(!rloft::input::mouse_lifecycle::result_pointer(
           out,current->data.size()))
        stop("ORIGINAL_1A064_INT33_RESPONSE_POINTER_CHANGED");
    return code2::load_le32(current->data.data()+out);
}
void original_20440_mouse_irq(u32 intr,u32 in,u32 out,u32 seg) noexcept {
    code2::Runtime98Boundary boundary{};
    boundary.call_39148=&original_20440_mouse_interrupt_39148;
    boundary.read_output_eax=&original_20440_mouse_interrupt_result;
    (void)code2::function_33a7c(boundary,intr,in,out,seg);
}
