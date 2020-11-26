//---------------------------------------------------------------------
//      AviUtlでDiscord RPCを表示するプラグイン
//---------------------------------------------------------------------

// インクルード
#include <Windows.h>
#include <string>
#include <chrono>
#include "aviutl-sdk/filter.h"
#include "main.h"
#include "discord-files/discord.h"

// バッファー
static void* buf0 = NULL;
// RPCプラグインが有効かどうか
static BOOL RPC_Enabled = TRUE;
// セーブ中(エンコード中)かどうか
static BOOL IS_SAVING = FALSE;
// 前回のRPC更新時にセーブ中(エンコード中)かどうか
static BOOL HAS_SAVING = FALSE;
// 現在編集中のファイル名
static const char* FILTER_CURRENT_FILENAME = NULL;
// フィル名がNULLだったかどうか
static BOOL FILENAME_WAS_NULL = FALSE;
// 前回のRPC更新時に編集中かどうか
static BOOL HAS_EDITING = FALSE;
// タイマーの識別子
static UINT_PTR timer_identifer = NULL;
// RPCが破棄されているかどうか
static BOOL IS_Disposed = TRUE;
// システム情報
static LPSTR SYS_INFO_STR = NULL;
// Detail Temp
std::string detail;

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
TCHAR   FILTER_INFO[]          = "AviUtl Discord RPC version 0.99b by mtripg6666tdr";

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
//		処理関数
//---------------------------------------------------------------------
BOOL func_proc(FILTER* fp, FILTER_PROC_INFO* fpip) {
	return TRUE;
}

//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
#pragma optimize("", off)
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
	SYS_INFO info;
	if (fp->exfunc->get_sys_info(NULL, &info)) {
		LPSTR ver = info.info;
		LPSTR app_name = "AviUtl ";
		LPSTR plugin_str = " (RPC powered by AviUtl RPC Plugin by mtripg6666tdr)";
		int num = strlen(ver) + strlen(app_name) + strlen(plugin_str) + 1;
		SYS_INFO_STR = new char[num];
		strcpy_s(SYS_INFO_STR, num, app_name);
		strcat_s(SYS_INFO_STR, num, ver);
		strcat_s(SYS_INFO_STR, num, plugin_str);
	}
	return TRUE;
}
#pragma optimize("", on)


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
//		プロジェクト保存
//---------------------------------------------------------------------
BOOL func_project_save(FILTER* fp, void* editP, void* data, int* size) {
	Update_RPC_Flags(fp, editP);
	return FALSE;
}

//---------------------------------------------------------------------
//		ウインドウタイトル変更
//---------------------------------------------------------------------
BOOL func_modify_title(FILTER* fp, void* editP, int frame, LPSTR title, int max_title) {
	//Display_RPC(fp, editP);
	Update_RPC_Flags(fp, editP);
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
	if (core != NULL) {
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
		Update_RPC_Flags(filterPtr, editPtr);
		break;
	case WM_FILTER_FILE_CLOSE:
		FILTER_CURRENT_FILENAME = NULL;
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
		activity.GetAssets().SetLargeImage("aviutl_icon_large");
		activity.GetAssets().SetLargeText(SYS_INFO_STR == NULL ? "AviUtl" : SYS_INFO_STR);
		activity.SetType(discord::ActivityType::Playing);
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			
		});
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
		// Ref: https://stackoverflow.com/questions/6012663/get-unix-timestamp-with-c
		if (!HAS_SAVING && IS_SAVING) {
			State = u8"エンコード中";
			activity.GetAssets().SetSmallImage("status_encoding");
			activity.GetAssets().SetSmallText(u8"エンコード");
			activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			HAS_SAVING = TRUE;
			if (FILTER_CURRENT_FILENAME != NULL) {
				activity.SetDetails(FILTER_CURRENT_FILENAME);
			}
		}else if (FILTER_CURRENT_FILENAME == NULL) {
			State = u8"アイドル中";
			activity.GetAssets().SetSmallImage("status_idle");
			activity.GetAssets().SetSmallText(u8"アイドル状態");
			if (!FILENAME_WAS_NULL) {
				activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			}
			FILENAME_WAS_NULL = TRUE;
			HAS_EDITING = FALSE;
			HAS_SAVING = FALSE;
			if (FILTER_CURRENT_FILENAME != NULL) {
				activity.SetDetails(FILTER_CURRENT_FILENAME);
			}
		} else if(!HAS_EDITING) {
			State = u8"編集中";
			activity.GetAssets().SetSmallImage("status_editing");
			activity.GetAssets().SetSmallText(u8"編集中");
			activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			HAS_EDITING = TRUE;
			HAS_SAVING = TRUE;
			FILENAME_WAS_NULL = FALSE;
		}
		activity.SetState(State.c_str());
		if (FILTER_CURRENT_FILENAME != NULL && strlen(FILTER_CURRENT_FILENAME) != 1) {
			activity.GetAssets().SetSmallText(FILTER_CURRENT_FILENAME);
		}
		if (FILTER_CURRENT_FILENAME != NULL) {
			activity.SetDetails(FILTER_CURRENT_FILENAME);
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
BOOL Update_RPC_Flags(FILTER* filterPtr, void* editPtr) {
	SYS_INFO si;
	FILE_INFO fi;
	if (filterPtr->exfunc->get_file_info(editPtr, &fi)) {
		detail = fi.name;
		if (filterPtr->exfunc->get_sys_info(editPtr, &si)) {
			std::string fullpath = si.project_name;
			// Ref: https://qiita.com/takano_tak/items/acf34b4a30cb974bab65
			int path_i = fullpath.find_last_of("\\") + 1;//7
			int ext_i = fullpath.find_last_of(".");//10
			std::string filename = fullpath.substr(path_i, ext_i - path_i) == "" ? "" : fullpath.substr(path_i, ext_i - path_i) + fullpath.substr(ext_i, fullpath.size() - ext_i);
			detail += "(";
			detail += filename == "" ? u8"未保存" : filename;
			detail += ")";
		}
		FILTER_CURRENT_FILENAME = detail.c_str();
	}
	else {
		FILTER_CURRENT_FILENAME = NULL;
	}
	return TRUE;
}