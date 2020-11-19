//---------------------------------------------------------------------
//      AviUtlでDiscord RPCを表示するプラグイン
//---------------------------------------------------------------------

#include <Windows.h>
#include "aviutl-sdk/filter.h"
#include "main.h"

typedef struct {
	short x, y;
} Vector2;
static void* buf0 = NULL;
static BOOL RPC_Enabled = TRUE;
static BOOL IS_SAVING = FALSE;

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
TCHAR   FILTER_NAME[]          = "AviUtl Discord RPC";
#define CHECK_NUM 1
TCHAR   CHECKBOX_NAME_0[]      = "有効にする";
TCHAR*  CHECKBOX_NAME          = CHECKBOX_NAME_0;
TCHAR** CHECKBOX_NAMES         = &CHECKBOX_NAME;
int     CHECKBOX_INITIAL_VAL[] = { 0 };
TCHAR   FILTER_INFO[]          = "";

FILTER_DLL filter = {
	// flag
	FILTER_FLAG_ALWAYS_ACTIVE | FILTER_FLAG_DISP_FILTER | FILTER_FLAG_EX_INFORMATION,
	// x, y
	NULL, NULL,
	// name
	FILTER_NAME,
	// Trackbar
	NULL, NULL, NULL, NULL, NULL,
	// Checkbox
	CHECK_NUM, CHECKBOX_NAMES, CHECKBOX_INITIAL_VAL,
	// func_proc: フィルタ処理関数
	func_proc,
	// func_init: フィルタ初期化関数
	func_init,
	// func_exit: フィルタ終了関数
	func_exit,
	// func_update: フィルタ設定変更適用関数
	func_update,
	// func_WndProc: ウインドウプロシージャ
	func_WndProc,
	// システム使用部分
	NULL, NULL, 
	// 拡張データ関連
	NULL, NULL,
	// 拡張プラグイン情報
	FILTER_INFO,
	// 出力開始関数
	func_save_start,
	// 出力終了関数
	func_save_end,
	/*外部関数テーブル*/NULL,/*システム使用*/NULL,NULL,NULL,/*Iのみ*/NULL,
	// プロジェクトファイル保存関連
	NULL, NULL, 
	// タイトルバー表示関数
	func_modify_title,
	NULL, NULL
};

//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void) {
	return &filter;
}

//---------------------------------------------------------------------
//		メモリ管理
//---------------------------------------------------------------------
void mem_alloc(FILTER* fp) {
	int size;
	SYS_INFO si;
	fp->exfunc->get_sys_info(NULL, &si);
	size = si.vram_w * si.vram_h * 3;
	if (buf0 == NULL) {
		buf0 = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	}
}
void mem_free(void) {
	if (buf0) {
		VirtualFree(buf0, NULL, MEM_RELEASE);
		buf0 = NULL;
	}
}

//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
BOOL func_init(FILTER* fp) {
	return TRUE;
}

//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit(FILTER* fp) {
	mem_free();
	return TRUE;
}

//---------------------------------------------------------------------
//		設定変更
//---------------------------------------------------------------------
BOOL func_update(FILTER* fp, int status) {

}

//---------------------------------------------------------------------
//		出力開始
//---------------------------------------------------------------------
BOOL func_save_start(FILTER* fp, int s, int e, void* editP) {

}

//---------------------------------------------------------------------
//		出力終了
//---------------------------------------------------------------------
BOOL func_save_end(FILTER* fp, void* editP) {

}

//---------------------------------------------------------------------
//		ウインドウタイトル変更
//---------------------------------------------------------------------
BOOL func_modify_title(FILTER* fp, void* editP, int frame, LPSTR title, int max_title) {
	Display_RPC(fp, editP);
	return TRUE;
}

//---------------------------------------------------------------------
//		ウインドウプロシージャ
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* editPtr, FILTER* filterPtr) {
	switch (message) {
	case WM_PAINT:
	case WM_FILTER_CHANGE_EDIT:
		Display_RPC(filterPtr, editPtr);
		break;
	case WM_FILTER_CHANGE_WINDOW:
		if (filterPtr->exfunc->is_filter_window_disp(filterPtr)) {
			mem_alloc(filterPtr);
			return TRUE;
		}
		else {
			mem_free();
		}
		break;
	}
	return FALSE;
}

//---------------------------------------------------------------------
//		Discord RPC 設定関数
//---------------------------------------------------------------------
BOOL Display_RPC(FILTER* fp, void* editP) {

}