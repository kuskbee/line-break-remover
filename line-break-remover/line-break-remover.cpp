// line-break-remover.cpp
//

#include "framework.h"
#include "line-break-remover.h"
#include <string>

#define MAX_LOADSTRING 100

// resource
#define ID_EDIT 100

// ui
#define WINDOW_WIDTH    350
#define WINDOW_HEIGHT   280
#define EDIT_X          10
#define EDIT_Y          10
#define EDIT_WIDTH      300
#define EDIT_HEIGHT     200

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LINEBREAKREMOVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LINEBREAKREMOVER));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LINEBREAKREMOVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
       DWORD dwError = ::GetLastError();
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit = nullptr;
    switch (message)
    {
    case WM_CREATE:
    {
        // Create an edit box
        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            EDIT_X, EDIT_Y, EDIT_WIDTH, EDIT_HEIGHT, hWnd, (HMENU)ID_EDIT, GetModuleHandle(NULL), NULL);
        if (hEdit == nullptr)
        {
            MessageBox(hWnd, _T("failed to create editbox."), _T("Error"), MB_OK | MB_ICONERROR);
            return -1;
        }
        break;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_EDIT && HIWORD(wParam) == EN_CHANGE)
        {
            int len = GetWindowTextLength(hEdit);
            TCHAR* str = (TCHAR*)malloc((len + 1) * sizeof(TCHAR));
            if (nullptr == str) {
                return -1;
            }

            std::wstring strFinal;
            GetWindowText(hEdit, str, len + 1);
            TCHAR* p = str;

            
            while (*p)
            {
                if (*p != '\r' && *p != '\n') // ignore line-feed, carriage return
                {
                    strFinal += *p;
                }
                else if (*p == '\n')
                {
                    int lastChatIndex = strFinal.length() - 1;
                    if (lastChatIndex > 0) {
                        wchar_t cLast = strFinal[lastChatIndex];
                        if (cLast == '-') 
                        {
                            // connect word (ex. tas-\r\nty -> tasty)
                            strFinal.pop_back();
                        }
                        else if(!GetKeyState(VK_RETURN)) // prevent to add blank when push enter-key
                        {
                            // add blank (ex. hello\r\nworld -> hello world)
                            strFinal += ' ';
                        }
                    }
                }
                ++p;
            }
            SetWindowText(hEdit, strFinal.c_str());
            SetFocus(hEdit);
            SendMessage(hEdit, EM_SETSEL, strFinal.length(), strFinal.length());
            free(str);
        }

        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (GetKeyState(VK_CONTROL))
        {
            SendMessage(hEdit, EM_SETSEL, 0, -1);
        }
        break;
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}