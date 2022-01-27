//---------------------------------------------------------------------
//      AviUtlでDiscord RPCを表示するプラグイン
//---------------------------------------------------------------------
#include <Windows.h>
#include <string>
#include <chrono>
#include <sstream>
#include <istream>
#include "aviutl-sdk/filter.h"
#include "main.h"
#include "discord-files/discord.h"
#include "strutil.h"

// バッファー
static void* buf0 = NULL;
// RPCプラグインが有効かどうか
static BOOL RPC_Enabled = TRUE;
// ファイル名を表示するかどうか
static BOOL RPC_Display_Filename = TRUE;
// ステータス
static int Status = RPC_STATUS_IDLING;
// タイマーの識別子
static UINT_PTR timer_identifer = NULL;
// RPCが破棄されているかどうか
static BOOL IS_Disposed = TRUE;
// システム情報
static std::string SYS_INFO_STR = "";
// 開始時刻が一度でも設定されているか
static BOOL RPC_Timestamp_Set = FALSE;
// 起動時の設定反映がされているか
static BOOL RPC_func_update_fisrt_called = FALSE;

// Discord Core
discord::Core* core{};
// Discord Acticity
discord::Activity activity{};

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
TCHAR FILTER_NAME[] = "AviUtl Discord RPC";
#define CHECK_NUM 2
#if R_ENGLISH
TCHAR* CHECKBOX_NAMES[] = { "Enable RPC", "Show filenames" };
#else
TCHAR* CHECKBOX_NAMES[] = { "有効にする", "ファイル名を表示する" };
#endif
int     CHECKBOX_INITIAL_VAL[] = { 0, 0 };
TCHAR   FILTER_INFO[] = "AviUtl Discord RPC version 0.99e by mtripg6666tdr";
TCHAR   VERSION[] = "0.99e";

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
	NULL,
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
	NULL,
	// 出力終了関数
	NULL,
	/*外部関数テーブル*/NULL,/*システム使用*/NULL,NULL,NULL,/*I解除プラグインのみ*/NULL,
	// プロジェクトファイル保存関連
	NULL, func_project_save,
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
	// コールバックを実行するためのタイマーを設定
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
	SYS_INFO info;
	if (fp->exfunc->get_sys_info(NULL, &info)) {
		SYS_INFO_STR = "AviUtl ";
		SYS_INFO_STR += info.info;
		SYS_INFO_STR += " (RPC v";
		SYS_INFO_STR += VERSION;
		SYS_INFO_STR += ")";
		SYS_INFO_STR = multi_to_utf8_winapi(SYS_INFO_STR);
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
	BOOL initialized = FALSE;
	switch (fp->check[0]) {
	case FILTER_CHECKBOX_STATUS_ON:
		if (!RPC_Enabled || !RPC_func_update_fisrt_called) {
			initialized = RPC_Enabled = TRUE;
			Initialize_RPC();
			Update_RPC(fp, NULL, Status, TRUE);
		}
		break;
	case FILTER_CHECKBOX_STATUS_OFF:
		if (RPC_Enabled) {
			RPC_Enabled = FALSE;
			Dispose_RPC();
		}
		break;
	}
	BOOL fn_now = fp->check[1] == FILTER_CHECKBOX_STATUS_ON;
	if (fn_now != RPC_Display_Filename || initialized) {
		RPC_Display_Filename = fn_now;
		PostMessage(fp->hwnd, WM_FILTER_CHANGE_PARAM_POST_EVENT, NULL, NULL);
	}
	RPC_func_update_fisrt_called = TRUE;
	return TRUE;
}

//---------------------------------------------------------------------
//		プロジェクトファイルの保存
//---------------------------------------------------------------------
BOOL func_project_save(FILTER* fp, void* editp, void* data, int* size) {
	Update_RPC(fp, editp, RPC_STATUS_EDITING, FALSE);
	return FALSE;
}

//---------------------------------------------------------------------
//		ウインドウタイトル変更
//---------------------------------------------------------------------
BOOL func_modify_title(FILTER* fp, void* editP, int frame, LPSTR title, int max_title) {
	return TRUE;
}

//---------------------------------------------------------------------
//		タイマーティック
//      コールバックを実行
//---------------------------------------------------------------------
void __stdcall func_timer_tick(HWND hWnd, UINT uMsg, UINT_PTR timer_id, DWORD dwTime) {
	if (core != NULL && RPC_Enabled) {
		core->RunCallbacks();
	}
}

//---------------------------------------------------------------------
//		ウインドウプロシージャ
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* editPtr, FILTER* filterPtr) {
	if (core != NULL && RPC_Enabled) {
		core->RunCallbacks();
	}

	switch (message) {
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
	case WM_FILTER_SAVE_END:
		Update_RPC(filterPtr, editPtr, RPC_STATUS_EDITING, TRUE);
		break;
	case WM_FILTER_FILE_CLOSE:
		Update_RPC(filterPtr, editPtr, RPC_STATUS_IDLING, TRUE);
		break;
	case WM_FILTER_SAVE_START:
		Update_RPC(filterPtr, editPtr, RPC_STATUS_SAVING, TRUE);
		break;
	case WM_FILTER_CHANGE_PARAM_POST_EVENT:
		Update_RPC(filterPtr, editPtr, Status, FALSE);
		break;
	}
	return FALSE;
}

//---------------------------------------------------------------------
//		Discord RPC 設定関数
//---------------------------------------------------------------------

// DiscordのアプリケーションのクライアントID
#define FILTER_RPC_CLIENT_ID 779296320019497020
BOOL Initialize_RPC() {
	if (!RPC_Enabled) {
		return false;
	}
	if (IS_Disposed) {
		discord::Core::Create(FILTER_RPC_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &core);
		RPC_Timestamp_Set = FALSE;
		IS_Disposed = FALSE;
	}
	return TRUE;
}
BOOL Update_RPC(FILTER* filterPtr, void* editPtr, int status, bool isStart) {
	// ステータスを保存
	Status = status;

	// 無効なら終わり
	if (!RPC_Enabled) {
		return FALSE;
	}
	if (!IS_Disposed) {
		if (core == NULL) {
			return FALSE;
		}
		std::string detail = "";
		if (RPC_Display_Filename && editPtr != NULL && filterPtr != NULL) {
			FILE_INFO fi;
			SYS_INFO si;
			if (filterPtr->exfunc->is_editing(editPtr) &&
				filterPtr->exfunc->get_sys_info(editPtr, &si) && 
				filterPtr->exfunc->get_file_info(editPtr, &fi)
				) {
				detail += fi.name;
				std::string filename;
				std::string rawFileName = std::string(si.project_name);
				if (rawFileName.find('\\') == std::string::npos) {
					filename = rawFileName;
				} else {
					std::stringstream ss{ rawFileName };
					std::string buf;
					while (std::getline(ss, buf, '\\')) {
						filename = buf;
					}
				}
				filename = multi_to_utf8_winapi(filename);
				detail += "(";
#if R_ENGLISH
				detail += filename == "" ? u8"Unsaved" : filename;
#else
				detail += filename == "" ? u8"未保存" : filename;
#endif
				detail += ")";
			}
		}

		std::string StateStr = "";
		// Ref: https://stackoverflow.com/questions/6012663/get-unix-timestamp-with-c
		switch (status) {
		case RPC_STATUS_SAVING:
#if R_ENGLISH
			StateStr = u8"Encoding";
			activity.GetAssets().SetSmallText(u8"Encode");
#else
			StateStr = u8"エンコード中";
			activity.GetAssets().SetSmallText(u8"エンコード");
#endif
			activity.GetAssets().SetSmallImage("status_encoding");
			break;
		case RPC_STATUS_IDLING:
#if R_ENGLISH
			StateStr = u8"Idling";
			activity.GetAssets().SetSmallText(u8"Idle");
#else
			StateStr = u8"アイドル中";
			activity.GetAssets().SetSmallText(u8"アイドル状態");
#endif
			activity.GetAssets().SetSmallImage("status_idle");
			break;
		case RPC_STATUS_EDITING:
#if R_ENGLISH
			StateStr = u8"Editing";
			activity.GetAssets().SetSmallText(u8"Editing");
#else
			StateStr = u8"編集中";
			activity.GetAssets().SetSmallText(u8"編集中");
#endif
			activity.GetAssets().SetSmallImage("status_editing");
			break;
		}
		activity.SetState(StateStr.c_str());
		activity.GetAssets().SetLargeImage("aviutl_icon_large");
		activity.GetAssets().SetLargeText(SYS_INFO_STR == "" ? "AviUtl" : SYS_INFO_STR.c_str());
		if (isStart || !RPC_Timestamp_Set) {
			activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			RPC_Timestamp_Set = TRUE;
		}
		
		activity.SetDetails(detail.c_str());
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			
		});
	}
	else {
		Initialize_RPC();
	}
	return TRUE;
}
BOOL Dispose_RPC() {
	IS_Disposed = TRUE;
	if (core != NULL) {
		core->ActivityManager().ClearActivity([](discord::Result result) {
		});
		core->~Core();
		core = NULL;
	}
	SYS_INFO_STR.~basic_string();
	return TRUE;
}