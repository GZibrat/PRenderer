#include "RingBuffer.h"
#include <iostream>

#define TEST_SIZE 16

RingBuffer(test, int, TEST_SIZE)
RB_OPT;
std::mutex readLock;
std::mutex writeLock;
};

struct test rb;

void producer(int id)
{
	int p = id;
	while (1)
	{
		std::unique_lock<std::mutex> headcvLock(rb.writeLock);
		RB_PUT_SAFE(rb, p);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		p += 3;
	}
}

void consumer(int id)
{
	while (1)
	{
		std::unique_lock<std::mutex> tailcvLock(rb.readLock);
		int ele;
		RB_GET_SAFE(rb, ele);
		std::cout << id << ": " << ele << std::endl;
	}
}

int main_test()
{
	RB_INIT(rb, TEST_SIZE);
	std::thread t1(producer, 1);
	std::thread t3(producer, 0);
	std::thread t4(producer, 2);
	std::thread t2(consumer, 2);
	consumer(1);
	return 0;
}