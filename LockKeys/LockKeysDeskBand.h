// LockKeysDeskBand.h : CLockKeysDeskBand 的声明

#pragma once
#include "resource.h"       // 主符号
#include "LockKeysGuids.h"
#include <Shobjidl.h>
#include <ShlObj.h>
#include "LockKeysDeskBandWindow.h"
#include "LockKeys_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;

// CLockKeysDeskBand

class ATL_NO_VTABLE CLockKeysDeskBand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLockKeysDeskBand, &CLSID_LockKeysDeskBand>,
	public IDispatchImpl<ILockKeysDeskBand, &IID_ILockKeysDeskBand, &LIBID_LockKeysLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectWithSiteImpl<CLockKeysDeskBand>,		// 对象基本属性
	public IPersistStreamInitImpl<CLockKeysDeskBand>,	// 确保任务栏选择正确
	public IDeskBand2,	 // DeskBand主继承类
	public IInputObject  // 输入对象
{
	typedef IPersistStreamInitImpl<CLockKeysDeskBand> IPersistStreamImpl;
public:
	CLockKeysDeskBand();

DECLARE_REGISTRY_RESOURCEID(IDR_LOCKKEYSDESKBAND)

BEGIN_COM_MAP(CLockKeysDeskBand)
	COM_INTERFACE_ENTRY(ILockKeysDeskBand)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IOleWindow)
	COM_INTERFACE_ENTRY(IDockingWindow)
	COM_INTERFACE_ENTRY(IDeskBand)
	COM_INTERFACE_ENTRY(IDeskBand2)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IInputObject)
	COM_INTERFACE_ENTRY_IID(IID_IPersist, IPersistStreamImpl)
	COM_INTERFACE_ENTRY_IID(IID_IPersistStream, IPersistStreamImpl)
	COM_INTERFACE_ENTRY_IID(IID_IPersistStreamInit, IPersistStreamImpl)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CLockKeysDeskBand)
	IMPLEMENTED_CATEGORY(CATID_DeskBand)
END_CATEGORY_MAP()

BEGIN_PROP_MAP(CLockKeysDeskBand)
	
END_PROP_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease()
	{
	}

public:
	// IObjectWithSite
	//
	STDMETHOD(SetSite)(
		/* [in] */ IUnknown *pUnkSite);

	// IDeskBand
	//
	STDMETHOD(GetBandInfo)(
		/* [in] */ DWORD dwBandID,
		/* [in] */ DWORD dwViewMode,
		/* [out][in] */ __RPC__inout DESKBANDINFO *pdbi);

	// IDeskBand2
	//
	STDMETHOD(CanRenderComposited)(
		/* [out] */ BOOL *pfCanRenderComposited);

	STDMETHOD(SetCompositionState)(
		/* [in] */ BOOL fCompositionEnabled);

	STDMETHOD(GetCompositionState)(
		/* [out] */ BOOL *pfCompositionEnabled);

	// IOleWindow
	//
	STDMETHOD(GetWindow)(
		/* [out] */ HWND *phwnd);

	STDMETHOD(ContextSensitiveHelp)(
		/* [in] */ BOOL fEnterMode);

	// IDockingWindow
	//
	STDMETHOD(ShowDW)(
		/* [in] */ BOOL fShow);

	STDMETHOD(CloseDW)(
		/* [in] */ DWORD dwReserved);

	STDMETHOD(ResizeBorderDW)(
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IUnknown *punkToolbarSite,
		/* [in] */ BOOL fReserved);

	// IInputObject
	//
	STDMETHOD(UIActivateIO)(
		/* [in] */ BOOL fActivate,
		/* [unique][in] */ MSG *pMsg);

	STDMETHOD(HasFocusIO)();

	STDMETHOD(TranslateAcceleratorIO)(
		/* [in] */ MSG *pMsg);

	// IPersistStreamImpl
	STDMETHOD(IPersistStreamInit_Load)(
		/* [in] */ LPSTREAM pStm,
		/* [in] */ const ATL_PROPMAP_ENTRY* pMap);

	STDMETHOD(IPersistStreamInit_Save)(
		/* [in] */ LPSTREAM pStm,
		/* [in] */ BOOL fClearDirty,
		/* [in] */ const ATL_PROPMAP_ENTRY* pMap);

	BOOL   m_bRequiresSave;
private:
	// Parameters 
	int    m_BandID;
	DWORD  m_ViewMode;
	BOOL   m_CompositionEnabled;
	CLockKeysDeskBandWindow m_DeskBandWnd;
	
};

OBJECT_ENTRY_AUTO(CLSID_LockKeysDeskBand, CLockKeysDeskBand)
