#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "WindowsOverlay.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>  // For malloc/free

using namespace Gdiplus;

WindowsOverlay::WindowsOverlay()
    : m_hwnd(nullptr)
    , m_hdc(nullptr)
    , m_memDC(nullptr)
    , m_hBitmap(nullptr)
    , m_hOldBitmap(nullptr)
    , m_screenWidth(0)
    , m_screenHeight(0)
    , m_currentIndex(0)
    , m_gdiplusToken(0)
{
    m_trailParts.reserve(MAX_TRAIL_PARTS);
    for (size_t i = 0; i < MAX_TRAIL_PARTS; ++i) {
        m_trailParts.emplace_back(0.0f, 0.0f, 0.0f);
    }
}

WindowsOverlay::~WindowsOverlay()
{
    Cleanup();
}

bool WindowsOverlay::Initialize()
{
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr) != Ok) {
        std::cerr << "Failed to initialize GDI+" << std::endl;
        return false;
    }

    // Get screen dimensions
    m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    m_screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"CursorTrailOverlay";
    
    if (!RegisterClassExW(&wc)) {
        std::cerr << "Failed to register window class" << std::endl;
        GdiplusShutdown(m_gdiplusToken);
        return false;
    }

    // Create layered window for transparent overlay
    m_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        L"CursorTrailOverlay",
        L"Cursor Trail",
        WS_POPUP,
        0, 0, m_screenWidth, m_screenHeight,
        nullptr, nullptr, GetModuleHandle(nullptr), this
    );

    if (!m_hwnd) {
        std::cerr << "Failed to create overlay window" << std::endl;
        GdiplusShutdown(m_gdiplusToken);
        return false;
    }

    // Set window to be click-through and fully transparent initially
    SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // Create memory DC for double buffering
    m_hdc = GetDC(m_hwnd);
    m_memDC = CreateCompatibleDC(m_hdc);
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_screenWidth;
    bmi.bmiHeader.biHeight = -m_screenHeight; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits;
    m_hBitmap = CreateDIBSection(m_memDC, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    m_hOldBitmap = (HBITMAP)SelectObject(m_memDC, m_hBitmap);

    // Load trail texture from the same PNG file used by OpenGL version
    std::wstring texturePath = L"cursortrail.png";
    m_trailTexture = std::unique_ptr<Bitmap>(Bitmap::FromFile(texturePath.c_str()));
    
    if (!m_trailTexture || m_trailTexture->GetLastStatus() != Ok) {
        std::cout << "Failed to load cursortrail.png, creating fallback texture" << std::endl;
        // Fallback: Create a simple gradient circle texture
        m_trailTexture = std::make_unique<Bitmap>(static_cast<INT>(SPRITE_SIZE), static_cast<INT>(SPRITE_SIZE), PixelFormat32bppARGB);
        Graphics textureGraphics(m_trailTexture.get());
        textureGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
        
        // Create a radial gradient brush for better visual effect
        GraphicsPath path;
        path.AddEllipse(0, 0, static_cast<INT>(SPRITE_SIZE), static_cast<INT>(SPRITE_SIZE));
        PathGradientBrush brush(&path);
        
        Color colors[] = { Color(255, 255, 255, 255), Color(0, 255, 255, 255) }; // White center, transparent edge
        REAL positions[] = { 0.0f, 1.0f };
        brush.SetInterpolationColors(colors, positions, 2);
        
        textureGraphics.FillEllipse(&brush, 0, 0, static_cast<INT>(SPRITE_SIZE), static_cast<INT>(SPRITE_SIZE));
    } else {
        std::cout << "Successfully loaded cursortrail.png texture (" << m_trailTexture->GetWidth() << "x" << m_trailTexture->GetHeight() << ")" << std::endl;
    }

    ShowWindow(m_hwnd, SW_SHOW);

    return true;
}

void WindowsOverlay::Update()
{
    if (!m_hwnd) return;

    // Get global cursor position
    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
        TrailPart currentTrail(static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y), FADE_TIME);
        AddTrailPart(currentTrail);
        UpdateTrail();
        
        // Debug output (first few seconds only)
        static int debugCounter = 0;
        if (debugCounter < 60) { // Print for first 60 frames only
            std::cout << "Cursor at: " << cursorPos.x << "," << cursorPos.y << " Trail parts active: ";
            int activeCount = 0;
            for (const auto& part : m_trailParts) {
                if (part.time > 0.0f) activeCount++;
            }
            std::cout << activeCount << std::endl;
            debugCounter++;
        }
    }

    // Update trail fade times - match OpenGL version timing
    for (auto& part : m_trailParts) {
        if (part.time > 0.0f) {
            part.time -= 0.05f; // Match the OpenGL version fade rate (Game.cpp line 123)
            if (part.time < 0.0f) {
                part.time = 0.0f;
            }
        }
    }
}

void WindowsOverlay::UpdateTrail()
{
    // Interpolate between trail points for smoother trail
    size_t prevIndex = (m_currentIndex == 0) ? m_trailParts.size() - 1 : m_currentIndex - 1;
    
    const TrailPart& current = m_trailParts[m_currentIndex];
    const TrailPart& previous = m_trailParts[prevIndex];
    
    float dx = current.x - previous.x;
    float dy = current.y - previous.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance > INTERPOLATION_INTERVAL) {
        float dirX = dx / distance;
        float dirY = dy / distance;
        
        for (float d = INTERPOLATION_INTERVAL; d < distance; d += INTERPOLATION_INTERVAL) {
            float interpX = previous.x + dirX * d;
            float interpY = previous.y + dirY * d;
            AddTrailPart(TrailPart(interpX, interpY, FADE_TIME));
        }
    }
}

void WindowsOverlay::AddTrailPart(const TrailPart& part)
{
    m_trailParts[m_currentIndex] = part;
    m_currentIndex = (m_currentIndex + 1) % MAX_TRAIL_PARTS;
}

void WindowsOverlay::Render()
{
    if (!m_hwnd || !m_memDC) return;

    // Clear the memory DC with fully transparent pixels using Graphics
    Graphics clearGraphics(m_memDC);
    clearGraphics.Clear(Color(0, 0, 0, 0)); // Fully transparent

    // Draw trail using GDI+
    Graphics graphics(m_memDC);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetCompositingMode(CompositingModeSourceOver);
    graphics.SetCompositingQuality(CompositingQualityHighQuality);
    
    DrawTrail(graphics);

    // Update the layered window
    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { m_screenWidth, m_screenHeight };
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(m_hwnd, nullptr, nullptr, &sizeWnd, m_memDC, &ptSrc, RGB(0, 0, 0), &bf, ULW_ALPHA);
}

void WindowsOverlay::DrawTrail(Graphics& graphics)
{
    for (const auto& part : m_trailParts) {
        if (part.time > 0.0f) {
            // Calculate alpha based on remaining time (match OpenGL version logic)
            float alpha = (std::max)(0.0f, (std::min)(1.0f, part.time / FADE_TIME));
            
            // Scale the texture to match the sprite size
            float spriteSize = SPRITE_SIZE;
            float textureWidth = static_cast<float>(m_trailTexture->GetWidth());
            float textureHeight = static_cast<float>(m_trailTexture->GetHeight());
            
            // Create a color matrix for alpha blending
            ColorMatrix colorMatrix = {
                {
                    {1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
                    {0.0f, 0.0f, 0.0f, alpha, 0.0f},
                    {0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
                }
            };
            
            ImageAttributes imageAttributes;
            imageAttributes.SetColorMatrix(&colorMatrix);
            
            // Draw the trail sprite scaled from the texture size to sprite size
            RectF destRect(
                part.x - spriteSize / 2.0f,
                part.y - spriteSize / 2.0f,
                spriteSize,
                spriteSize
            );
            
            graphics.DrawImage(
                m_trailTexture.get(),
                destRect,
                0, 0, textureWidth, textureHeight,
                UnitPixel,
                &imageAttributes
            );
        }
    }
}

void WindowsOverlay::Cleanup()
{
    if (m_hOldBitmap && m_memDC) {
        SelectObject(m_memDC, m_hOldBitmap);
        m_hOldBitmap = nullptr;
    }
    
    if (m_hBitmap) {
        DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
    
    if (m_memDC) {
        DeleteDC(m_memDC);
        m_memDC = nullptr;
    }
    
    if (m_hdc && m_hwnd) {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = nullptr;
    }
    
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    
    if (m_gdiplusToken) {
        GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
}

LRESULT CALLBACK WindowsOverlay::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CREATE:
            {
                CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
                WindowsOverlay* pOverlay = reinterpret_cast<WindowsOverlay*>(pCreate->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pOverlay));
            }
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

#endif // _WIN32