// GazeSampleSynth.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GazeSampleSynth.h"
#include "TouchElement.h"
#include "audio/AudioEngine.h"
#include "audio/MusicalSoundGenerator.h"
#include "audio/SineMonoSynth.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// added manually, move to a final location later on
TouchElement** touchElements;
AudioEngine* ae;
int lastTouchElementActivated = -1;

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAZESAMPLESYNTH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAZESAMPLESYNTH));

    MSG msg;

    // Main message loop:
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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAZESAMPLESYNTH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAZESAMPLESYNTH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable


   touchElements = (TouchElement**)malloc(64 * sizeof(TouchElement*));
   for(UINT8 c=0;c<64;c++)
   {
       *(touchElements + c) = NULL;
   }
   TouchElement * te = new TouchElement();
   SineMonoSynth* exampleSynth;
   exampleSynth = new SineMonoSynth();

   touchElements[0] = te;
   te->setPosX(20);
   te->setPosY(20);
   te->setWidth(100);
   te->setHeight(100);
   te->setCornerRadius(5);
   te->setSoundGeneratorIndex(0);
   ae = new AudioEngine();
   ae->initAudioEngine();
   exampleSynth->setNote(0.0f);
   exampleSynth->setAttack(0.05f);
   exampleSynth->setDecay(0.0f);
   exampleSynth->setSustain(1.0f);
   exampleSynth->setRelease(0.95f);
   ae->addSoundGenerator(exampleSynth);
   
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
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int xPos;
    int yPos;
    bool doRedraw = false;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
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
            RECT windowRect;
            GetWindowRect(hWnd, &windowRect);
            for (UINT8 c = 0; c < 64; c++)
            {
                if (*(touchElements + c) != NULL)
                {
                    (*(touchElements + c))->paint(hdc);
                }
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
        xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        doRedraw = false;
        for (UINT8 c = 0; c < 64; c++)
        {
            if (*(touchElements + c) != NULL)
            {
                if ((*(touchElements + c))->isOnTouchElement(xPos, yPos))
                {
                    (*(touchElements + c))->setActivated(true);
                    doRedraw = true;
                    if ((*(touchElements + c))->getSoundGeneratorIndex() >= 0)
                    {
                        ae->getSoundGenerator((*(touchElements + c))->getSoundGeneratorIndex())->switchOn(1.0f);
                    }
                    lastTouchElementActivated = c;
                }
            }
        }
        if (doRedraw)
        {
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_LBUTTONUP:
        xPos = GET_X_LPARAM(lParam);
        yPos = GET_Y_LPARAM(lParam);
        doRedraw = false;
        
        for (UINT8 c = 0; c < 64; c++)
        {
            if (*(touchElements + c) != NULL)
            {
                if ((*(touchElements + c))->isOnTouchElement(xPos, yPos))
                {
                    (*(touchElements + c))->setActivated(false);
                    lastTouchElementActivated = -1;
                    doRedraw = true;
                    if ((*(touchElements + c))->getSoundGeneratorIndex() >= 0)
                    {
                        ae->getSoundGenerator((*(touchElements + c))->getSoundGeneratorIndex())->switchOff(1.0f);
                    }
                }
            }
        }
        if (doRedraw)
        {
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_MOUSEMOVE:
        doRedraw=false;
        
        if (lastTouchElementActivated >= 0)
        {
            if ((*(touchElements + lastTouchElementActivated)) != NULL)
            {
                xPos = GET_X_LPARAM(lParam);
                yPos = GET_Y_LPARAM(lParam);
                if (!(*(touchElements + lastTouchElementActivated))->isOnTouchElement(xPos, yPos))
                {
                    (*(touchElements + lastTouchElementActivated))->setActivated(false);
                    doRedraw = true;
                    if ((*(touchElements + lastTouchElementActivated))->getSoundGeneratorIndex() >= 0)
                    {
                        ae->getSoundGenerator((*(touchElements + lastTouchElementActivated))->getSoundGeneratorIndex())->switchOff(1.0f);
                    }
                    lastTouchElementActivated = -1;
                }
                
            }
        }
        if (doRedraw)
        {
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

