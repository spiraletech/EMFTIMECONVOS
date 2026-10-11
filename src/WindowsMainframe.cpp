#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <array>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "aether/AetherCore.hpp"

namespace {
constexpr int kEdit=101;
HWND window{}, input{};
HFONT font{}, mono{};
aether::HardStop safety;
int selected=0, rings=2, phase=0;
bool animate=true;
std::wstring output=L"AETHER MAINFRAME ONLINE // enter HELP for commands";
const std::array<std::wstring,12> tabs={
 L"HOME",L"RESONANCE",L"SIGNAL LAB",L"COMMUNICATE",L"ENTITY",L"SPOON FORK",
 L"CHRONOS",L"DRIFT ANCHOR",L"GUFF",L"RADAR",L"AUTOMATIONS",L"SETTINGS"};
const std::array<std::wstring,12> desc={
 L"MISSION CONTROL / Mystery-driven engineering and intelligence workstation",
 L"RESONANCE ENGINE / Generate measurable audio and mathematical sacred geometry",
 L"SIGNAL LAB / Measurement adapters pending; never invent live sensor readings",
 L"COMMUNICATE / Experimental research; no fabricated messages or entity identification",
 L"ENTITY / Evidence-first hypothesis registry and source provenance",
 L"SPOON FORK / Astronomy, symbolic interpretations and geospatial research",
 L"CHRONOS / Clock studies, temporal experiments and future-self journals",
 L"DRIFT ANCHOR / Independent safety authority and deliberate reset",
 L"GOLF-GUFF / Local inference adapter not connected; repository registry available",
 L"RADAR / Online research adapters pending; source verification required",
 L"AUTOMATIONS / Controlled job scheduler and engineering evaluation planned",
 L"SETTINGS / Offline-first, local authority, Windows-native C++20"};
COLORREF ink=RGB(220,246,245),muted=RGB(123,164,176),cyan=RGB(82,233,219);
void rect(HDC dc,RECT r,COLORREF c){HBRUSH b=CreateSolidBrush(c);FillRect(dc,&r,b);DeleteObject(b);}
void frame(HDC dc,RECT r,COLORREF c){HPEN p=CreatePen(PS_SOLID,1,c);auto prev=SelectObject(dc,p);auto old=SelectObject(dc,GetStockObject(HOLLOW_BRUSH));Rectangle(dc,r.left,r.top,r.right,r.bottom);SelectObject(dc,old);SelectObject(dc,prev);DeleteObject(p);}
void text(HDC dc,int x,int y,const std::wstring& s,COLORREF col,int h=22,bool bold=false){
 LOGFONTW lf{};lf.lfHeight=-h;lf.lfWeight=bold?FW_BOLD:FW_NORMAL;lstrcpyW(lf.lfFaceName,L"Segoe UI");
 HFONT f=CreateFontIndirectW(&lf);auto old=SelectObject(dc,f);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,col);
 TextOutW(dc,x,y,s.c_str(),(int)s.size());SelectObject(dc,old);DeleteObject(f);
}
void line(HDC dc,int x,int y,int xx,int yy,COLORREF c,int width=1){
 HPEN p=CreatePen(PS_SOLID,width,c);auto o=SelectObject(dc,p);
 MoveToEx(dc,x,y,nullptr);LineTo(dc,xx,yy);SelectObject(dc,o);DeleteObject(p);
}
void disk(HDC dc,int x,int y,int rad,COLORREF color){
 HPEN p=CreatePen(PS_SOLID,1,color);auto oldp=SelectObject(dc,p);auto oldb=SelectObject(dc,GetStockObject(NULL_BRUSH));
 Ellipse(dc,x-rad,y-rad,x+rad,y+rad);SelectObject(dc,oldb);SelectObject(dc,oldp);DeleteObject(p);
}
void button(HDC dc,RECT r,const std::wstring& label,bool active=false){
 rect(dc,r,active?RGB(24,95,97):RGB(22,48,61));frame(dc,r,active?cyan:RGB(46,88,99));
 text(dc,r.left+13,r.top+8,label,active?RGB(220,255,251):ink,15,true);
}
RECT buttonRect(int i,int w,int h){
 int left=250, top=h-169, ww=(std::max(500,w-290))/3-9;
 return {left+(i%3)*(ww+8),top+(i/3)*42,left+(i%3)*(ww+8)+ww,top+(i/3)*42+35};
}
std::wstring saveDialog(const wchar_t* filter,const wchar_t* name,const wchar_t* ext){
 wchar_t path[MAX_PATH]{};lstrcpynW(path,name,MAX_PATH);
 OPENFILENAMEW o{};o.lStructSize=sizeof(o);o.hwndOwner=window;o.lpstrFilter=filter;o.lpstrFile=path;
 o.nMaxFile=MAX_PATH;o.lpstrDefExt=ext;o.Flags=OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
 return GetSaveFileNameW(&o)?std::wstring(path):L"";
}
void setOutput(std::wstring s){output=std::move(s);InvalidateRect(window,nullptr,FALSE);}
void geometry(){
 auto path=saveDialog(L"SVG drawing\0*.svg\0\0",L"AETHER_FlowerOfLife.svg",L"svg");
 if(path.empty())return;
 std::ofstream out{std::filesystem::path(path),std::ios::binary};
 if(!out){setOutput(L"ERROR: Unable to create SVG");return;}
 out<<aether::geometrySvg((unsigned)rings,40);
 setOutput(out?L"VERIFIED FILE OUTPUT: Geometry SVG exported":L"ERROR: SVG write failed");
}
void tone(){
 if(safety.engaged()){setOutput(L"HARDSTOP ACTIVE: synthesis denied");return;}
 auto path=saveDialog(L"WAV audio\0*.wav\0\0",L"AETHER_528Hz.wav",L"wav");
 if(path.empty())return;
 auto s=aether::synthesizeSine(528,2,48000,0.1,safety);
 if(s.empty()){setOutput(L"Generation cancelled by safety authority");return;}
 // The core WAV writer accepts UTF-8; keep ASCII-only path in this alpha.
 bool ascii=std::all_of(path.begin(),path.end(),[](wchar_t c){return c<128;});
 if(!ascii){setOutput(L"Save to an ASCII path in this alpha (Unicode path support pending)");return;}
 std::string p(path.begin(),path.end());
 setOutput(aether::writeWavMono16(p,s,48000)?L"VERIFIED FILE OUTPUT: 528 Hz PCM WAV exported":L"ERROR: WAV export failed");
}
void command(std::wstring cmd){
 std::transform(cmd.begin(),cmd.end(),cmd.begin(),[](wchar_t c){return (wchar_t)towupper(c);});
 if(cmd==L"HELP")setOutput(L"COMMANDS: HELP / STATUS / GEOMETRY / TONE / STOP / RESET / REPOS / GUFF / MYSTERY");
 else if(cmd==L"STATUS")setOutput(safety.engaged()?L"SAFETY: HARDSTOP ENGAGED":L"AETHER v0.2 / GUI LIVE / GUFF UNCONNECTED / DSP EXPORT ONLINE");
 else if(cmd==L"GEOMETRY")geometry();
 else if(cmd==L"TONE")tone();
 else if(cmd==L"STOP"){safety.engage();setOutput(L"HARDSTOP ENGAGED / synthetic audio generation inhibited");}
 else if(cmd==L"RESET"){if(MessageBoxW(window,L"Reset HARDSTOP? Only offline generation is enabled.",L"Operator approval",MB_YESNO|MB_ICONQUESTION)==IDYES){safety.resetByOperator();setOutput(L"OPERATOR RESET ACCEPTED");}}
 else if(cmd==L"REPOS")setOutput(L"21 registered GitHub repositories. Source indexing pending. See docs/repository_registry.json");
 else if(cmd==L"GUFF")setOutput(L"GUFF: MODEL NOT CONNECTED. No fake text generation. Adapter to GOLF-GUFF is next.");
 else if(cmd==L"MYSTERY")setOutput(L"MYSTERY ENGINE: create a hypothesis; measure, test, verify. Research workflow pending.");
 else setOutput(L"UNKNOWN COMMAND. Type HELP.");
}
void paint(HDC dc,int w,int h){
 rect(dc,{0,0,w,h},RGB(7,18,28));
 rect(dc,{0,0,w,58},RGB(12,33,46));line(dc,0,57,w,57,RGB(35,86,97));
 text(dc,22,13,L"⬡  AETHER OS",RGB(196,255,246),27,true);
 text(dc,244,20,L"AN APP FOR THE FUTURE  /  SPIRAL OS",muted,15);
 text(dc,w-210,20,L"●  LOCAL MAINFRAME",cyan,14,true);
 rect(dc,{0,58,226,h},RGB(10,28,40));
 text(dc,20,79,L"WORKSPACES  /  12",RGB(91,159,174),13,true);
 for(int i=0;i<12;++i){
  RECT r{12,110+i*36,214,143+i*36};
  if(r.bottom>h-10)break;
  if(i==selected){rect(dc,r,RGB(22,73,80));rect(dc,{12,r.top,16,r.bottom},cyan);}
  text(dc,26,r.top+7,(i<9?L"0":L"")+std::to_wstring(i+1)+L"    "+tabs[i],i==selected?ink:muted,14,i==selected);
 }
 const int cx=std::max(400,(w+235)/2), cy=std::max(235,(h-128)/2);
 text(dc,250,82,tabs[selected],ink,29,true);
 text(dc,251,124,desc[selected],muted,15);
 RECT panel{245,165,w-23,std::max(330,h-192)};
 rect(dc,panel,RGB(9,32,44));frame(dc,panel,RGB(32,87,99));
 text(dc,265,182,L"SACRED GEOMETRY  /  PARAMETRIC HEXAGONAL FIELD",RGB(102,217,209),13,true);
 int radius=std::min({(w-310)/7,(h-370)/5,55});radius=std::max(12,radius);
 int shift=animate?phase:0;
 for(int q=-rings;q<=rings;++q)for(int r=-rings;r<=rings;++r){
  if(std::max({std::abs(q),std::abs(r),std::abs(q+r)})>rings)continue;
  int px=cx+(int)((q+r*0.5)*radius*1.5);
  int py=cy+(int)(r*std::sqrt(3.0)*radius*0.76);
  disk(dc,px,py,radius,((q+r+shift)%3==0)?cyan:RGB(43,133,146));
 }
 disk(dc,cx,cy,6,ink);
 text(dc,267,panel.bottom-43,L"GENERATIVE MATHEMATICS  /  NOT A SENSOR READING",muted,12);
 const std::array<std::wstring,6> labels={L"EXPORT GEOMETRY",L"EXPORT 528 HZ",L"HARDSTOP",L"RINGS -",L"RINGS +",L"RUN STATUS"};
 for(int i=0;i<6;++i)button(dc,buttonRect(i,w,h),labels[i],i==2&&safety.engaged());
 text(dc,250,h-76,L"TERMINAL  >",cyan,14,true);
 RECT terminal{348,h-81,w-22,h-47};frame(dc,terminal,RGB(41,108,117));
 text(dc,250,h-35,output,safety.engaged()?RGB(255,175,133):muted,13);
}
LRESULT CALLBACK proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 switch(msg){
 case WM_CREATE:{
  input=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,357,0,260,27,hwnd,(HMENU)(INT_PTR)kEdit,nullptr,nullptr);
  font=CreateFontW(-17,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Consolas");
  SendMessageW(input,WM_SETFONT,(WPARAM)font,TRUE);
  SetTimer(hwnd,1,180,nullptr);return 0;
 }
 case WM_SIZE:{RECT r{};GetClientRect(hwnd,&r);MoveWindow(input,357,r.bottom-77,std::max(120,r.right-390),25,TRUE);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
 case WM_TIMER:if(animate){phase=(phase+1)%6;InvalidateRect(hwnd,nullptr,FALSE);}return 0;
 case WM_KEYDOWN:if(wp==VK_RETURN){wchar_t buf[512]{};GetWindowTextW(input,buf,512);SetWindowTextW(input,L"");command(buf);return 0;}break;
 case WM_COMMAND:if(LOWORD(wp)==kEdit && HIWORD(wp)==EN_CHANGE)return 0;break;
 case WM_LBUTTONDOWN:{
  RECT r{};GetClientRect(hwnd,&r);int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);
  if(x>=12&&x<224&&y>=110&&y<110+36*12){selected=std::clamp((y-110)/36,0,11);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
  for(int i=0;i<6;++i){auto b=buttonRect(i,r.right,r.bottom);
   if(x<b.left||x>b.right||y<b.top||y>b.bottom)continue;
   try{switch(i){case 0:geometry();break;case 1:tone();break;
    case 2:safety.engage();setOutput(L"HARDSTOP ENGAGED");break;
    case 3:rings=std::max(0,rings-1);break;case 4:rings=std::min(5,rings+1);break;
    case 5:command(L"STATUS");break;}}catch(...){setOutput(L"ERROR: operation failed safely");}
   InvalidateRect(hwnd,nullptr,FALSE);return 0;
  }
  break;
 }
 case WM_PAINT:{
  PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);RECT r{};GetClientRect(hwnd,&r);
  HDC mem=CreateCompatibleDC(dc);HBITMAP bitmap=CreateCompatibleBitmap(dc,std::max(1,r.right),std::max(1,r.bottom));
  auto old=SelectObject(mem,bitmap);paint(mem,r.right,r.bottom);
  BitBlt(dc,0,0,r.right,r.bottom,mem,0,0,SRCCOPY);
  SelectObject(mem,old);DeleteObject(bitmap);DeleteDC(mem);EndPaint(hwnd,&ps);return 0;
 }
 case WM_DESTROY:safety.engage();KillTimer(hwnd,1);if(font)DeleteObject(font);PostQuitMessage(0);return 0;
 }
 return DefWindowProcW(hwnd,msg,wp,lp);
}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,LPWSTR,int show){
 WNDCLASSW wc{};wc.hInstance=instance;wc.lpfnWndProc=proc;wc.lpszClassName=L"AetherMainframeCustom";
 wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
 if(!RegisterClassW(&wc))return 1;
 window=CreateWindowExW(0,wc.lpszClassName,L"AETHER OS  //  AN APP FOR THE FUTURE",
 WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,1180,790,nullptr,nullptr,instance,nullptr);
 if(!window)return 2;
 ShowWindow(window,show);UpdateWindow(window);
 MSG m{};while(GetMessageW(&m,nullptr,0,0)>0){if(m.message==WM_KEYDOWN&&m.wParam==VK_RETURN){
  wchar_t buf[512]{};GetWindowTextW(input,buf,512);SetWindowTextW(input,L"");command(buf);continue;
 }TranslateMessage(&m);DispatchMessageW(&m);}
 return (int)m.wParam;
}
