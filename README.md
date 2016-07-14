# ringbuffer
Crossplatform lock free ringbuffer.

Sample
======
'''
void *p_mem = malloc(RB_SIZE);
RingBuffer rb(RB_SIZE, false, false);
rb_ctx ctx;
rb.push(&ctx, sizeof(ctx), p_mem);
'''
