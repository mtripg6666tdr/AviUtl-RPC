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
BOOL Update_PluginMetaInfo(FILTER* fp);
BOOL Initialize_RPC();
BOOL Dispose_RPC();
BOOL Update_RPC(FILTER*,void*, int, bool, bool);
void __stdcall func_timer_tick(HWND, UINT, UINT_PTR, DWORD);

// �`�F�b�N�{�b�N�X�̃X�e�[�^�X���I�t
#define FILTER_CHECKBOX_STATUS_OFF 0
// �`�F�b�N�{�b�N�X�̃X�e�[�^�X���I��
#define FILTER_CHECKBOX_STATUS_ON 1

// �X�e�[�^�X��\���t���O

// �X�e�[�^�X���u�ҏW���v
#define RPC_STATUS_EDITING 1
// �X�e�[�^�X���u�A�C�h�����v
#define RPC_STATUS_IDLING 2
// �X�e�[�^�X���u�G���R�[�h���v
#define RPC_STATUS_SAVING 3

// �^�C�}�[�Ԋu
#define RPC_UPDATE_TICK 1000

#define WM_FILTER_CHANGE_PARAM_POST_EVENT (WM_USER+10011)
#endif // !AVIUTL_DISCORD_RPC_MAIN
