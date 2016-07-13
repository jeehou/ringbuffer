#include "ring_buffer.h"
#include "atomic_ops.h"
#include <memory.h>
#include <assert.h>
#include <stdio.h>

/* peek操作说明，当push数据大于m_data_buffer的剩余数据，为了避免将数据分放到不连续内存，
 * 会将m_data_buffer剩余数据写一个MAGIC_PEEK标识数据不再使用，真实数据从m_data_buffer的
 * 开始位置存放（如果存放的下）。
 */

#define MAGIC_PEEK ((unsigned int)-1)

RingBuffer::RingBuffer(unsigned int size, bool real_size, bool real_count):
m_b_real_size(real_size), m_b_real_count(real_count),m_tail(0), m_head(0),
m_real_size(0), m_real_count(0), m_size(size) 
{}
	
RingBuffer::~RingBuffer(){
}

unsigned int RingBuffer::countToIndex(unsigned int value){
	return (value & (m_size - 1));
}

int RingBuffer::push(const void *p_buf, unsigned int len, const void *p_mem){
	unsigned int peek_len = MAGIC_PEEK;
	unsigned int cur_head = m_head;
	unsigned int cur_tail = m_tail;
	unsigned char *p_base = (unsigned char*)const_cast<void*>(p_mem);
	if(cur_head - cur_tail + len + sizeof(len) > m_size){
        //printf("full cur_head %u len %d cur_tail %u m_size %d \n",cur_head,len,cur_tail,m_size);
		return -1; //full
	}
	if(countToIndex(m_head) + len + sizeof(len) > m_size){ //for peek
		memcpy(p_base+countToIndex(cur_head), &peek_len, sizeof(len));
		AtomicAdd(&m_head, m_size - countToIndex(m_head));
		return push(p_buf, len, p_mem);
	}else{
		memcpy(p_base+countToIndex(cur_head), &len, sizeof(len));
		memcpy(p_base+countToIndex(cur_head)+sizeof(len), p_buf, len);
		AtomicAdd(&m_head, len+sizeof(len));
		if(m_b_real_count) AtomicAdd(&m_real_count, 1);
		if(m_b_real_size) AtomicAdd(&m_real_size, len+sizeof(len));
		return 0;
	}
}

int RingBuffer::pop(void *p_buf, unsigned int *len, const void *p_mem){
	unsigned int cur_max_read;
	unsigned int cur_read;
	unsigned int in_len = *len;
	unsigned char *p_base = (unsigned char*)const_cast<void*>(p_mem);
	do{
		cur_read = m_tail;
		cur_max_read = m_head;
		if(cur_read == cur_max_read){
			return -1; //empty
		}
		*len = *(unsigned int*)(p_base + countToIndex(cur_read));
		if(*len > in_len && *len != MAGIC_PEEK) return -2; //buffer too small
		if(*len != MAGIC_PEEK){
			memcpy(p_buf, p_base + countToIndex(cur_read) + sizeof(unsigned int), *len);
			if(CAS(&m_tail, cur_read, cur_read + *len + sizeof(unsigned int))){
				if(m_b_real_count) AtomicAdd(&m_real_count, -1);
				if(m_b_real_size) AtomicAdd(&m_real_size, -(long)(*len+sizeof(unsigned int)));
				return 0;
			}
		}else{
            //printf("PEEK! \n");
			if(CAS(&m_tail, cur_read, cur_read + m_size - countToIndex(cur_read))){
				return pop(p_buf, len, p_mem);
			}	
		}
	}while(1);
	return -1;
}

void* RingBuffer::peek(unsigned int *len, unsigned int offset, const void *p_mem){
	unsigned int cur_max_read;
	unsigned int cur_read;
	void *p_ret = NULL;
	unsigned char *p_base = (unsigned char*)const_cast<void*>(p_mem);
	do{
		cur_read = m_tail;
		if(offset != 0) cur_read += (offset + sizeof(unsigned int));
		cur_max_read = m_head;
		if(cur_read == cur_max_read){
			return NULL; //empty
		}
		*len = *(unsigned int*)(p_base + countToIndex(cur_read));
		if(*len != MAGIC_PEEK){
			p_ret = p_base + countToIndex(cur_read) + sizeof(len);
			//if(CAS(&m_tail, cur_read, cur_read)){
				return p_ret;
			//}
		}else{
			if(CAS(&m_tail, cur_read, cur_read + m_size - countToIndex(cur_read))){
				return peek(len, offset, p_mem);
			}	
		}
	}while(1);
	return NULL;
}

int RingBuffer::remove(const void *p_mem){
	unsigned int cur_max_read;
	unsigned int cur_read;
	unsigned int len;
	//void *p_ret = NULL;
	unsigned char *p_base = (unsigned char*)const_cast<void*>(p_mem);
	do{
		cur_read = m_tail;
		cur_max_read = m_head;
		if(cur_read == cur_max_read){
			return NULL; //empty
		}
		len = *(unsigned int*)(p_base + countToIndex(cur_read));
		if(len != MAGIC_PEEK){
			if(CAS(&m_tail, cur_read, cur_read + len + sizeof(len))){
				if(m_b_real_count) AtomicAdd(&m_real_count, -1);
				if(m_b_real_size) AtomicAdd(&m_real_size, -(long)(len+sizeof(len)));
				return 0;
			}
		}else{
			if(CAS(&m_tail, cur_read, cur_read + m_size - countToIndex(cur_read))){
				return remove(p_mem);
			}	
		}
	}while(1);
	return -1;
}

unsigned int RingBuffer::size(){
	return m_real_size;
}

unsigned int RingBuffer::count(){
	return m_real_count;
}

void RingBuffer::dump(){
	unsigned int cur_read;
	unsigned int cur_max_read;
	do{
		cur_read = m_tail;
		cur_max_read = m_head;
		if(countToIndex(cur_read) == countToIndex(cur_max_read)){
			return ;
		}
	}while(1);
}

void RingBuffer::setId(int id){
	m_id = id;
}

int RingBuffer::getId(){
	return m_id;
}

unsigned int RingBuffer::capacity() const{
	return m_size - sizeof(unsigned int);
}
