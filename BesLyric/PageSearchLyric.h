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
* @file       PageSearchLyric.h
* 
* Describe    CPageSearchLyric�࣬����ԭ��ʣ�lrc ���
*/

#pragma once
#include "stdafx.h"
#include "MainDlg.h"
#include <helper/SAdapterBase.h>


//MC ListView �������࣬���ڴ��� MCListView ���߼�
class CLyricMcAdapterFix : public SMcAdapterBase
{
private:
    struct LYRIC_INFO
    {
        wstring strMusicName;		//����
        wstring strMusicArtist;		//����
        wstring strLyricFrom;		//�����Դ
		wstring strPlaneText;		//����ʱ��ĸ��
		wstring strLabelText;		//��ʱ���ǩ���ı�
    };

    SArray<LYRIC_INFO> m_lyricInfo;

	SHostWnd*	m_mainWnd;

public:
    CLyricMcAdapterFix(){}

	void AttachMainDlgPointer(SHostWnd* swindow){m_mainWnd = swindow;}

    virtual int getCount()
	{
        return m_lyricInfo.GetCount();
    }   
    
    virtual void getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
    {
        if(pItem->GetChildrenCount()==0)
        {
            pItem->InitFromXml(xmlTemplate);
        }
        LYRIC_INFO *pli =m_lyricInfo.GetData()+position;

		pItem->FindChildByName(L"txt_mclv_lyric_num")->SetWindowText(SStringW().Format(L"%02d",position + 1));
		pItem->FindChildByName(L"txt_mclv_music_name")->SetWindowText(SStringW().Format(L"%s",pli->strMusicName.c_str()));
		pItem->FindChildByName(L"txt_mclv_music_artist")->SetWindowText(SStringW().Format(L"%s",pli->strMusicArtist.c_str()));
		pItem->FindChildByName(L"txt_mclv_origin")->SetWindowText(SStringW().Format(L"%s",pli->strLyricFrom.c_str()));
		
        SButton *pBtnViewOrigin = pItem->FindChildByName2<SButton>(L"btn_mclv_view_origin");
        pBtnViewOrigin->SetUserData(position);
        pBtnViewOrigin->GetEventSet()->subscribeEvent(EVT_CMD,Subscriber(&CLyricMcAdapterFix::OnButtonOrignClick,this));
        SButton *pBtnViewLrc = pItem->FindChildByName2<SButton>(L"btn_mclv_view_lrc");
        pBtnViewLrc->SetUserData(position);
        pBtnViewLrc->GetEventSet()->subscribeEvent(EVT_CMD,Subscriber(&CLyricMcAdapterFix::OnButtonLrcClick,this));
    }

    bool OnButtonOrignClick(EventArgs *pEvt)
    {
        SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
        int iItem = pBtn->GetUserData();
        LYRIC_INFO *pli =m_lyricInfo.GetData()+iItem;
        
		m_mainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_name)->SetWindowTextW( pli->strMusicName.c_str());
		m_mainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_artist)->SetWindowTextW( pli->strMusicArtist.c_str());
		m_mainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_content)->SetWindowTextW( pli->strPlaneText.c_str());

		//�л���ԭ���ҳ��
		m_mainWnd->FindChildByID2<STabCtrl>(R.id.tab_lyric_list)->SetCurSel(1);
        return true;
    }
	
    bool OnButtonLrcClick(EventArgs *pEvt)
    {
        SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
        int iItem = pBtn->GetUserData();
		LYRIC_INFO *pli =m_lyricInfo.GetData()+iItem;
        
		m_mainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_name)->SetWindowTextW( pli->strMusicName.c_str());
		m_mainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_artist)->SetWindowTextW( pli->strMusicArtist.c_str());
		m_mainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_content)->SetWindowTextW( pli->strLabelText.c_str());
        
		//�л���Lrc���ҳ��
		m_mainWnd->FindChildByID2<STabCtrl>(R.id.tab_lyric_list)->SetCurSel(2);
        return true;
    }
	
    //ɾ��һ�У��ṩ�ⲿ���á�
    void DeleteItem(int iPosition)
    {
        if(iPosition>=0 && iPosition<getCount())
        {
            m_lyricInfo.RemoveAt(iPosition);
            notifyDataSetChanged();
        }
    }
	   
	//ɾ�������У��ṩ�ⲿ���á�
    void DeleteAllItem()
    {
		m_lyricInfo.RemoveAll();
        notifyDataSetChanged();
    }
	
    //���һ�У��ṩ�ⲿ���á�
	void AddItem(wstring strMusicName, wstring strMusicArtist, wstring strLyricFrom, wstring strPlaneText, wstring strLabelText)
	{
		LYRIC_INFO lyricInfo;
		lyricInfo.strMusicName = strMusicName;
		lyricInfo.strMusicArtist = strMusicArtist;
		lyricInfo.strLyricFrom = strLyricFrom;
		lyricInfo.strPlaneText = strPlaneText;
		lyricInfo.strLabelText = strLabelText;
		m_lyricInfo.Add(lyricInfo);
	}

    virtual SStringW GetColumnName(int iCol) const{
        return SStringW().Format(L"col%d",iCol+1);
    }
    
    struct SORTCTX
    {
        int iCol;
        SHDSORTFLAG stFlag;
    };
    
    bool OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols)
    {
        if(iCol==0 || iCol==4) //��1����ź����һ�� ����������֧������
            return false;
        
        SHDSORTFLAG stFlag = stFlags[iCol];
        switch(stFlag)
        {
            case ST_NULL:stFlag = ST_UP;break;
            case ST_DOWN:stFlag = ST_UP;break;
            case ST_UP:stFlag = ST_DOWN;break;
        }
        for(int i=0;i<nCols;i++)
        {
            stFlags[i]=ST_NULL;
        }
        stFlags[iCol]=stFlag;
        
        SORTCTX ctx={iCol,stFlag};
        qsort_s(m_lyricInfo.GetData(),m_lyricInfo.GetCount(),sizeof(LYRIC_INFO),SortCmp,&ctx);
        return true;
    }
    
    static int __cdecl SortCmp(void *context,const void * p1,const void * p2)
    {
        SORTCTX *pctx = (SORTCTX*)context;
        const LYRIC_INFO *pLI1=(const LYRIC_INFO*)p1;
        const LYRIC_INFO *pLI2=(const LYRIC_INFO*)p2;
        int nRet =0;
        switch(pctx->iCol)
        {
            case 0://���
                break;
            case 1://strMusicName
				nRet = wcscmp(pLI1->strMusicName.c_str(), pLI2->strMusicName.c_str());
                break;
            case 2://strMusicArtist
				nRet = wcscmp(pLI1->strMusicArtist.c_str(), pLI2->strMusicArtist.c_str());
                break;
            case 3://strLyricFrom
				nRet = wcscmp(pLI1->strLyricFrom.c_str(), pLI2->strLyricFrom.c_str());
                break;

            case 4://����
                break;

        }
        if(pctx->stFlag == ST_UP)
            nRet = -nRet;
        return nRet;
    }
};


class CMainDlg;							//Ƕ�׶��壬������
class CPageSearchLyric
{
	friend class CMainDlg;	

public:
	CPageSearchLyric();
	~CPageSearchLyric(){};

	void Init(SHostWnd *pMainWnd);		//��ʼ������ҳ��
	CMainDlg* M();					//�����������ʵ������
	
	//�ڰٶ��������
	void OnBtnSearchBaidu();

	//ͨ��������ڻ�ȡ���
	void OnBtnSearchInProgram();
	void ShowLyricResult(LyricSearchResult* pResult);
	
	void OnBtnSelectOriginLyricPath(LPCWSTR pFilePath = NULL);	//ѡ��ԭ��ʱ���·��
	void OnBtnSelectLrcLyricPath(LPCWSTR pFilePath = NULL);		//ѡ��Lrc��ʱ���·��

	void OnBtnSaveOriginLyricPath();	//����ԭ���
	void OnBtnSaveLrcLyricPath();		//����Lrc���

private:
	//��ý�������д�ĸ������͸�����
	bool GetMusicAndArtist(SStringW &strMusicName, SStringW &strMusicArtist);


	//��Ϣ
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		
		EVENT_ID_COMMAND(R.id.btn_search_lyric_using_baidu , OnBtnSearchBaidu)
		EVENT_ID_COMMAND(R.id.btn_search_lyric_using_program , OnBtnSearchInProgram)
		
		EVENT_ID_COMMAND(R.id.btn_select_origin_lyric_path , OnBtnSelectOriginLyricPath);
		EVENT_ID_COMMAND(R.id.btn_select_lrc_lyric_path , OnBtnSelectLrcLyricPath);
		EVENT_ID_COMMAND(R.id.btn_save_origin_lyric_path , OnBtnSaveOriginLyricPath);
		EVENT_ID_COMMAND(R.id.btn_save_lrc_lyric_path , OnBtnSaveLrcLyricPath);

	EVENT_MAP_BREAK()

	
private:

	//�ؼ�ָ��
	SEdit	*m_editSearchLyricName;
	SEdit	*m_editSearchLyricArtist;

	SButton *m_btnSearchHere;
	SButton *m_btnSearchBaidu;

	SWindow *m_wndSearchLyricTip;
	SStatic *m_txtSearchNameTip;
	SStatic *m_txtSearchArtistTip;
	SWindow *m_wndSearchArtistTip;
	SStatic *m_txtSearchResultTip;
	SWindow *m_txtIsSearchingLyricTip;

	SEdit	*m_editOriginLyricPath;
	SEdit	*m_editLrcLyricPath;
	
	SButton *m_btnSelectOriginLyricPath;
	SButton *m_btnSelectLrcLyricPath;
	
	SEdit	*m_editOriginLyricName;
	SEdit	*m_editOriginLyricArtist;
	SEdit	*m_editLrcLyricName;
	SEdit	*m_editLrcLyricArtist;
	
	SEdit	*m_editOriginLyricContent;
	SEdit	*m_editLrcLyricContent;

	CLyricMcAdapterFix*	m_lrcListAdapter;

	
	SHostWnd *m_pMainWnd;		/* ������ָ�� */
};