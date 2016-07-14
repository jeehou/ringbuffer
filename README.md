# ringbuffer
Crossplatform lock free ringbuffer.

Sample
======
how to push
----
```c++
void *p_mem = malloc(RB_SIZE);
RingBuffer rb(RB_SIZE, false, false);
rb_ctx ctx;
rb.push(&ctx, sizeof(ctx), p_mem);
```

how to pop
----------
```c++
rb_ctx ctx;
unsigned int len = 0;
rb.pop(&ctx, &len, p_mem); //p_mem same buffer as push
```

