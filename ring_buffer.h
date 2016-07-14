#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H
#pragma once

/// @brief one producer and N consumer!
class RingBuffer{
public:
	/// @brief WARNING£ºsize must pow2
	///        if left size of count in buffer is considered, please set true
	///        for better performance£¬real_size and real_count may set false
	RingBuffer(unsigned int size, bool real_size = true, bool real_count = true);
	
	~RingBuffer();

	/// @brief success:0£¬fail:-1
	int push(const void *p_buf, unsigned int len, const void *p_mem);

	/// @brief success:0£¬fail:-1
	int pop(void *p_buf, unsigned int *len, const void *p_mem);

	/// @brief fail:NULL
    //         WARNING:ONLY SUPPORT ONE READER AND ONE WRITTER!
	void* peek(unsigned int *len, unsigned int offset = 0, const void *p_mem = 0);

	/// @brief for use after peek
	///        delete tail
	///        success:0£¬fail:-1
	int remove(const void *p_mem);

	/// @brief for debug use
	///        this function may called by producer and consumer
	void dump();

	unsigned int capacity() const;

	/// @brief return the real size(in byte) left in buffer
	unsigned int size();

	/// @brief return the num of data left in buffer
	unsigned int count();

	void* getMemory();

	void  setMemory(void *p_buf);

	void setId(int id);

	int getId();

private:

	unsigned int countToIndex(unsigned int value);
private:
	int                     m_id;
	const bool              m_b_real_size;
	const bool              m_b_real_count;
	unsigned int            m_tail;
	unsigned int            m_head;
	unsigned int            m_real_size;
	unsigned int            m_real_count;
	unsigned int            m_size;
};

#endif //__RING_BUFFER_H
