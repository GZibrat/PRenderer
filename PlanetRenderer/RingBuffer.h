#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include "PlanetRenderer.h"

/*

Macro Implementation of RingBuffer
To use make a struct with generic type T

RingBuffer(name, T, size)

other variables

};

*/

#define RingBuffer(name, type, size) struct name  \
{												  \
	volatile uint head;									  \
	volatile uint tail;									  \
	uint cap;									  \
	type buffer[size];

#define RB_INIT(rb, size) \
rb.head = 0;			  \
rb.tail = 0;			  \
rb.cap = size;

#define RB_OPT std::condition_variable headcv; \
std::condition_variable tailcv;



#define RB_IS_FULL(rb) \
rb.head == rb.tail + rb.cap

#define RB_IS_EMPTY(rb) \
rb.head == rb.tail

#define RB_PUT_SAFE(rb, ele)             \
while (RB_IS_FULL(rb)) {	             \
	rb.headcv.wait(headcvLock);	         \
}							             \
rb.buffer[rb.head++ & (rb.cap-1)] = ele; \
rb.tailcv.notify_all()

#define RB_GET_SAFE(rb, ele)             \
while (RB_IS_EMPTY(rb)) {		         \
	rb.tailcv.wait(tailcvLock);	         \
}								         \
ele = rb.buffer[rb.tail++ & (rb.cap-1)]; \
rb.headcv.notify_all();

#define RB_PUT(rb, ele, ret)                   \
if (RB_IS_FULL(rb))                            \
	ret = 0;	                               \
else {										   \
	rb.buffer[rb.head++ & (rb.cap - 1)] = ele; \
	ret = 1;								   \
}

#define RB_GET(rb, ele, null)				   \
if (RB_IS_EMPTY(rb))						   \
	ele = null;								   \
else										   \
	ele = rb.buffer[rb.tail++ & (rb.cap - 1)];




