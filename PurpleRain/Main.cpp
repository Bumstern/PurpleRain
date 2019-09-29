#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define _WIN32_WINNT 0x0500
#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
using namespace sf;

void SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash)
{
	// get the size of the bitmap
	BITMAP bm;
	GetObject(hbmpSplash, sizeof(bm), &bm);
	SIZE sizeSplash = { bm.bmWidth, bm.bmHeight };

	// get the primary monitor info
	POINT ptZero = { 0 };
	HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorinfo = { 0 };
	monitorinfo.cbSize = sizeof(monitorinfo);
	GetMonitorInfo(hmonPrimary, &monitorinfo);

	// center the splash screen in the middle of the primary work area
	const RECT& rcWork = monitorinfo.rcWork;
	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - sizeSplash.cy) / 2;

	// create a memory DC holding the splash bitmap
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpSplash);

	// use the source image alpha channel for blending
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	// paint the window (in the right location) with the alpha-blended bitmap
	UpdateLayeredWindow(hwndSplash, hdcScreen, &ptOrigin, &sizeSplash,
		hdcMem, &ptZero, RGB(0, 0, 0), &blend, ULW_ALPHA);

	// delete temporary objects
	SelectObject(hdcMem, hbmpOld);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);
}

struct Particle {
	int l;		    // Длина хвоста
	int w;		    // Толщина капли
	Vector2f coor;  // Координата головы
	float v;		// Скорость капли
	int alpha;      // Прозрачность
};

int main() {
	const int Xwin = 1024, Ywin = 620, Ndrop = 250;	   // Xwin - ширина экрана, Ywin - длина экрана,
													   // Ndrop - кол-во капель
	Particle rain[Ndrop];

	RenderWindow window(VideoMode(Xwin, Ywin), "Purple Rain", Style::Close);
	window.setFramerateLimit(60);



	RectangleShape drop;
	drop.setFillColor(Color(138, 43, 226));

	for (int i = 0, coin; i < Ndrop; i++) {	   // Начальные значения для капель
		rain[i].coor.x = rand() % (Xwin - 2) + 2;
		rain[i].coor.y = -(rand() % (Ywin / 2 - 2) + 2);	 // Отрицательное значение нужно,
															 // чтоб они спавнились заранее
		rain[i].l = rand() % 8 + 30;
		rain[i].w = rand() % 3 + 2;
		coin = rand() % 2;	 // Рандомизирование капель ака подкидывание монетки
		if (coin == 0) {     // Близкие капли		
			rain[i].v = rand() % 6 + 3;
			rain[i].alpha = rand() % 22 + 233;
		}
		else {				 // Далёкие капли
			rain[i].v = rand() % 4 + 2;
			rain[i].alpha = rand() % 50 + 100;
		}
	}

	sf::Clock clock;

	while (window.isOpen()) {
		Event evnt;
		Time deltaT = clock.restart();

		while (window.pollEvent(evnt)) {
			if (evnt.type == Event::Closed) {
				window.close();
			}
		}


		for (int i = 0; i < Ndrop; i++) {
			rain[i].coor.y += rain[i].v * (deltaT.asMilliseconds() / 10);
			drop.setPosition(rain[i].coor.x, rain[i].coor.y);
			drop.setFillColor(Color(138, 43, 226, rain[i].alpha));
			drop.setSize(Vector2f(rain[i].w, rain[i].l));

			if (rain[i].coor.y > Ywin) {		// Если заходит за нижнюю границу, то телепортирует вверх
				rain[i].coor.y = -(rand() % (Ywin / 2 - 2) + 2);
				rain[i].coor.x = rand() % (Xwin - 2) + 2;
			}
			window.draw(drop);
		}

		window.display();
		window.clear();

	}
}