#pragma once

#include<memory>
#include<vector>

template<typename T>
class Ring
{
private:
	std::shared_ptr<Ring> ref;
	std::weak_ptr<Ring> next;
	std::weak_ptr<Ring> back;

	friend std::shared_ptr<Ring<T>> MakeRing(const std::vector<std::shared_ptr<Ring<T>>> &ring_ptrs)
	{
		using namespace std;
		pRing<T> ret;
		auto length = size(ring_ptrs);

		auto &ptr = ret;
		ptr = ring_ptrs.front();
		ptr->ref = ptr;
		for (decltype(length) i = 0; i < length; ++i)
		{
			ptr->next = ring_ptrs[(i + 1) % length];
			ptr->next.lock()->ref = ptr->next.lock();
			ptr->next.lock()->back = (ring_ptrs[(i + length) % length]);
			ptr = ptr->next.lock();
		}

		return ret;
	}
public:
	T content;
	
	std::shared_ptr<Ring> Next() 
	{
		return this->next.lock();
	}

	std::shared_ptr<Ring> Back()
	{
		return this->back.lock();
	}
};

template<typename T>
using pRing = std::shared_ptr<Ring<T>>;

