#pragma once
#include "atlwin.h"
#include <Uxtheme.h>
#include <Shobjidl.h>

using namespace ATL;

class CLockKeysDeskBandWindow :
	public CWindowImpl<CLockKeysDeskBandWindow>
{
public:
	CLockKeysDeskBandWindow();
	~CLockKeysDeskBandWindow();

	BOOL Create(HWND ParentWnd, LPUNKNOWN DeskBand, LPUNKNOWN InputObjectSite);

	POINTL CalcWindowsSize() const;
	BOOL HasFocus() const;


BEGIN_MSG_MAP(CLockKeysDeskBandWindow)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
	MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_TIMECHANGE, OnTimeChange)
	MESSAGE_HANDLER(WM_POWERBROADCAST, OnPowerBroadcast)
END_MSG_MAP()

private:
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimeChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPowerBroadcast(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	BOOL m_HasFocus;
	
	LPUNKNOWN m_DeskBand;
	CComQIPtr<IInputObjectSite> m_InputObjectSite;
	UINT_PTR m_TimerId;
	HICON CapslkIcons[2];
	HICON NumlkIcons[2];
};

