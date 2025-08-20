#ifndef WINDOWS_OVERLAY_H
#define WINDOWS_OVERLAY_H

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <objbase.h>
#include <propidl.h>
#include <gdiplus.h>
#include <vector>
#include <memory>
#include "TrailPart.h"

// Windows-specific overlay implementation for guaranteed top-level transparent overlay
class WindowsOverlay
{
public:
    WindowsOverlay();
    ~WindowsOverlay();
    
    bool Initialize();
    void Update();
    void Render();
    void Cleanup();
    
    bool IsActive() const { return m_hwnd != nullptr; }
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void DrawTrail(Gdiplus::Graphics& graphics);
    void AddTrailPart(const TrailPart& part);
    void UpdateTrail();
    
    HWND m_hwnd;
    HDC m_hdc;
    HDC m_memDC;
    HBITMAP m_hBitmap;
    HBITMAP m_hOldBitmap;
    
    int m_screenWidth;
    int m_screenHeight;
    
    std::vector<TrailPart> m_trailParts;
    size_t m_currentIndex;
    static const size_t MAX_TRAIL_PARTS = 2048;
    
    std::unique_ptr<Gdiplus::Bitmap> m_trailTexture;
    ULONG_PTR m_gdiplusToken;
    
    // Trail settings
    static constexpr float SPRITE_SIZE = 15.0f;
    static constexpr float FADE_TIME = 1.0f;
    static constexpr float INTERPOLATION_INTERVAL = SPRITE_SIZE / 2.5f; // Match OpenGL version exactly (6.0f)
};

#endif // _WIN32

#endif // WINDOWS_OVERLAY_H