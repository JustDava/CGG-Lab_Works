#include<windows.h>
#include<Windowsx.h>
#include<gdiplus.h>
#include<gdiplustypes.h>
#include <string>
#include <tchar.h>
#include<Strsafe.h>
#include<vector>
#include <cmath>
#define PI 3.14159265
#pragma comment(lib,"Gdiplus.lib")
using namespace Gdiplus;
using namespace std;
VOID OnPaint(HDC hdc);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("Коэн-Сазерланд ");
	RegisterClass(&wndClass);
	hWnd = CreateWindow(
		TEXT("Коэн-Сазерланд "), TEXT("Коэн-Сазерланд "), WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 700, 700,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_INITDIALOG:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
template <class Func>
inline std::vector<Gdiplus::GpPointF> Curve(float tMin, float tMax, size_t n, Func f) {
	std::vector<Gdiplus::GpPointF> result;
	if (n > 1)
	{
		result.reserve(n); // зарезервируем место для n точек
		// определяем шаг изменения t
		float dt = (tMax - tMin) / (float)(n - 1);
		for (size_t i = 0; i < n; ++i)
		{
			// добавим точку в массив
			result.push_back(f(tMin + i * dt));
		}
	}
	return result;
}
struct WorldWindow
{
	REAL m00, m11, m02, m12;
	template<class T>
	WorldWindow(T l, T r, T b, T t)
		: m00(static_cast<REAL>(2) / (r - l))
		, m11(static_cast<REAL>(2) / (t - b))
		, m02(static_cast<REAL>(-(r + l)) / (r - l))
		, m12(static_cast<REAL>(-(t + b)) / (t - b))
	{}
	GpPointF map(GpPointF p) const
	{
		return { p.X * m00 + m02, p.Y * m11 + m12 };
	}
};
struct Viewport
{
	GpRectF r;
	template<class T>
	Viewport(T l, T t, T r, T b)
		: r(static_cast<REAL>(l),
			static_cast<REAL>(t),
			static_cast<REAL>(r - l)* static_cast<REAL>(0.5),
			static_cast<REAL>(b - t)* static_cast<REAL>(0.5))
	{}
	GpPointF map(GpPointF p) const
	{
		return
		{
		r.X + (p.X + static_cast<REAL>(1)) * r.Width,
		r.Y + (static_cast<REAL>(1) - p.Y) * r.Height
		};
	}
};
using vec2f = GpPointF;
struct clipper
{
	enum region : uint32_t
	{
		INSIDE = 0,
		LEFT = 1,
		RIGHT = 1 << 1,
		BOTTOM = 1 << 2,
		TOP = 1 << 3
	};
	static constexpr float border = 1;
	static uint32_t clip_point(const vec2f& point)
	{
		uint32_t result = INSIDE;
		if (point.X < -border)
			result |= LEFT;
		else if (border < point.X)
			result |= RIGHT;
		if (point.Y < -border)
			result |= BOTTOM;
		else if (border < point.Y)
			result |= TOP;
		return result;
	}
	static bool clip_line(vec2f& first, vec2f& second)
	{
		uint32_t first_pos = clip_point(first);
		uint32_t second_pos = clip_point(second);
		while (true)
		{
			if ((first_pos | second_pos) == INSIDE)
				return true;
			else if ((first_pos & second_pos))
				return false;
			else
			{
				const vec2f delta = second - first;
				uint32_t* pos = &first_pos;
				vec2f* point = &first;
				if (*pos == INSIDE)
				{
					pos = &second_pos;
					point = &second;
				}
				if (*pos & TOP)
				{
					point->X = first.X + delta.X * (border - first.Y) / delta.Y;
					point->Y = border;
				}
				else if (*pos & BOTTOM)
				{
					point->X = first.X + delta.X * (-border - first.Y) / delta.Y;
					point->Y = -border;
				}
				else if (*pos & RIGHT)
				{
					point->Y = first.Y + delta.Y * (border - first.X) / delta.X;
					point->X = border;
				}
				else // if (*pos & LEFT)
				{
					point->Y = first.Y + delta.Y * (-border - first.X) / delta.X;
					point->X = -border;
				}
				*pos = clip_point(*point);
			}
		}
	}
};
void DrawPolyline(Gdiplus::Graphics& g, const Gdiplus::Pen* pen, const std::vector<Gdiplus::GpPointF>& polyline,
	const WorldWindow& w, const Viewport& vp)
{
	for (size_t i = 1; i < polyline.size(); ++i)
	{
		GpPointF points[2] = { w.map(polyline[i - 1]), w.map(polyline[i]) };
		if (clipper::clip_line(points[0], points[1]))
		{
			points[0] = vp.map(points[0]);
			points[1] = vp.map(points[1]);
			// рисуем ребро
			g.DrawLines(pen, points, 2);
		} // if
	} // for
} // DrawPolyline
Gdiplus::GpPointF f(REAL t)
{
	GpPointF res;
	res.X = 24.f * std::cos(t) - 5.f * std::cos(24.f * t / 5.f);
	res.Y = 24.f * std::sin(t) - 5.f * std::sin(24.f * t / 5.f);
	return res;
}
VOID OnPaint(HDC hdc)
{
	Graphics drawingBoard(hdc);
	drawingBoard.Clear(Color::White);
	Pen curvePen(Color(0, 0, 0), 2.0f);
	Pen borderPen(Color::Black);
	WorldWindow w(-36, 10, 10, -36); // кординаты мирового окна
	Viewport vp(100, 100, 600, 600); // координаты порта просмотра
	drawingBoard.DrawRectangle(&borderPen, 100, 100, 500, 500);
	std::vector<Gdiplus::GpPointF> parabola = Curve(0, 10 * PI, 1000, f);
	DrawPolyline(drawingBoard, &curvePen, parabola, w, vp);
}
