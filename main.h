#ifndef AVIUTL_DISCORD_RPC_MAIN
#define AVIUTL_DISCORD_RPC_MAIN
#include <Windows.h>
#include "aviutl-sdk/filter.h"
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);
void mem_alloc(FILTER*);
void mem_free(void);
BOOL func_init(FILTER*);
BOOL func_exit(FILTER*);
BOOL func_update(FILTER*);
BOOL func_save_start(FILTER*);
BOOL func_save_end(FILTER*);
BOOL func_modify_title(FILTER*);
BOOL func_WndProc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);
#endif // !AVIUTL_DISCORD_RPC_MAIN
