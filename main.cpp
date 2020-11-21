//---------------------------------------------------------------------
//      AviUtlでDiscord RPCを表示するプラグイン
//---------------------------------------------------------------------

// インクルード
#include <Windows.h>
#include <string>
#include "aviutl-sdk/filter.h"
#include "main.h"
#include "discord-files/discord.h"

typedef struct {
	short x, y;
} Vector2;

// バッファー
static void* buf0 = NULL;
// RPCプラグインが有効かどうか
static BOOL RPC_Enabled = TRUE;
// セーブ中(エンコード中)かどうか
static BOOL IS_SAVING = FALSE;
// 現在編集中のファイル名
static LPSTR FILE_NAME = NULL;
// タイマーの識別子
static UINT_PTR timer_identifer = NULL;
// RPCが破棄されているかどうか
static BOOL IS_Disposed = TRUE;
// Discord Core
discord::Core* core{};
// Discord Acticity
discord::Activity activity{};

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
TCHAR   FILTER_NAME[]          = "AviUtl Discord RPC";
#define CHECK_NUM 1
TCHAR  *CHECKBOX_NAMES[]       = { "有効にする" };
int     CHECKBOX_INITIAL_VAL[] = { 0 };
TCHAR   FILTER_INFO[]          = "AviUtl Discord RPC Plugin version 0.99b by mtripg6666tdr";

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
	/*外部関数テーブル*/NULL,/*システム使用*/NULL,NULL,NULL,/*I解除プラグインのみ*/NULL,
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
//		処理関数
//---------------------------------------------------------------------
BOOL func_proc(FILTER* fp, FILTER_PROC_INFO* fpip) {
	return TRUE;
}

//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
BOOL func_init(FILTER* fp) {
//	MessageBox(NULL, "テスト", "テスト", MB_ICONINFORMATION);
	UINT_PTR timer = SetTimer(
		// ウインドウハンドル
		// *今回はプラグインのウインドウが表示されているとは限らないためNULL
		NULL,
		// タイマーID
		// 上と同じ。
		NULL,
		// タイマー間隔
		RPC_UPDATE_TICK,
		// コールバック関数
		func_timer_tick
	);
	if (timer) {
		timer_identifer = timer;
	}
	else {
		throw;
	}
	return TRUE;
}

//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit(FILTER* fp) {
	mem_free();
	KillTimer(NULL, timer_identifer);
	Dispose_RPC();
	return TRUE;
}

//---------------------------------------------------------------------
//		設定変更
//---------------------------------------------------------------------
BOOL func_update(FILTER* fp, int status) {
	switch (fp->check[0]) {
	case FILTER_CHECKBOX_STATUS_ON:
		RPC_Enabled = TRUE;
		break;
	case FILTER_CHECKBOX_STATUS_OFF:
		RPC_Enabled = FALSE;
		break;
	}
	return TRUE;
}

//---------------------------------------------------------------------
//		出力開始
//---------------------------------------------------------------------
BOOL func_save_start(FILTER* fp, int s, int e, void* editP) {
	IS_SAVING = TRUE;
	return TRUE;
}

//---------------------------------------------------------------------
//		出力終了
//---------------------------------------------------------------------
BOOL func_save_end(FILTER* fp, void* editP) {
	IS_SAVING = FALSE;
	return TRUE;
}

//---------------------------------------------------------------------
//		ウインドウタイトル変更
//---------------------------------------------------------------------
BOOL func_modify_title(FILTER* fp, void* editP, int frame, LPSTR title, int max_title) {
	//Display_RPC(fp, editP);
	return TRUE;
}

//---------------------------------------------------------------------
//		タイマーティック
//---------------------------------------------------------------------
void __stdcall func_timer_tick(HWND hWnd, UINT uMsg, UINT_PTR timer_id, DWORD dwTime) {
	Display_RPC(NULL, NULL);
}

//---------------------------------------------------------------------
//		ウインドウプロシージャ
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* editPtr, FILTER* filterPtr) {
	FILE_INFO* fi = NULL;

	switch (message) {
	/*case WM_FILTER_CHANGE_EDIT:
		core->RunCallbacks();
		break;*/
	case WM_FILTER_CHANGE_WINDOW:
		if (filterPtr->exfunc->is_filter_window_disp(filterPtr)) {
			mem_alloc(filterPtr);
			return TRUE;
		}
		else {
			mem_free();
		}
		break;
	case WM_FILTER_FILE_OPEN:
		if (filterPtr->exfunc->get_file_info(editPtr, fi)) {
			FILE_NAME = fi->name;
		}
		else {
			FILE_NAME = NULL;
		}
		break;
	case WM_FILTER_FILE_CLOSE:
		FILE_NAME = NULL;
		core->RunCallbacks();
		break;
	}
	return FALSE;
}

//---------------------------------------------------------------------
//		Discord RPC 設定関数
//---------------------------------------------------------------------
#define FILTER_RPC_CLIENT_ID 779296320019497020
BOOL Initialize_RPC() {
	if (!RPC_Enabled) {
		return false;
	}
	if (IS_Disposed) {
		IS_Disposed = FALSE;
		discord::Core::Create(FILTER_RPC_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &core);
		activity.SetState(u8"起動中");
		//activity.SetDetails("");
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			
		});
		Display_RPC(NULL, NULL);
	}
	else {
		throw;
	}
	return TRUE;
}
BOOL Display_RPC(FILTER* fp, void* editPtr) {
	if (!RPC_Enabled) {
		return FALSE;
	}
	if (!IS_Disposed) {
		std::string State = "";
		if (IS_SAVING) {
			State = u8"エンコード中";
		}else if (FILE_NAME == NULL) {
			State = u8"アイドル中";
		}
		else {
			State = u8"編集中";
		}
		activity.SetState(State.c_str());
		if (FILE_NAME != NULL) {
			activity.SetDetails(FILE_NAME);
		}
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			
		});
		core->RunCallbacks();
	}
	else {
		Initialize_RPC();
	}
	return TRUE;
}
BOOL Dispose_RPC() {
	IS_Disposed = TRUE;
	return TRUE;
}