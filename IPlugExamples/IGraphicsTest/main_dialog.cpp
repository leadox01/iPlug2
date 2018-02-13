#include "IPlugPlatform.h"
#ifdef OS_MAC
  #include "IGraphicsMac.h"
  #define PostQuitMessage SWELL_PostQuitMessage
#else
  #include "IGraphicsWin.h"
#endif

#include "IGraphicsTest.h"
#include "IPlugParameter.h"
#include "IControls.h"

#define UI_WIDTH 700
#define UI_HEIGHT 700

void CenterWindow(HWND hWnd, int w, int h)
{
  const int screenwidth = GetSystemMetrics(SM_CXSCREEN);
  const int screenheight = GetSystemMetrics(SM_CYSCREEN);
  const int x = (screenwidth / 2) - (w / 2);
  const int y = (screenheight / 2) - (h / 2);
  
  RECT rcClient, rcWindow;
  GetClientRect(hWnd, &rcClient);
  GetWindowRect(hWnd, &rcWindow);
  
  POINT ptDiff;
  ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
  ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
  SetWindowPos(gHWND, 0, x, y, w + ptDiff.x, h + ptDiff.y, 0);
}

class MyDelegate: public IDelegate
{
public:
  //IDelegate
  IParam* GetParamFromUI(int paramIdx) override { return nullptr; }
  void SetParameterValueFromUI(int paramIdx, double value) override { DBGMSG("p %i %f!\n", paramIdx, value); }
  void BeginInformHostOfParamChangeFromUI(int paramIdx) override { ; }
  void EndInformHostOfParamChangeFromUI(int paramIdx) override { ; }
};

MyDelegate dummyDelegate;

enum EParams
{
  kGain = 0,
  kNumParams
};

void CreateGraphics()
{
#ifdef OS_MAC
  IGraphicsMac* pGraphics = new IGraphicsMac(dummyDelegate, UI_WIDTH, UI_HEIGHT, 60);
  pGraphics->SetBundleID("com.OliLarkin.app.IGraphicsTest");
  pGraphics->HandleMouseOver(true);
  pGraphics->CreateMetalLayer();
#else
  IGraphicsWin* pGraphics = new IGraphicsWin(dummyDelegate, UI_WIDTH, UI_HEIGHT, 60);
  pGraphics->SetPlatformInstance(gHINSTANCE);
#endif

  pGraphics->OpenWindow((void*) gHWND);
  pGraphics->AttachPanelBackground(COLOR_RED);
  
  /*ISVG svg = pGraphics->LoadSVG("resources/img/BefacoBigKnob.svg");*/
  //pGraphics->AttachControl(new IVSVGKnob(dummyDelegate, pGraphics->GetBounds().GetPadded(-20), svg, kGain));
  const int nRows = 15;
  const int nColumns = 15;
  IRECT bounds = pGraphics->GetBounds();

  for (int i = 0; i < nRows * nColumns; i++)
  {
    IRECT cellBounds = bounds.GetGridCell(i, nRows, nColumns).GetPadded(-5.);
    pGraphics->AttachControl(new IVKnobControl(dummyDelegate, cellBounds, kGain));
  }
}

//static
WDL_DLGRET MainDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_INITDIALOG:
    {
      gHWND = hwndDlg;
      
      CreateGraphics();
      CenterWindow(gHWND, UI_WIDTH, UI_HEIGHT);
      ShowWindow(gHWND, SW_SHOW);
     
      return 1;
    }
    case WM_DESTROY:
      gHWND = NULL;
      PostQuitMessage(gHWND);
      return 0;
    case WM_CLOSE:
      DestroyWindow(hwndDlg);
      return 0;
    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case ID_QUIT:
          DestroyWindow(hwndDlg);
          return 0;
        case ID_ABOUT:
        {
          WDL_String version;
          version.SetFormatted(100, "Built on %s", __DATE__);
          MessageBox(hwndDlg, version.Get(), "IGraphicsTest", MB_OK);
          return 0;
        }
      }
      return 0;
  }
  return 0;
}
