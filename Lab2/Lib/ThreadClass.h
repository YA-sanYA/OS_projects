#pragma once

#include <windows.h>
#include <string>
#include <iostream>

class ThreadClass
{
	HANDLE hThread;
	std::string name;

public:

	ThreadClass(LPTHREAD_START_ROUTINE func, LPVOID lpParam, const std::string& name);

	void wait();

	~ThreadClass();
};

