#ifndef AVIUTL_DISCORD_RPC_MAIN
#define AVIUTL_DISCORD_RPC_MAIN
#include <Windows.h>
#include "aviutl-sdk/filter.h"

void mem_alloc(FILTER*);
void mem_free(void);
BOOL func_init(FILTER*);
BOOL func_exit(FILTER*);
BOOL func_update(FILTER*);
BOOL func_modify_title(FILTER*);
BOOL func_WndProc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);
BOOL Initialize_RPC();
BOOL Dispose_RPC();
BOOL Update_RPC(FILTER*,void*, int, bool);
void __stdcall func_timer_tick(HWND, UINT, UINT_PTR, DWORD);

// チェックボックスのステータスがオフ
#define FILTER_CHECKBOX_STATUS_OFF 0
// チェックボックスのステータスがオン
#define FILTER_CHECKBOX_STATUS_ON 1

// ステータスを表すフラグ

// ステータスが「編集中」
#define RPC_STATUS_EDITING 1
// ステータスが「アイドル中」
#define RPC_STATUS_IDLING 2
// ステータスが「エンコード中」
#define RPC_STATUS_SAVING 3

// タイマー間隔
#define RPC_UPDATE_TICK 1000

#define WM_FILTER_CHANGE_PARAM_POST_EVENT (WM_USER+10011)
#endif // !AVIUTL_DISCORD_RPC_MAIN
