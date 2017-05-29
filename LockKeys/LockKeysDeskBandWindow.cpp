#include "stdafx.h"
#include "LockKeysDeskBandWindow.h"
 
#define UPDATE_TIME_ID 1
extern HINSTANCE g_Hinstance;

HHOOK KeyBoardHook = NULL;
HWND  DeskBandWindowWnd = NULL;

// KeyBoard Status
BOOL g_CapslkStatus = FALSE;
BOOL g_NumlkStatus = FALSE;

#define RECTWIDTH(rc)   ((rc).right - (rc).left)
#define RECTHEIGHT(rc)  ((rc).bottom - (rc).top)

LRESULT _stdcall HookKeyboard(int code, WPARAM wParam, LPARAM lParam);

CLockKeysDeskBandWindow::CLockKeysDeskBandWindow()
{
	m_TimerId = 0;
	m_DeskBand = NULL;
	m_HasFocus = FALSE;
}


CLockKeysDeskBandWindow::~CLockKeysDeskBandWindow()
{
	// 窗口销毁 记得卸载钩子
	if (KeyBoardHook != NULL)
	{
		UnhookWindowsHookEx(KeyBoardHook);
		KeyBoardHook = NULL;
	}
		
}

BOOL CLockKeysDeskBandWindow::Create(HWND ParentWnd, LPUNKNOWN DeskBand, LPUNKNOWN InputObjectSite)
{
	if (!__super::Create(ParentWnd))
		return FALSE;

	ATLASSERT(DeskBand);
	m_DeskBand = DeskBand;

	ATLASSERT(InputObjectSite);
	m_InputObjectSite = InputObjectSite;

	// 初始化 ICONS
	CapslkIcons[0] = LoadIcon(g_Hinstance, MAKEINTRESOURCE(IDI_CAPSLKON));
	CapslkIcons[1] = LoadIcon(g_Hinstance, MAKEINTRESOURCE(IDI_CAPSLKOFF));

	NumlkIcons[0] = LoadIcon(g_Hinstance, MAKEINTRESOURCE(IDI_NUMLKON));
	NumlkIcons[1] = LoadIcon(g_Hinstance, MAKEINTRESOURCE(IDI_NUMLKOFF));
	

	// 保存窗口句柄
	DeskBandWindowWnd = m_hWnd;
	
	// 初始化键位状态
	if (GetKeyState(VK_CAPITAL) & 0x1)
		g_CapslkStatus = TRUE;

	if (GetKeyState(VK_NUMLOCK) & 01)
		g_NumlkStatus = TRUE;

	// 设置timer 
	m_TimerId = SetTimer(UPDATE_TIME_ID, 500);

	// 挂钩全局键盘消息
	//KeyBoardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)HookKeyboard, g_Hinstance, 0);

	return TRUE;
}

POINTL CLockKeysDeskBandWindow::CalcWindowsSize() const
{
	// Not a window -> return minimal size
	POINTL Pointl = { 0 };
	if (!IsWindow())
	{
		Pointl.x = GetSystemMetrics(SM_CXMIN);
		Pointl.y = GetSystemMetrics(SM_CYMIN);
	}
	else
	{
		// we will show at least two icons ( 16 * 16)
		// 6 16 4 16 6
		Pointl.x = 48;
		Pointl.y = 16;
	}

	return Pointl;
}

BOOL CLockKeysDeskBandWindow::HasFocus() const
{
	return m_HasFocus;
}

LRESULT CLockKeysDeskBandWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	// 任务栏绘制窗口 - 几个关键点 -> Theme 
	PAINTSTRUCT PaintStruct = { 0 };
	HDC hdc = ::BeginPaint(m_hWnd, &PaintStruct);

	int X = PaintStruct.rcPaint.left;
	int Y = PaintStruct.rcPaint.top;
	int Width = RECTWIDTH(PaintStruct.rcPaint);
	int Height = RECTHEIGHT(PaintStruct.rcPaint);

	HDC MemoryDC = ::CreateCompatibleDC(hdc);
	HGDIOBJ MemoryBitmap = ::CreateCompatibleBitmap(hdc, Width, Height);
	HGDIOBJ MemorySave = ::SelectObject(MemoryDC, MemoryBitmap);
	::SetWindowOrgEx(MemoryDC, X, Y, NULL);

	DrawThemeParentBackground(m_hWnd, MemoryDC, &PaintStruct.rcPaint);

	RECT ClientRect = { 0 };
	GetClientRect(&ClientRect);

	// DebugBreak()
	if (g_CapslkStatus)
		::DrawIconEx(MemoryDC, 6, (RECTHEIGHT(ClientRect) - 16) / 2, CapslkIcons[0], 16, 16, 0, NULL, DI_COMPAT | DI_NORMAL);
	else
		::DrawIconEx(MemoryDC, 6, (RECTHEIGHT(ClientRect) - 16) / 2, CapslkIcons[1], 16, 16, 0, NULL, DI_COMPAT | DI_NORMAL);

	if (g_NumlkStatus)
		::DrawIconEx(MemoryDC, 26, (RECTHEIGHT(ClientRect) - 16) / 2, NumlkIcons[0], 16, 16, 0, NULL, DI_COMPAT | DI_NORMAL);
	else
		::DrawIconEx(MemoryDC, 26, (RECTHEIGHT(ClientRect) - 16) / 2, NumlkIcons[1], 16, 16, 0, NULL, DI_COMPAT | DI_NORMAL);

	::BitBlt(hdc, X, Y, Width, Height, MemoryDC, X, Y, SRCCOPY);

	::SelectObject(MemoryDC, MemorySave);
	::DeleteObject(MemoryBitmap);
	::DeleteDC(MemoryDC);

	::EndPaint(m_hWnd, &PaintStruct);


	return LRESULT(0);
}

LRESULT CLockKeysDeskBandWindow::OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	m_HasFocus = (uMsg == WM_SETFOCUS);

	if (m_InputObjectSite)
		m_InputObjectSite->OnFocusChangeIS(m_DeskBand, m_HasFocus);

	return LRESULT(0);
}

LRESULT CLockKeysDeskBandWindow::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return LRESULT(0);
}

LRESULT CLockKeysDeskBandWindow::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	BOOL IsFresh = FALSE;
	if (GetKeyState(VK_CAPITAL) & 0x1 
		&& g_CapslkStatus == FALSE)
	{
		g_CapslkStatus = TRUE;
		IsFresh = TRUE;
	}
	else if ((GetKeyState(VK_CAPITAL) & 0x1) == 0 
			&& g_CapslkStatus == TRUE)
	{
		g_CapslkStatus = FALSE;
		IsFresh = TRUE;
	}
	
	if (GetKeyState(VK_NUMLOCK) & 01 
		&& g_NumlkStatus == FALSE)
	{
		g_NumlkStatus = TRUE;
		IsFresh = TRUE;
	}
	else if ((GetKeyState(VK_NUMLOCK) & 0x1) == 0 
			&& g_NumlkStatus == TRUE)
	{
		g_NumlkStatus = FALSE;
		IsFresh = TRUE;
	}

	CWindow RefreshWin;
	RefreshWin.Attach(m_hWnd);
	RefreshWin.Invalidate();

	return LRESULT(0);
}

LRESULT CLockKeysDeskBandWindow::OnTimeChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return OnTimer(uMsg, wParam, lParam, bHandled);
}

LRESULT CLockKeysDeskBandWindow::OnPowerBroadcast(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if(wParam == PBT_APMRESUMECRITICAL ||
		wParam == PBT_APMRESUMESUSPEND ||
		wParam == PBT_APMRESUMESTANDBY)
	{
		// re-enable timer
		UINT_PTR TimerId = SetTimer(UPDATE_TIME_ID, 500);
	}

	return 0;
}

LRESULT _stdcall HookKeyboard(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION)
	{
		ULONG TransitionState = (ULONG)lParam >> 31;
		if (wParam == VK_CAPITAL)
		{
			if (TransitionState == 1)
			{
				// DebugBreak()
				g_CapslkStatus = !g_CapslkStatus;
				// 强制窗口完全刷新
				if (DeskBandWindowWnd != NULL)
				{
					CWindow RefreshWin;
					RefreshWin.Attach(DeskBandWindowWnd);
					RefreshWin.Invalidate();
				}
			}
		}
		else if (wParam == VK_NUMLOCK)
		{
			if (TransitionState == 1)
			{
				g_NumlkStatus = !g_NumlkStatus;
				// 强制窗口完全刷新
				if (DeskBandWindowWnd != NULL)
				{
					CWindow RefreshWin;
					RefreshWin.Attach(DeskBandWindowWnd);
					RefreshWin.Invalidate();
				}
			}
		}
	}

	return CallNextHookEx(KeyBoardHook, code, wParam, lParam);
}