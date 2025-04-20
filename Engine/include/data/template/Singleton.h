#pragma once

#include <cassert>
#include <memory>

template <typename T>
class Singleton
{
public:
	static T& Get()
	{
		if (instance == nullptr)
			instance = std::make_unique<T>();
		return *instance;
	}

protected:
	virtual void initialize()
	{
		if (isInitialized)
			assert(false && "Singleton has already been initialized");
		isInitialized = true;
	}

	static std::unique_ptr<T> instance;
	bool isInitialized;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;