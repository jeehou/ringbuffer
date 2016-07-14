# ringbuffer
Crossplatform lock free ringbuffer.

How to use?
===========
just include atomic_ops.h ring_buffer.h ring_buffer.cpp to your Makefile or other else

Sample
======
How to push
----
```c++
void *p_mem = malloc(RB_SIZE);
RingBuffer rb(RB_SIZE, false, false);
rb_ctx ctx;
rb.push(&ctx, sizeof(ctx), p_mem);
```

How to pop
----------
```c++
rb_ctx ctx;
unsigned int len = 0;
rb.pop(&ctx, &len, p_mem); //p_mem same buffer as push
```

And an other way to get data
----------------------------
For better performarce, you can get data without copy it to an other buffer.
```c++
unsigned int len = 0;
rb_ctx *p_ctx = (rb_ctx*)rb.peek(&len, 0, p_mem);
```

Also more example about how to use, please refer to [test_ring_buffer.cpp](test_ring_buffer.cpp)
