/*
	BesLyric  һ�� �����򵥡�����ʵ�õ� ר�������������������ֹ�����ʵ� ������������
    Copyright (C) 2017  
	Author: BensonLaur <BensonLaur@163.com>
	Author:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file       FileDroper.h
* 
* @Describe   CFileDroper�࣬ʵ���ļ��Ϸţ� CFileDroper ʵ���Ϸź󣬷�����Ϣ��·�����ݸ�Ŀ�괰�ڣ�CTestDropTarget Ϊ CFileDroper ���ࣻ 
*/

#pragma once
#include "stdafx.h"
#include <shellapi.h>

class CTestDropTarget:public IDropTarget
{
public:
    CTestDropTarget()
    {
        nRef=0;
    }
    
    virtual ~CTestDropTarget(){}
    
    //////////////////////////////////////////////////////////////////////////
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
    {
        HRESULT hr=S_FALSE;
        if(riid==__uuidof(IUnknown))
            *ppvObject=(IUnknown*) this,hr=S_OK;
        else if(riid==__uuidof(IDropTarget))
            *ppvObject=(IDropTarget*)this,hr=S_OK;
        if(SUCCEEDED(hr)) AddRef();
        return hr;

    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void){return ++nRef;}

    virtual ULONG STDMETHODCALLTYPE Release( void) { 
        ULONG uRet= -- nRef;
        if(uRet==0) delete this;
        return uRet;
    }

    //////////////////////////////////////////////////////////////////////////
    // IDropTarget

    virtual HRESULT STDMETHODCALLTYPE DragEnter( 
        /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect)
    {
        *pdwEffect=DROPEFFECT_LINK;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect)
    {
        *pdwEffect=DROPEFFECT_LINK;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DragLeave( void)
    {
        return S_OK;
    }


protected:
    int nRef;
};

class CFileDroper : public CTestDropTarget
{
protected:
    SWindow *m_pEdit;
	HWND m_hwndRecieveMsg;

public:
	/* hwnd Ϊ������ϢĿ�괰�ھ���� pEditΪ RegisterDragDrop ע������ �ؼ���ָ��*/
    CFileDroper(HWND hwnd, SWindow *pEdit):m_pEdit(pEdit)
    {
		m_hwndRecieveMsg = hwnd;	//��ô��ھ�� 

        if(m_pEdit) m_pEdit->AddRef();
    }
    ~CFileDroper()
    {
        if(m_pEdit) m_pEdit->Release();
    }
public:
    virtual HRESULT STDMETHODCALLTYPE Drop( 
        /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect)
    {
        FORMATETC format =
        {
            CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL
        };
        STGMEDIUM medium;
        if(FAILED(pDataObj->GetData(&format, &medium)))
        {
            return S_FALSE;
        }

        HDROP hdrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));

        if(!hdrop)
        {
            return S_FALSE;
        }

        bool success = false;
        TCHAR filename[MAX_PATH];
        success=!!DragQueryFile(hdrop, 0, filename, MAX_PATH);
        DragFinish(hdrop);
        GlobalUnlock(medium.hGlobal);

        if(success && m_pEdit)
        {
			//���ؼ�ָ�� �� �ļ����ַ���ָ�� ͨ����Ϣ���͸�������
			::SendMessage(m_hwndRecieveMsg, MSG_USER_DROP_FILE,(WPARAM) m_pEdit, (LPARAM)filename);
        }

        *pdwEffect=DROPEFFECT_LINK;
        return S_OK;
    }
};
