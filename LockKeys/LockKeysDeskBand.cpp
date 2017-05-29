// LockKeysDeskBand.cpp : CLockKeysDeskBand 的实现

#include "stdafx.h"
#include "LockKeysDeskBand.h"


// CLockKeysDeskBand
CLockKeysDeskBand::CLockKeysDeskBand()
{


}

HRESULT CLockKeysDeskBand::FinalConstruct()
{
	m_BandID = 0;
	m_ViewMode = DBIF_VIEWMODE_NORMAL;
	m_CompositionEnabled = TRUE;

	// Show
	OLECHAR szAtom[39] = { 0 };
	StringFromGUID2(CLSID_LockKeysDeskBand, szAtom, 39);

	HRESULT hr = S_OK;
	const ATOM AtomShow = GlobalFindAtom(szAtom);

	if (AtomShow != 0)
	{
		CComPtr<IBandSite> BandSite;
		hr = BandSite.CoCreateInstance(CLSID_TrayBandSiteService);

		if (SUCCEEDED(hr))
		{
			LPUNKNOWN Unknown = static_cast<IOleWindow*>(this);
			hr = BandSite->AddBand(Unknown);
		}

		GlobalDeleteAtom(AtomShow);
	}

	return hr;
}

// IObjectWithSite
STDMETHODIMP CLockKeysDeskBand::SetSite(IUnknown * pUnkSite)
{
	HRESULT hr = __super::SetSite(pUnkSite);

	if (SUCCEEDED(hr) && pUnkSite) // pUnkSite is NULL when band is being destroyed
	{
		CComQIPtr<IOleWindow> spOleWindow = pUnkSite;

		if (spOleWindow)
		{
			HWND hwndParent = NULL;
			hr = spOleWindow->GetWindow(&hwndParent);

			if (SUCCEEDED(hr))
			{
				// 窗口DeskBand窗口创建

				m_DeskBandWnd.Create(hwndParent,
					static_cast<IDeskBand*>(this), pUnkSite);

				if (!m_DeskBandWnd.IsWindow())
					hr = E_FAIL;
			}
		}
	}

	return hr;
}


// IInputObject
STDMETHODIMP CLockKeysDeskBand::UIActivateIO(BOOL fActivate, MSG * pMsg)
{
	ATLTRACE(atlTraceCOM, 2, _T("IInputObject::UIActivateIO (%s)\n"),
		(fActivate ? _T("TRUE") : _T("FALSE")));

	if (fActivate)
		m_DeskBandWnd.SetFocus();

	return S_OK;
}

STDMETHODIMP CLockKeysDeskBand::HasFocusIO(void)
{
	ATLTRACE(atlTraceCOM, 2, _T("IInputObject::HasFocusIO\n"));

	return (m_DeskBandWnd.HasFocus() ? S_OK : S_FALSE);
}

STDMETHODIMP CLockKeysDeskBand::TranslateAcceleratorIO(MSG * pMsg)
{
	ATLTRACE(atlTraceCOM, 2, _T("IInputObject::TranslateAcceleratorIO\n"));

	return S_FALSE;
}

// IDeskBand
STDMETHODIMP CLockKeysDeskBand::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO * pdbi)
{
	ATLTRACE(atlTraceCOM, 2, _T("IDeskBand::GetBandInfo\n"));

	if (!pdbi) 
		return E_INVALIDARG;

	m_BandID = dwBandID;
	m_ViewMode = dwViewMode;

	if (pdbi->dwMask & DBIM_MODEFLAGS)
	{
		pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT;
	}

	if (pdbi->dwMask & DBIM_MINSIZE)
	{
		pdbi->ptMinSize = m_DeskBandWnd.CalcWindowsSize();
	}

	if (pdbi->dwMask & DBIM_MAXSIZE)
	{
		// the band object has no limit for its maximum height
		pdbi->ptMaxSize.x = -1;
		pdbi->ptMaxSize.y = -1;
	}

	if (pdbi->dwMask & DBIM_INTEGRAL)
	{
		pdbi->ptIntegral.x = 1;
		pdbi->ptIntegral.y = 1;
	}

	if (pdbi->dwMask & DBIM_ACTUAL)
	{
		pdbi->ptActual = m_DeskBandWnd.CalcWindowsSize();
	}

	if (pdbi->dwMask & DBIM_TITLE)
	{
		if (dwViewMode == DBIF_VIEWMODE_FLOATING)
		{
			lstrcpynW(pdbi->wszTitle, L"LockKeys", ARRAYSIZE(pdbi->wszTitle));
		}
		else
			pdbi->dwMask &= ~DBIM_TITLE; // do not show title
	}

	if (pdbi->dwMask & DBIM_BKCOLOR)
	{
		//Use the default background color by removing this flag.
		pdbi->dwMask &= ~DBIM_BKCOLOR;
	}

	return S_OK;
}

// IOleWindow
STDMETHODIMP CLockKeysDeskBand::GetWindow(HWND * phwnd)
{
	ATLTRACE(atlTraceCOM, 2, _T("IOleWindow::GetWindow\n"));

	if (phwnd)
		*phwnd = m_DeskBandWnd;

	return S_OK;
}

STDMETHODIMP CLockKeysDeskBand::ContextSensitiveHelp(BOOL fEnterMode)
{
	ATLTRACE(atlTraceCOM, 2, _T("IOleWindow::ContextSensitiveHelp\n"));

	ATLTRACENOTIMPL("IOleWindow::ContextSensitiveHelp");
}

// IDockingWindow
STDMETHODIMP CLockKeysDeskBand::ShowDW(BOOL fShow)
{
	ATLTRACE(atlTraceCOM, 2, _T("IDockingWindow::ShowDW\n"));
	if (m_DeskBandWnd)
		m_DeskBandWnd.ShowWindow(fShow ? SW_SHOW : SW_HIDE);

	return S_OK;
}

STDMETHODIMP CLockKeysDeskBand::CloseDW(DWORD dwReserved)
{
	ATLTRACE(atlTraceCOM, 2, _T("IDockingWindow::CloseDW\n"));

	if (m_DeskBandWnd)
	{
		m_DeskBandWnd.ShowWindow(SW_HIDE);
		m_DeskBandWnd.DestroyWindow();
	}

	return S_OK;
}

STDMETHODIMP CLockKeysDeskBand::ResizeBorderDW(LPCRECT prcBorder, IUnknown * punkToolbarSite, BOOL fReserved)
{
	ATLTRACE(atlTraceCOM, 2, _T("IDockingWindow::ResizeBorderDW\n"));

	if (!m_DeskBandWnd)
		return S_OK;

	CComQIPtr<IDockingWindowSite> spDockingWindowSite = punkToolbarSite;

	if (spDockingWindowSite)
	{
		BORDERWIDTHS bw = { 0 };
		bw.top = bw.bottom = ::GetSystemMetrics(SM_CYBORDER);
		bw.left = bw.right = ::GetSystemMetrics(SM_CXBORDER);

		HRESULT hr = spDockingWindowSite->RequestBorderSpaceDW(
			static_cast<IDeskBand*>(this), &bw);

		if (SUCCEEDED(hr))
		{
			HRESULT hr = spDockingWindowSite->SetBorderSpaceDW(
				static_cast<IDeskBand*>(this), &bw);

			if (SUCCEEDED(hr))
			{
				m_DeskBandWnd.MoveWindow(prcBorder);
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

// IDeskBand2
STDMETHODIMP CLockKeysDeskBand::CanRenderComposited(BOOL * pfCanRenderComposited)
{
	if (pfCanRenderComposited)
		*pfCanRenderComposited = TRUE;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CLockKeysDeskBand::SetCompositionState(
	/* [in] */ BOOL fCompositionEnabled)
{
	m_CompositionEnabled = fCompositionEnabled;
	m_DeskBandWnd.Invalidate();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CLockKeysDeskBand::GetCompositionState(
	/* [out] */ BOOL *pfCompositionEnabled)
{
	if (pfCompositionEnabled)
		*pfCompositionEnabled = m_CompositionEnabled;

	return S_OK;
}

HRESULT CLockKeysDeskBand::IPersistStreamInit_Load(
	LPSTREAM pStm,
	const ATL_PROPMAP_ENTRY* pMap)
{
	const HRESULT hr = __super::IPersistStreamInit_Load(pStm, pMap);

	return hr;
}

HRESULT CLockKeysDeskBand::IPersistStreamInit_Save(
	LPSTREAM pStm,
	BOOL fClearDirty,
	const ATL_PROPMAP_ENTRY* pMap)
{
	return __super::IPersistStreamInit_Save(pStm, fClearDirty, pMap);
}