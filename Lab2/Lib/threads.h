#pragma once
#include <windows.h>
#include "utils.h"

// Единые функции логики
void min_max_logic(ThreadData* data);
void average_logic(ThreadData* data);

// Обертки для WinAPI
DWORD WINAPI min_max_win(LPVOID lpParam);
DWORD WINAPI average_win(LPVOID lpParam);