#include "stdafx.h"
#include "PageSearchLyric.h"
#include <helper/SAdapterBase.h>
//#include "utility/FileHelper.h"
//#include "utility/WinFile.h"

//MC ListView �������࣬���ڴ��� MCListView ���߼�
class CLyricMcAdapterFix : public SMcAdapterBase
{
public:
    struct LYRIC_INFO
    {
        wstring strMusicName;
        wstring strMusicArtist;
        wstring strLyricFrom;
    };

    SArray<LYRIC_INFO> m_lyricInfo;

public:
    CLyricMcAdapterFix(){}

    virtual int getCount()
	{
		//m_lyricInfo.GetCount();
        return 2;
    }   
    
    virtual void getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
    {
        if(pItem->GetChildrenCount()==0)
        {
            pItem->InitFromXml(xmlTemplate);
        }
        //pItem->FindChildByName(L"txt_mclv_music_name")->SetWindowText(S_CW2T(psi->pszDesc));
        //pItem->FindChildByName(L"txt_index")->SetWindowText(SStringT().Format(_T("��%d��"),position+1));
        
        LYRIC_INFO *pli =m_lyricInfo.GetData()+position;

        pItem->FindChildByName(L"txt_mclv_lyric_num")->SetWindowText(L"1");
        pItem->FindChildByName(L"txt_mclv_music_name")->SetWindowText(L"���ǵĸ�");
        pItem->FindChildByName(L"txt_mclv_music_artist")->SetWindowText(L"������");
        pItem->FindChildByName(L"txt_mclv_origin")->SetWindowText(L"�����");
		
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
        
        if(SMessageBox(NULL,SStringT().Format(_T("click OnButtonOrignClick [%d] ?"),iItem),_T("tip"),MB_OK)==IDOK)
        {

        }
        return true;
    }
	
    bool OnButtonLrcClick(EventArgs *pEvt)
    {
        SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
        int iItem = pBtn->GetUserData();
		
        if(SMessageBox(NULL,SStringT().Format(_T("click OnButtonLrcClick [%d] ?"),iItem),_T("tip"),MB_OK)==IDOK)
        {

        }
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
	void AddItem(wstring strMusicName, wstring strMusicArtist, wstring strLyricFrom)
	{
		LYRIC_INFO lyricInfo;
		lyricInfo.strMusicName = strMusicName;
		lyricInfo.strMusicArtist = strMusicArtist;
		lyricInfo.strLyricFrom = strLyricFrom;
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


CPageSearchLyric::CPageSearchLyric()
{
	m_editSearchLyricName = NULL;
	m_editSearchLyricArtist = NULL;

	m_btnSearchHere = NULL;
	m_btnSearchBaidu = NULL;

	m_wndSearchLyricTip = NULL;
	m_txtSearchNameTip = NULL;
	m_txtSearchArtistTip = NULL;
	m_wndSearchArtistTip = NULL;
	m_txtSearchResultTip = NULL;

	m_editOriginLyricPath = NULL;
	m_editLrcLyricPath = NULL;
	
	m_btnSelectOriginLyricPath = NULL;
	m_btnSelectLrcLyricPath = NULL;
	
	m_editOriginLyricName = NULL;
	m_editOriginLyricArtist = NULL;
	m_editLrcLyricName = NULL;
	m_editLrcLyricArtist = NULL;
	
	m_editOriginLyricContent = NULL;
	m_editLrcLyricContent = NULL;
	
	m_pMainWnd = NULL;		/* ������ָ�� */
}

//��ʼ������ҳ��
void CPageSearchLyric::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//���������ڶ���
	m_pMainWnd = pMainWnd;

	//��ô����г��õĿؼ���ָ��
	m_editSearchLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_lyric_name);
	m_editSearchLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_lyric_artist);

	m_btnSearchHere = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_program);
	m_btnSearchBaidu = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_baidu);;

	m_wndSearchLyricTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_lyric_tip);;
	m_txtSearchNameTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_name_tip);;
	m_txtSearchArtistTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_artist_tip);;
	m_wndSearchArtistTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_lyric_artist_tip);;
	m_txtSearchResultTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_result_tip);;

	m_editOriginLyricPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_path);;
	m_editLrcLyricPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_path);;
	
	m_btnSelectOriginLyricPath = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_select_origin_lyric_path);;
	m_btnSelectLrcLyricPath = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_select_lrc_lyric_path);;
	
	m_editOriginLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_name);;
	m_editOriginLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_artist);;
	m_editLrcLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_name);;
	m_editLrcLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_artist);;
	
	m_editOriginLyricContent = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_content);;
	m_editLrcLyricContent = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_content);;
	
	SASSERT(m_editSearchLyricName != NULL);
	SASSERT(m_editSearchLyricArtist != NULL);
	SASSERT(m_btnSearchHere != NULL);
	SASSERT(m_btnSearchBaidu != NULL);
	SASSERT(m_wndSearchLyricTip != NULL);
	SASSERT(m_txtSearchNameTip != NULL);
	SASSERT(m_txtSearchArtistTip != NULL);
	SASSERT(m_wndSearchArtistTip != NULL);
	SASSERT(m_txtSearchResultTip != NULL);
	SASSERT(m_editOriginLyricPath != NULL);
	SASSERT(m_editLrcLyricPath != NULL);
	SASSERT(m_btnSelectOriginLyricPath != NULL);
	SASSERT(m_btnSelectLrcLyricPath != NULL);
	SASSERT(m_editOriginLyricName != NULL);
	SASSERT(m_editOriginLyricArtist != NULL);
	SASSERT(m_editLrcLyricName != NULL);
	SASSERT(m_editLrcLyricArtist != NULL);
	SASSERT(m_editOriginLyricContent != NULL);
	SASSERT(m_editLrcLyricContent != NULL);


	//����listview
    SMCListView * pMcListView = m_pMainWnd->FindChildByName2<SMCListView>("mclv_lyric");
    if(pMcListView)
    {
        IMcAdapter *pAdapter = new CLyricMcAdapterFix;
        pMcListView->SetAdapter(pAdapter);
        pAdapter->Release();
    }
}

//��������ڶ���
CMainDlg* CPageSearchLyric::M()
{
	return (CMainDlg*)m_pMainWnd;
}


/*
*	����Ԥ��ҳ�����Ӧ����
*/

void CPageSearchLyric::OnBtnSearchBaidu()
{
	SStringW strMusicName = m_editSearchLyricName->GetWindowTextW();
	SStringW strMusicArtist = m_editSearchLyricArtist->GetWindowTextW();
	
	strMusicName.Trim(' ');//��ȥ��ǰ��ո�
	strMusicName.Trim('\t');
	strMusicArtist.Trim(' ');
	strMusicArtist.Trim('\t');

	if(strMusicName.GetLength() == 0)
	{
		_MessageBox(m_pMainWnd->m_hWnd, L"����������Ϊ��", L"��ʾ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	//�������е��ַ�����ת������ʹ�������ܹ�֧��
	/*
	"\"����б��  %5C 
	"|"      %7C 
	�س�  %0D%0A 
	�ո�  %20 
	˫���� %22 
	"&"		%26 
	*/
	wstring sMusicName = L"";
	wstring sMusicArtist = L"";
	for(auto i = 0;  i < strMusicName.GetLength(); i++)
	{
		switch(strMusicName.GetAt(i))
		{
		case L'\\':
			sMusicName += L"%5C";
			break;
		case L'|':
			sMusicName += L"%7C";
			break;
		case L'\n':
			sMusicName += L"%0D%0A";
			break;
		case L' ':
			sMusicName += L"%20";
			break;
		case L'"':
			sMusicName += L"%22";
			break;
		case L'&':
			sMusicName += L"%26";
			break;
		default:
			sMusicName += strMusicName.GetAt(i);
		}
	}

	if(strMusicArtist.GetLength() != 0)
	for(auto i = 0;  i < strMusicArtist.GetLength(); i++)
	{
		switch(strMusicArtist.GetAt(i))
		{
		case L'\\':
			sMusicArtist += L"%5C";
			break;
		case L'|':
			sMusicArtist += L"%7C";
			break;
		case L'\n':
			sMusicArtist += L"%0D%0A";
			break;
		case L' ':
			sMusicArtist += L"%20";
			break;
		case L'"':
			sMusicArtist += L"%22";
			break;
		case L'&':
			sMusicArtist += L"%26";
			break;
		default:
			sMusicArtist += strMusicArtist.GetAt(i);
		}
	}

	wstring sParam = L"start https://www.baidu.com/s?wd=%22";
	sParam += sMusicName.c_str();

	if(strMusicArtist.GetLength() != 0)
	{
		sParam += L"%22%20%22";
		sParam += sMusicArtist.c_str();
	}

	sParam += L"%22%20%22���%22";

	//ʹ�ÿ���̨��Ĭ��������ٶ��������
	_wsystem(sParam.c_str());
	
}



