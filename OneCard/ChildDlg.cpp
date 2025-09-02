// ChildDlg.cpp: 구현 파일
//

#include "pch.h"
#include "OneCard.h"
#include "afxdialogex.h"
#include "ChildDlg.h"
#include <compressapi.h>
#pragma comment(lib, "Cabinet.lib")


// ChildDlg 대화 상자

IMPLEMENT_DYNAMIC(ChildDlg, CDialogEx)

ChildDlg::ChildDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHILD, pParent)
{
    do {
        // 1. 리소스 핸들 찾기
        HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN1), RT_RCDATA);
        if (!hResInfo) {
            break;
        }

        // 2. 리소스 로드
        HGLOBAL hRes = LoadResource(NULL, hResInfo);
        if (!hRes) {
            break;
        }

        // 3. 리소스 데이터에 대한 포인터 및 크기 얻기
        LPVOID pData = LockResource(hRes);
        DWORD dwSize = SizeofResource(NULL, hResInfo);
        if (!pData || dwSize == 0) {
            break;
        }

        // 4. 암호화된 데이터를 벡터로 복사
		bmpData.resize(dwSize);
		memcpy(bmpData.data(), pData, dwSize);

        // 5. 복호화
        for (BYTE& byte : bmpData) {
            byte ^= 0x77;
        }

    } while (0);
}

ChildDlg::~ChildDlg()
{
}

void ChildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ChildDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// ChildDlg 메시지 처리기

BOOL ChildDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, bmpData.size());
    if (hGlobal)
    {
        void* pBuffer = ::GlobalLock(hGlobal);
        if (pBuffer)
        {
            memcpy(pBuffer, bmpData.data(), bmpData.size());
            ::GlobalUnlock(hGlobal);

            IStream* pStream = NULL;
            if (SUCCEEDED(::CreateStreamOnHGlobal(hGlobal, TRUE, &pStream)))
            {
                // 3. IStream으로부터 CImage를 로드합니다.
                if (FAILED(m_image.Load(pStream)))
                {
                    AfxMessageBox(_T("메모리 스트림으로부터 이미지를 로드하는데 실패했습니다."));
                }
                pStream->Release();
            }
        }
        else
        {
            ::GlobalFree(hGlobal);
        }
    }

    if (!m_image.IsNull())
    {
        const double scaleFactor = 2.0;
        int scaledWidth = static_cast<int>(m_image.GetWidth() * scaleFactor);
        int scaledHeight = static_cast<int>(m_image.GetHeight() * scaleFactor);

        const int PADDING = 20;

        m_destRect.SetRect(PADDING, PADDING, PADDING + scaledWidth, PADDING + scaledHeight);

        // --- 이 부분을 올바르게 수정합니다 ---

        // 1. 현재 창의 전체 크기와 클라이언트 영역 크기를 가져옵니다.
        CRect windowRect, clientRect;
        GetWindowRect(&windowRect);
        GetClientRect(&clientRect);

        // 2. 테두리의 너비와 높이를 계산합니다.
        int frameWidth = windowRect.Width() - clientRect.Width();
        int frameHeight = windowRect.Height() - clientRect.Height();
        // --- 여기까지 ---

        // 새 창의 너비와 높이 설정
        int newWidth = scaledWidth + (PADDING * 2) + frameWidth;
        int newHeight = scaledHeight + (PADDING * 2) + frameHeight;

        SetWindowPos(NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
    }

	return TRUE;
}

void ChildDlg::OnPaint()
{
    CPaintDC dc(this);
    if (!m_image.IsNull())
    {
        m_image.Draw(dc.GetSafeHdc(), m_destRect);
    }
}

INT_PTR ChildDlg::DoModal()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

    return CDialogEx::DoModal();
}
