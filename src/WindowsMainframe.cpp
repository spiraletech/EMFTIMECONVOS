#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include "aether/AetherCore.hpp"
#include <array>
#include <fstream>
#include <string>
#include <exception>

namespace {
constexpr int kNav = 1000, kGeometry = 2001, kTone = 2002, kStop = 2003, kStatus = 2004;
HWND mainWindow{}, nav{}, title{}, description{}, status{};
aether::HardStop safety;
constexpr std::array<const wchar_t*, 12> tabs{
  L"HOME",L"RESONANCE",L"SIGNAL LAB",L"COMMUNICATE",L"ENTITY",L"SPOON FORK",
  L"CHRONOS",L"DRIFT ANCHOR",L"GUFF",L"RADAR",L"AUTOMATIONS",L"SETTINGS"
};
constexpr std::array<const wchar_t*, 12> descriptions{
  L"AETHER OS v0.1 desktop mainframe. Working: mathematical sacred geometry and offline sine-wave WAV generation. Additional instruments are in development.",
  L"Generate a bounded 528 Hz reference tone as a WAV file. This is offline file synthesis; it does not transmit to a speaker or microphone.",
  L"Planned: calibrated microphone input, spectrum analysis, phase, and pseudo-relay detection. Not implemented in this build.",
  L"Planned: structured experimental communication investigations with strict source provenance. No synthetic messages presented as received.",
  L"Planned: evidence-based anomaly catalog, repeatability tests and alternative-source hypotheses.",
  L"Planned: astronomical ephemerides, symbolic chart interpretations and practical geographic comparisons.",
  L"Planned: temporal experiments, clock drift and versioned future-self planning.",
  L"HARDSTOP blocks new waveform generation. Reset requires deliberate operator action.",
  L"Planned: GOLF-GUFF local inference and repository-aware retrieval. No language model is embedded in this build.",
  L"Planned: online-optional research indexing and verified article discovery.",
  L"Planned: permission-controlled tasks, CI checks, error diagnosis and GUFF benchmark evaluation.",
  L"Planned: device calibration, local privacy, resource limits and model configurations."
};
void setText(HWND w,const wchar_t* s){SetWindowTextW(w,s);}
void render() {
 const int i=static_cast<int>(SendMessageW(nav,LB_GETCURSEL,0,0));
 const int index=(i>=0 && i<static_cast<int>(tabs.size()))?i:0;
 setText(title,tabs[index]);setText(description,descriptions[index]);
 setText(status,safety.engaged()?L"STATUS: HARDSTOP ENGAGED - synthesis blocked":L"STATUS: READY - offline/simulated features labeled");
}
std::wstring chooseFile(HWND owner,const wchar_t* filter,const wchar_t* defaultName,const wchar_t* ext) {
 wchar_t path[MAX_PATH]{}; lstrcpynW(path,defaultName,MAX_PATH);
 OPENFILENAMEW ofn{}; ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=owner;
 ofn.lpstrFilter=filter;ofn.lpstrFile=path;ofn.nMaxFile=MAX_PATH;
 ofn.lpstrDefExt=ext;ofn.Flags=OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
 if(!GetSaveFileNameW(&ofn)) return {};
 return path;
}
void notify(const wchar_t* message){MessageBoxW(mainWindow,message,L"AETHER OS",MB_OK|MB_ICONINFORMATION);}
void saveGeometry(){
 const auto path=chooseFile(mainWindow,L"SVG Image (*.svg)\0*.svg\0\0",L"aether_geometry.svg",L"svg");
 if(path.empty())return;
 std::ofstream out(path,std::ios::binary);
 if(!out){notify(L"Could not write geometry file.");return;}
 out<<aether::geometrySvg(3,40.0);
 if(!out){notify(L"Geometry export failed.");return;}
 notify(L"Saved a real mathematical geometry SVG file.");
}
void saveTone(){
 if(safety.engaged()){notify(L"HARDSTOP is active. Reset it before generating new audio.");return;}
 const auto path=chooseFile(mainWindow,L"Wave Audio (*.wav)\0*.wav\0\0",L"aether_528Hz.wav",L"wav");
 if(path.empty())return;
 const auto samples=aether::synthesizeSine(528.0,2.0,48000,0.1,safety);
 if(samples.empty()){notify(L"Generation cancelled by HARDSTOP.");return;}
 if(!aether::writeWavMono16(std::string(path.begin(),path.end()),samples,48000)){
   notify(L"WAV save failed. Choose a simple ASCII filename in this alpha.");return;
 }
 notify(L"Saved 528 Hz reference WAV (offline file only).");
}
LRESULT CALLBACK windowProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 switch(msg){
 case WM_CREATE: {
  HFONT font=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
  nav=CreateWindowExW(WS_EX_CLIENTEDGE,L"LISTBOX",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|LBS_NOTIFY,
                      16,70,205,430,hwnd,(HMENU)(INT_PTR)kNav,nullptr,nullptr);
  for(auto* tab:tabs)SendMessageW(nav,LB_ADDSTRING,0,(LPARAM)tab);
  SendMessageW(nav,LB_SETCURSEL,0,0);
  title=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE,245,76,490,38,hwnd,nullptr,nullptr,nullptr);
  description=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE|SS_LEFT,245,135,495,110,hwnd,nullptr,nullptr,nullptr);
  status=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE,245,370,490,55,hwnd,nullptr,nullptr,nullptr);
  const auto b1=CreateWindowW(L"BUTTON",L"Export Geometry SVG",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,245,270,220,38,hwnd,(HMENU)(INT_PTR)kGeometry,nullptr,nullptr);
  const auto b2=CreateWindowW(L"BUTTON",L"Export 528 Hz WAV",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,485,270,220,38,hwnd,(HMENU)(INT_PTR)kTone,nullptr,nullptr);
  const auto stop=CreateWindowW(L"BUTTON",L"HARDSTOP",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,245,317,220,38,hwnd,(HMENU)(INT_PTR)kStop,nullptr,nullptr);
  const auto reset=CreateWindowW(L"BUTTON",L"Operator Reset",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,485,317,220,38,hwnd,(HMENU)(INT_PTR)kStatus,nullptr,nullptr);
  for(HWND c:{nav,title,description,status,b1,b2,stop,reset})SendMessageW(c,WM_SETFONT,(WPARAM)font,TRUE);
  render();return 0;
 }
 case WM_COMMAND:
  if(LOWORD(wp)==kNav && HIWORD(wp)==LBN_SELCHANGE){render();return 0;}
  try{
   switch(LOWORD(wp)){
   case kGeometry: saveGeometry();return 0;
   case kTone: saveTone();return 0;
   case kStop: safety.engage();render();return 0;
   case kStatus:
    if(MessageBoxW(hwnd,L"Reset HARDSTOP and allow new offline file synthesis?",L"Operator authorization",MB_YESNO|MB_ICONQUESTION)==IDYES)safety.resetByOperator();
    render();return 0;
   }
  }catch(const std::exception&){notify(L"Operation failed. Check inputs and disk access.");return 0;}
  break;
 case WM_DESTROY:safety.engage();PostQuitMessage(0);return 0;
 }
 return DefWindowProcW(hwnd,msg,wp,lp);
}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,LPWSTR,int show){
 WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.hInstance=instance;wc.lpfnWndProc=windowProc;
 wc.lpszClassName=L"AetherMainframe001";wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
 wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
 if(!RegisterClassExW(&wc))return 1;
 mainWindow=CreateWindowExW(0,wc.lpszClassName,L"AETHER OS | An App for the Future",WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT,CW_USEDEFAULT,790,570,nullptr,nullptr,instance,nullptr);
 if(!mainWindow)return 2;
 ShowWindow(mainWindow,show);UpdateWindow(mainWindow);
 MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}
 return static_cast<int>(msg.wParam);
}
