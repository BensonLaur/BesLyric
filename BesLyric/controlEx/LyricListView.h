#pragma once

#include "stdafx.h"


//��չ�ؼ���ʹ�� SHeaderCtrl��ÿһ��item��� �ܹ����Ŵ��ڱ仯���ı�
class CLyricListView: public SMCListView
{
    SOUI_CLASS_NAME(LyricListView, L"lyriclistview")

protected:

	//�ڴ��ڴ�С�����ı�ʱ���ı�ÿһ����Ŀ��
	void OnSize(UINT nType, CSize size)
	{
		//SHDITEM item;
		//item.mask = 0 | SHDI_TEXT| SHDI_WIDTH| SHDI_LPARAM| SHDI_SORTFLAG| SHDI_ORDER;
		//item.cchTextMax = MAX_BUFFER_SIZE/2;
		//
		//int count = m_pHeader->GetItemCount();
		//vector<SHDITEM> items(count,item);			//�洢����Ϣ	
		//vector<WCHAR*> pszText(count, nullptr);		//�洢���ַ���ָ��
		//
		////��ȡ�����Ϣ
		//for(int i=0; i<count; i++)
		//{
		//	pszText[i] = new WCHAR[MAX_BUFFER_SIZE/2];  //�����ڴ�洢������
		//	items[i].pszText = pszText[i];
		//	m_pHeader->GetItem(i, &items[i]);			
		//}

		////�����޸ĺ�ĸ�����Ŀ��
		//items[0].cx = 30;
		//items[3].cx = 100;
		//items[4].cx = 200;

		//int nWidthLeft = (size.cx - 30 - items[0].cx - items[3].cx - items[4].cx);
		//items[1].cx = nWidthLeft * 2 / 3;
		//items[2].cx = nWidthLeft * 1 / 3;

		////ɾ��ԭ�����е���
		//m_pHeader->DeleteAllItems();
		//
		////����޸Ŀ�Ⱥ����
		//for(int i=0; i<count; i++)
		//	m_pHeader->InsertItem(i,items[i].pszText, items[i].cx, items[i].stFlag, items[i].lParam);
		//
		//for(int i=0; i<count; i++)
		//{
		//	delete pszText[i];  //�ͷ� ����Ĵ洢�����Ƶ��ڴ�
		//}

		SMCListView::OnSize(nType, size);
	}


	 SOUI_MSG_MAP_BEGIN()
            MSG_WM_SIZE(OnSize)
     SOUI_MSG_MAP_END()
};