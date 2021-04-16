// CGG-Lab_2.3.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "CGG-Lab_2.3.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Resource.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100


// Глобальные переменные:
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
RECT clientRect;
HWND mhWnd;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID                Display(HDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CGGLAB23, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CGGLAB23));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_CGGLAB23));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CGGLAB23);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        100, 100, 1280, 720, nullptr, nullptr, hInstance, nullptr);


    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    mhWnd = hWnd;

    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Display(hdc);
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

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

VOID Display(HDC hdc)
{
    Graphics g(hdc);
    g.Clear(Color::White);
    g.SetSmoothingMode(SmoothingModeHighQuality);

    // Колеса

    Region rgn;

    Region rgn1(Rect(350, 350, 110, 180));
    Pen compPen1(Color::Black, 10.f);
    float comp[6] = {
        0.0f, 0.2f,
        0.3f, 0.7f,
        0.8f, 1.0f };
    compPen1.SetCompoundArray(comp, 6);

    g.SetClip(&rgn1);
    g.DrawEllipse(&compPen1, 400, 400, 120, 120);    //заднее колесо    
    g.SetClip(&rgn);
    
    Region rgn2(Rect(460, 350, 110, 180));
    Pen compPen2(Color::DarkCyan, 10.f);
    compPen2.SetCompoundArray(comp, 6);

    g.SetClip(&rgn2);
    g.DrawEllipse(&compPen2, 400, 400, 120, 120);
    g.SetClip(&rgn);

    Region rgn3(Rect(600, 350, 80, 180));
    g.SetClip(&rgn3);
    g.DrawEllipse(&compPen2, 620, 400, 120, 120);
    g.SetClip(&rgn);

    Region rgn4(Rect(680, 350, 80, 180));
    g.SetClip(&rgn4);
    g.DrawEllipse(&compPen1, 620, 400, 120, 120);    //переднее колесо
    g.SetClip(&rgn);

    HatchBrush hatchBrush(HatchStyleOutlinedDiamond, Color::Gray, Color::White);

    g.FillEllipse(&hatchBrush, 400, 400, 120, 120);    //заднее колесо    

    g.FillEllipse(&hatchBrush, 620, 400, 120, 120);    //переднее колесо

    //g.FillEllipse(&hatchBrush, 447, 447, 26, 26);  //задняя ось
    //g.FillEllipse(&hatchBrush, 671, 451, 18, 18);  //передняя ось
    Pen redPen(Color::Red, 8.f);
    redPen.SetStartCap(LineCapRound);
    redPen.SetLineJoin(LineJoinRound);
    redPen.SetEndCap(LineCapRound);

    // Рама
    Point framePoints[4] = {
        Point(460, 460),
        Point(550, 460),
        Point(650, 360),
        Point(490, 360)
    };

    g.DrawPolygon(&redPen, framePoints, 4);

    // Руль
    Point wheelPoints[3] = {
        Point(680, 460),
        Point(637, 320),
        Point(610, 320)
    };
    g.DrawLines(&redPen, wheelPoints, 3);

    // Сиденье
    Point Wpt1(550, 460);
    Point Wpt2(475, 340);
    g.DrawLine(&redPen, Wpt1, Wpt2);

    Pen blackPen(Color::Black, 8.f);
    blackPen.SetStartCap(LineCapRound);
    blackPen.SetEndCap(LineCapRound);
    Point Qpt1(460, 340);
    Point Qpt2(490, 340);
    g.DrawLine(&blackPen, Qpt1, Qpt2);
}