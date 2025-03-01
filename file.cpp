#include <windows.h>
#include <stdint.h>
#include <iostream>
#include <math.h>


#define pi 3.1415926535
// Ekran boyutu
#define WIDTH  600
#define HEIGHT 600

// Bitmap verisini tutacak bellek alanı
uint32_t framebuffer[WIDTH * HEIGHT];

// Bellekte belirli bir pikseli koşula göre renklendiren fonksiyon
void SetPixelInMemory(int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

    framebuffer[y * WIDTH + x] = 0xFFFF0000; //y = 2 ise, 3. satira eris.
}

void _SetPixelInMemory(int x, int y,uintptr_t hex = 0x1229FA) {
    x += 300;
    y *= -1;
    y += 300;
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    framebuffer[y * WIDTH + x] = hex;
}

int mutlakdeger(const int &sayi)
{
    if (sayi < 0)
        return -(sayi);
    return sayi;
}

//Bresenham çizgi algoritması
void drawline(int x1, int y1, int x2, int y2)
{
    int __x = 1;
    int __y = 1;
    if (x1 > x2)
        __x = -1;
    if (y1 > y2)
        __y = -1;

    int yukseklik = mutlakdeger(y1 - y2); //yukari-asagi sayisi
    int genislik = mutlakdeger(x2 - x1);  //sag-sol sayisi

    int hata = yukseklik - genislik; //egim hata payi

    while (!(x1 == x2 && y1 == y2))
    {
        SetPixelInMemory(x1, y1); 

        if (hata * 2 > -genislik)
        {
            hata -= genislik;
            y1 += __y;
        }

        if (hata * 2 < yukseklik)
        {
            hata += yukseklik;
            x1 += __x;
        }
    }
}

void drawline2(int x1, int y1,int x2,int y2) //iki noktasi bilinen dogru denkleminden
{
    double y; double x;
    int sinirx1 = x1, sinirx2 = x2;
    int siniry1 = y1, siniry2 = y2;
    if (x1 > x2)
    {
        sinirx1 = x2;
        sinirx2 = x1;
    }
    if (y1 > y2)
    {
        siniry1 = y2;
        siniry2 = y1;
    }

    for (int i = sinirx1; i <= sinirx2; i++) //x'ler icin cizer.
    {
        x = i;
        int y = (x - x1) / (x1 - x2) * (y1 - y2) + y1;
        SetPixelInMemory(x, y);
    }

    for (int i = siniry1; i <= siniry2; i++) //y'ler icin cizer.
    {
        y = i;
        int x = (y - y1) * (x1 - x2) / (y1 - y2) + x1;
        SetPixelInMemory(x, y);
    }
}

//ekranin ortasini 0,0 kabul et
void _drawline(int x1, int y1, int x2, int y2)
{
    drawline(x1 + 300, y1 + 300, x2 + 300, y2 + 300);
}

double f(double x)
{
    return cos(x);
}

double f1(double x)
{
    if (-pi < x && x < 0)
        return -1;
    if (0 < x && x < pi)
        return 1;

    return 0; //tanımsız
}

void draw_function()
{
    for (int i = -100; i <= 100; i++)
    {
        _SetPixelInMemory(i, f1(i * 0.1) * 10, 0x008000);
    }
}


double bn(int n)
{
    return 2 / (n * pi) * (1 - pow(-1, n));
}

double f2(double x, int n, int sinir)
{
    if (n == sinir)
        return 0;
    return bn(n) * sin(n * x) + f2(x, n + 1, sinir);
}

void draw_function2()
{
    for (int i = -100; i <= 100; i++)
    {
        _SetPixelInMemory(i, f2(i * 0.1, 1, 10) * 10);
    }
}

// Pencere prosedürü
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
       
        //KORDINAT SISTEMINI ÇİZER.
        _drawline(0,210 ,0, -220);
        _drawline(220, 0, -220, 0);
        //draw_function();
        draw_function2();

        // Bitmap yapısını hazırla
        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = WIDTH;
        bmi.bmiHeader.biHeight = -HEIGHT; // Negatif değer ile bitmapin doğru çizilmesini sağlar
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        // Bellekteki framebuffer'ı ekrana çiz
        StretchDIBits(hdc,
            0, 0, WIDTH, HEIGHT,
            0, 0, WIDTH, HEIGHT,
            framebuffer, &bmi,
            DIB_RGB_COLORS, SRCCOPY);

        EndPaint(hwnd, &ps);
    }
                 break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// WinMain giriş noktası
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"RawPixelWindow";

    if (!RegisterClass(&wc))
        return 0;

    HWND hwnd = CreateWindow(
        L"RawPixelWindow", L"Ekrana çizim",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
