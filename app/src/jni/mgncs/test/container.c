
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>

#ifdef _MGNCSCTRL_CONTAINER

#define	PROMPTB		"happy new year!"
#define	CAPTION		"Demo for Container Control"
#define	PROMPTA		"Double-click me!"

#define	IDC_LAMIAN			101
#define	IDC_CHOUDOUFU		102
#define	IDC_JIANBING		103
#define	IDC_MAHUA			104
#define	IDC_SHUIJIAO		105
#define	IDC_XIAN			110
#define	IDC_LA				111
#define	IDC_PROMPT			200

static CTRLDATA CtrlYourTaste[] = {
	{
		"static",
		WS_VISIBLE | SS_GROUPBOX,
		16, 10, 230, 160,
		IDC_STATIC,
		"optional snack",
		0,
		WS_EX_TRANSPARENT
	},
	{
		"button",
		WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP,
		36, 38, 200, 20,
		IDC_LAMIAN,
		"northwest_pulled_noodle",
		0
	},
	{
		"button",
		WS_VISIBLE | BS_AUTORADIOBUTTON,
		36, 64, 200, 20,
		IDC_CHOUDOUFU,
		"chang_sha_bad_smelling_bean_curd",
		0
	},
	{
		"button",
		WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DISABLED,
		36, 90, 200, 20,
		IDC_JIANBING,
		"shan_dong_thini_pancake",
		0
	},
	{
		"button",
		WS_VISIBLE | BS_AUTORADIOBUTTON,
		36, 116, 200, 20,
		IDC_MAHUA,
		"tianjin_fired_dough_twist",
		0
	},
	{
		"button",
		WS_VISIBLE | BS_AUTORADIOBUTTON,
		36, 142, 200, 20,
		IDC_SHUIJIAO,
		"chengdu_red_oil_boiled_dumpling",
		0
	},
	{
		"static",
		WS_VISIBLE | SS_GROUPBOX | WS_GROUP,
		250, 10, 100, 160,
		IDC_STATIC,
		"flavor",
		0,
		WS_EX_TRANSPARENT
	},
	{
		"button",
		WS_VISIBLE | BS_AUTOCHECKBOX,
		260, 38, 80, 20,
		IDC_XIAN,
		"partial_salty",
		0
	},
	{
		"button",
		WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
		260, 64, 80, 20,
		IDC_LA,
		"partial_spicy",
		0
	},
	{
		"static",
		WS_VISIBLE | SS_LEFT | WS_GROUP | WS_BORDER,
		16, 180, 360, 40,
		IDC_PROMPT,
		"northwest_pulled_noodle_is_competitive_product_in_the_wheaten_food",
		0
	},
	{
		"button",
		WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP | WS_BORDER ,
		70, 230, 70, 30,
		IDOK,
		"OK",
		0
	},
	{
		"button",
		WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		200, 230, 70, 30,
		IDCANCEL,
		"Cancel",
		0
	},
};


static int MainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	mContainer* container;

	switch (message) {
		case MSG_CREATE:
			container = (mContainer*)ncsCreateWindow (NCSCTRL_CONTAINER,
						  "",
						  WS_BORDER | WS_VISIBLE,
						  WS_EX_NONE,
						  IDC_STATIC,
						  0, 0, 300, 200, hWnd,
						  NULL, NULL, NULL, 0);

			_c(container)->addIntrinsicControls (container, CtrlYourTaste,
					sizeof(CtrlYourTaste)/sizeof(CTRLDATA));
			return 0;

		case MSG_CSIZECHANGED: {
			HWND hContainer = GetDlgItem(hWnd, IDC_STATIC);
			if (hContainer != HWND_NULL || hContainer != HWND_INVALID)
				MoveWindow(hContainer, 0, 0, wParam, lParam, TRUE);
			break;
		}

		case MSG_DESTROY:
			DestroyAllControls (hWnd);
			return 0;

		case MSG_CLOSE:
			DestroyMainWindow (hWnd);
			PostQuitMessage (hWnd);
			return 0;
	}

	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain(int argc, const char* argv[])
{
	MSG Msg;
	HWND hMainWnd;
	MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
	JoinLayer(NAME_DEF_LAYER , "static" , 0 , 0);
#endif

	ncsInitialize();

	CreateInfo.dwStyle = WS_CAPTION | WS_BORDER | WS_VISIBLE;
	CreateInfo.dwExStyle = WS_EX_NONE;
	CreateInfo.spCaption = "happy";
	CreateInfo.hMenu = 0;
	CreateInfo.hCursor = GetSystemCursor(0);
	CreateInfo.hIcon = 0;
	CreateInfo.MainWindowProc = MainWinProc;
	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = 310;
	CreateInfo.by = 230;
	CreateInfo.iBkColor = GetWindowElementPixel(HWND_NULL, WE_MAINC_THREED_BODY);
	CreateInfo.dwAddData = 0;
	CreateInfo.hHosting = HWND_DESKTOP;

	hMainWnd = CreateMainWindow(&CreateInfo);

	if (hMainWnd == HWND_INVALID)
		return -1;

	ShowWindow(hMainWnd, SW_SHOWNORMAL);

	while (GetMessage(&Msg, hMainWnd)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	ncsUninitialize();

	return 0;
}

#else //_MGNCSCTRL_CONTAINER

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the container contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-container ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_CONTAINER

