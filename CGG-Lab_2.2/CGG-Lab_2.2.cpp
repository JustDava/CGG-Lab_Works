// CGG-Lab_2.2.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "CGG-Lab_2.2.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Resource.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")



#define MAX_LOADSTRING 100

#define LEFT    1
#define RIGHT   2
#define BOT     4
#define TOP     8

class WorldWindow
{
public:
    float Left;
    float Top;
    float Right;
    float Bottom;

    inline WorldWindow(float left, float top, float right, float bottom) : Left(left), Top(top), Right(right), Bottom(bottom) {}

    //inline float Width() const
    //{
    //    return (Right - Left);
    //}
    //inline float Height() const
    //{
    //    return (Top - Bottom);
    //}
};

class ViewPort : public Gdiplus::Rect
{
public:
    inline ViewPort(int left, int top, int right, int bottom) : Gdiplus::Rect(left, top, right-left, bottom-top) {}

    //inline ViewPort(const RECT &rect) : Gdiplus::Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top) {}

    //inline operator RECT() const
    //{
    //    RECT rect = { X, Y, X + Width, Y + Width };
    //    return rect;
    //}
};

inline void WorldToViewport(const WorldWindow& w, const ViewPort& vp, Gdiplus::PointF* points, unsigned int count)
{
    float A = vp.Width / (w.Right - w.Left);
    float B = vp.Height / (w.Bottom - w.Top);
    float C = vp.X - A * w.Left;
    float D = vp.Y - B * w.Top;

    for (unsigned int i = 0; i < count; i++)
    {
        points[i].X = A * points[i].X + C;
        points[i].Y = B * points[i].Y + D;
    }
}

int vcode(RectF* r, PointF* p)
{
    int code = 0;
    if (p->X < r->GetLeft())
    {
        code += LEFT;
    }
    if (p->X > r->GetRight())
    {
        code += RIGHT;
    }
    if (p->Y > r->GetBottom())
    {
        code += BOT;
    }
    if (p->Y < r->GetTop())
    {
        code += TOP;
    }
    return code;
}

void Display(HDC hdc, PointF* a, PointF* b)
{
    Graphics g(hdc);
        
    Pen pen(Color::Black);

    g.DrawLine(&pen, *a, *b);
}

int cohen_sutherland(RectF* r, PointF* points, unsigned int count, HDC hdc)
{
    int code_a, code_b, code; /* код концов отрезка */
    PointF* c; /* одна из точек */
    int i, j = 0;
    for (i = 0, j = count - 1; i < count; j = i++)
    {       
        code_a = vcode(r, &points[i]);
        code_b = vcode(r, &points[j]);

        while (code_a | code_b) {

            //if (code_a & code_b)
            //{
            //    break;
            //}

            if (code_a) {
                code = code_a;
                c = &points[i];
            }
            else {
                code = code_b;
                c = &points[j];
            }

            if (code & LEFT) {
                c->Y += (&points[i].Y - &points[j].Y) * (float)(r->GetLeft() - c->X) / (&points[i].X - &points[j].X);
                c->X = r->GetLeft();
            }
            else if (code & RIGHT) {
                c->Y += (&points[i].Y - &points[j].Y) * (float)(r->GetRight() - c->X) / (&points[i].X - &points[j].X);
                c->X = r->GetRight();
            }
            else if (code & BOT) {
                c->X += (&points[i].X - &points[j].X) * (float)(r->GetBottom() - c->Y) / (&points[i].Y - &points[j].Y);
                c->Y = r->GetBottom();
            }
            else if (code & TOP) {
                c->X += (&points[i].X - &points[j].X) * (float)(r->GetTop() - c->Y) / (&points[i].Y - &points[j].Y);
                c->Y = r->GetTop();
            }

            if (code == code_a) {
                points[i].X = c->X;
                points[i].Y = c->Y;
                code_a = vcode(r, &points[i]);
            }
            else {
                points[j].X = c->X;
                points[j].Y = c->Y;
                code_b = vcode(r, &points[j]);
            }


        }
        Display(hdc, &points[i], &points[j]);
        
    }   
    return 0;
}

//int cohen_sutherland(RectF* r, PointF* a, PointF* b, HDC hdc)
//{
//    int code_a, code_b, code; /* код концов отрезка */
//    PointF* c; /* одна из точек */
//
//    code_a = vcode(r, a);
//    code_b = vcode(r, b);
//
//    /* пока одна из точек отрезка вне прямоугольника */
//    while (code_a | code_b) {
//        /* если обе точки с одной стороны прямоугольника, то отрезок не пересекает прямоугольник */
//        if (code_a & code_b)
//            return -1;
//
//        /* выбираем точку c с ненулевым кодом */
//        if (code_a) {
//            code = code_a;
//            c = a;
//        }
//        else {
//            code = code_b;
//            c = b;
//        }
//
//        /* если c левее r, то передвигаем c на прямую x = r->x_min
//           если c правее r, то передвигаем c на прямую x = r->x_max */
//        if (code & LEFT) {
//            c->Y += (a->Y - b->Y) * (r->GetLeft() - c->X) / (a->X - b->X);
//            c->X = r->GetLeft();
//        }
//        else if (code & RIGHT) {
//            c->Y += (a->Y - b->Y) * (r->GetRight() - c->X) / (a->X - b->X);
//            c->X = r->GetRight();
//        }/* если c ниже r, то передвигаем c на прямую y = r->y_min
//            если c выше r, то передвигаем c на прямую y = r->y_max */
//        else if (code & BOT) {
//            c->X += (a->X - b->X) * (r->GetBottom() - c->Y) / (a->Y - b->Y);
//            c->Y = r->GetBottom();
//        }
//        else if (code & TOP) {
//            c->X += (a->X - b->X) * (r->GetTop() - c->Y) / (a->Y - b->Y);
//            c->Y = r->GetTop();
//        }
//
//        /* обновляем код */
//        if (code == code_a) {
//            a = c;
//            code_a = vcode(r, a);
//        }
//        else {
//            b = c;
//            code_b = vcode(r, b);
//        }
//    }
//
//    Display(hdc, a, b);
//
//    /* оба кода равны 0, следовательно обе точки в прямоугольнике */
//    return 0;
//}

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_CGGLAB22, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CGGLAB22));

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
    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CGGLAB22));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CGGLAB22);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

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
            Graphics g(hdc);
            g.Clear(Color::White);
            
            Pen pen(Color::Black);

            RectF r(100.f, 100.f, 500.f, 300.f);
            g.DrawRectangle(&pen, r);

            //PointF a(50.f, 50.f);
            //PointF b(550.f, 550.f);

            //int result = cohen_sutherland(&r, &a, &b, hdc);

            WorldWindow wWnd(-50, -50, 50, 50);

            ViewPort vPort(150, 50, 300, 150);
            vPort.Width = 600;
            vPort.Height = 600;

            const int n = 64;

            PointF points[n];

            float tMax = 10 * M_PI;
            float tMin = 0;
            float dt = (tMax - tMin) / (float)(n - 1);
            
            for (int i = 0; i < n; i++)
            {                
                points[i].X = 24 * cos(dt * i) - 5 * (float)cos(24 / 5 * dt * i);
                points[i].Y = 24 * sin(dt * i) - 5 * (float)sin(24 / 5 * dt * i);
            }

            WorldToViewport(wWnd, vPort, points, n);

            int result = cohen_sutherland(&r, points, n, hdc);

            //g.DrawCurve(&pen, points, n);

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
