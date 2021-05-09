#include "framework.h"
#include "CGG-Lab_3.h"

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
int frame_step = 0;
float dist = 0.f;
float angle = 0.f;

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

Image* image;
UINT frameIndex;
UINT frameCount;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void                Display(HDC);
void                DrawWheels(HDC, Graphics*);
void Tween(const PointF*, const PointF*, PointF*, int, float);

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
    LoadStringW(hInstance, IDC_CGGLAB3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CGGLAB3));

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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CGGLAB3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CGGLAB3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        image = Image::FromFile(L"bckgrnd.gif");
        if (image == NULL) return -1;

        frameIndex = 0;
        frameCount = image->GetFrameCount(&FrameDimensionTime);

        SetTimer(hWnd, 1, 40, NULL);
        return 0;
    }
    case WM_TIMER:
    {
        frameIndex = (frameIndex + 1) % frameCount;
        image->SelectActiveFrame(&FrameDimensionTime, frameIndex);
        frame_step++;
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

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
    case WM_ERASEBKGND: return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Display(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        if (image != NULL)
        {
            delete image;
        }
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

void DrawBike(Graphics& g)
{
    Rect rect;
    g.GetVisibleClipBounds(&rect);
    Bitmap backbuffer(rect.Width, rect.Height, &g);

    Graphics temp(&backbuffer);
    temp.SetSmoothingMode(SmoothingModeHighQuality);

    Pen compPen(Color::Black, 10.f);
    float comp[6] = {
        0.0f, 0.2f,
        0.3f, 0.7f,
        0.8f, 1.0f };
    compPen.SetCompoundArray(comp, 6);
    g.DrawEllipse(&compPen, 400, 400, 120, 120);    //заднее колесо    
    g.DrawEllipse(&compPen, 620, 400, 120, 120);    //переднее колесо

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
    PointF framePoints[4] = {
        PointF(460.f, 460.f),
        PointF(550.f, 460.f),
        PointF(650.f, 360.f),
        PointF(490.f, 360.f)
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

void Display(HDC hdc)
{
    Graphics g(hdc);

    Rect rect;
    g.GetVisibleClipBounds(&rect);

    Bitmap backBuffer(rect.Width, rect.Height, &g);

    Graphics temp(&backBuffer);
    temp.DrawImage(image, 0, 0, rect.Width, rect.Height);

    angle = -90.f * frame_step / frameCount;

    if (frame_step < 50)
    {
        dist = 5.f * (frame_step - 50);
        temp.TranslateTransform(0.f + dist, 0.f);
        DrawBike(temp);
    }
    else if (frame_step < 150)
    {
        temp.RotateTransform(angle + 10);
        DrawBike(temp);
    }
    else if (frame_step == 150)
    {
        frame_step = -1;
        temp.ResetTransform();
    }
    g.DrawImage(&backBuffer, rect);
}