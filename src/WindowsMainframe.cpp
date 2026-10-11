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
const std::array<std::array<const wchar_t*,6>,12> controls{{
 {L"OPEN RESONANCE",L"OPEN GUFF",L"HARDSTOP",L"OPEN RADAR",L"OPEN SIGNAL",L"STATUS"},
 {L"EXPORT GEOMETRY",L"EXPORT 528 HZ",L"HARDSTOP",L"RINGS -",L"RINGS +",L"STATUS"},
 {L"STATUS",L"SIGNAL HELP",L"HARDSTOP",L"OPEN RESONANCE",L"OPEN ENTITY",L"STATUS"},
 {L"CONTACT RULES",L"OPEN ENTITY",L"HARDSTOP",L"OPEN SIGNAL",L"OPEN POLYGLOT",L"STATUS"},
 {L"EVIDENCE RULES",L"OPEN SIGNAL",L"HARDSTOP",L"OPEN RADAR",L"OPEN GUFF",L"STATUS"},
 {L"ASTRO STATUS",L"OPEN CHRONOS",L"HARDSTOP",L"OPEN RADAR",L"OPEN GUFF",L"STATUS"},
 {L"CHRONOS STATUS",L"OPEN SPOON FORK",L"HARDSTOP",L"OPEN GUFF",L"OPEN RADAR",L"STATUS"},
 {L"SAFETY STATUS",L"OPERATOR RESET",L"HARDSTOP",L"OPEN HOME",L"OPEN SETTINGS",L"STATUS"},
 {L"GUFF STATUS",L"REPOSITORIES",L"HARDSTOP",L"OPEN RADAR",L"OPEN AUTOMATIONS",L"STATUS"},
 {L"RADAR STATUS",L"REPOSITORIES",L"HARDSTOP",L"OPEN GUFF",L"OPEN ENTITY",L"STATUS"},
 {L"AUTOMATION STATUS",L"GUFF STATUS",L"HARDSTOP",L"OPEN SETTINGS",L"OPEN HOME",L"STATUS"},
 {L"DEVICE STATUS",L"SAFETY STATUS",L"HARDSTOP",L"OPEN GUFF",L"OPEN RESONANCE",L"STATUS"}
}};
void changeTab(int tab){selected=std::clamp(tab,0,11);InvalidateRect(window,nullptr,FALSE);}
void tabAction(int index){
 if(index==2){safety.engage();setOutput(L"HARDSTOP ENGAGED");return;}
 if(selected==1) {
  if(index==0){geometry();return;}if(index==1){tone();return;}
  if(index==3){rings=std::max(0,rings-1);return;}
  if(index==4){rings=std::min(5,rings+1);return;}
 }
 if(selected==7 && index==1){command(L"RESET");return;}
 if(index==5){command(L"STATUS");return;}
 const auto label=std::wstring(controls[selected][index]);
 if(label.find(L"REPOSITORIES")!=std::wstring::npos){command(L"REPOS");return;}
 if(label.find(L"GUFF STATUS")!=std::wstring::npos){command(L"GUFF");return;}
 if(label.find(L"OPEN ")==0){
  const std::wstring target=label.substr(5);
  if(target==L"POLYGLOT"){setOutput(L"POLYGLOT module scheduled for GUFF; translation not yet connected");return;}
  for(int i=0;i<12;++i)if(tabs[i]==target){changeTab(i);return;}
  if(target==L"SIGNAL")changeTab(2);
  else if(target==L"AUTOMATIONS")changeTab(10);
  else if(target==L"RESONANCE")changeTab(1);
  else if(target==L"ENTITY")changeTab(4);
  else if(target==L"RADAR")changeTab(9);
  else if(target==L"CHRONOS")changeTab(6);
  else if(target==L"SPOON FORK")changeTab(5);
  else if(target==L"SETTINGS")changeTab(11);
  else if(target==L"GUFF")changeTab(8);
  else if(target==L"HOME")changeTab(0);
  return;
 }
 setOutput(label+L" / MODULE NOT CONNECTED. No fabricated data.");
}
void moduleCard(HDC dc,RECT r,const std::wstring& heading,const std::wstring& value,const std::wstring& foot){
 rect(dc,r,RGB(14,48,60));frame(dc,r,RGB(40,95,105));
 text(dc,r.left+15,r.top+12,heading,muted,13,true);
 text(dc,r.left+15,r.top+40,value,ink,20,true);
 text(dc,r.left+15,r.top+76,foot,RGB(89,202,191),12);
}
void waveform(HDC dc,int left,int top,int width,int height){
 line(dc,left,top+height/2,left+width,top+height/2,RGB(35,88,100));
 for(int x=0;x<width-1;++x){
  double a=std::sin((x+phase*5)*0.075);
  double b=std::sin((x+1+phase*5)*0.075);
  line(dc,left+x,top+height/2-(int)(height*.38*a),left+x+1,top+height/2-(int)(height*.38*b),cyan,2);
 }
}
void paintWorkspace(HDC dc,RECT r){
 const int w=r.right-r.left,h=r.bottom-r.top,cx=(r.left+r.right)/2,cy=(r.top+r.bottom)/2;
 text(dc,r.left+19,r.top+15,tabs[selected]+L"  /  INSTRUMENT "+std::to_wstring(selected+1),cyan,14,true);
 switch(selected){
 case 0:
  moduleCard(dc,{r.left+17,r.top+62,r.left+w/2-6,r.top+174},L"GUFF INTELLIGENCE",L"ADAPTER PENDING",L"21 repositories registered");
  moduleCard(dc,{r.left+w/2+6,r.top+62,r.right-17,r.top+174},L"SAFETY AUTHORITY",safety.engaged()?L"STOPPED":L"READY",L"Offline controls operational");
  moduleCard(dc,{r.left+17,r.top+195,r.left+w/2-6,r.top+307},L"GEOMETRY ENGINE",L"GENERATIVE",L"Export functional SVG");
  moduleCard(dc,{r.left+w/2+6,r.top+195,r.right-17,r.top+307},L"SIGNAL INSTRUMENT",L"NOT CONNECTED",L"No synthetic receiver readings");
  break;
 case 1:{
  text(dc,r.left+17,r.top+43,L"SACRED GEOMETRY / PARAMETRIC",muted,13);
  int radius=std::max(11,std::min({w/11,h/8,43}));
  for(int q=-rings;q<=rings;++q)for(int z=-rings;z<=rings;++z){
    if(std::max({std::abs(q),std::abs(z),std::abs(q+z)})>rings)continue;
    int px=cx+(int)((q+z*.5)*radius*1.5),py=cy+(int)(z*std::sqrt(3.0)*radius*.76);
    disk(dc,px,py,radius,((q+z+phase)%3==0)?cyan:RGB(43,133,146));
  }
  disk(dc,cx,cy,6,ink);
  text(dc,r.left+17,r.bottom-54,L"RINGS: "+std::to_wstring(rings)+L"    SINE: 528 Hz    GAIN: 0.10",ink,15,true);
  text(dc,r.left+17,r.bottom-29,L"Mathematical synthesis / exports only, not live sensor acquisition",muted,12);
  break;
 }
 case 2:
  text(dc,r.left+20,r.top+50,L"PRISM SPECTRAL INSTRUMENT / NO DEVICE CONNECTED",RGB(255,191,140),15,true);
  for(int i=1;i<=6;++i)line(dc,r.left+20,r.top+90+i*35,r.right-20,r.top+90+i*35,RGB(27,69,82));
  text(dc,r.left+22,r.bottom-62,L"RX: unavailable     TX: no transmission     CALIBRATION: pending",ink,14);
  text(dc,r.left+22,r.bottom-33,L"FFT and WASAPI integration required before plotting real signal data.",muted,12);break;
 case 3:
  text(dc,r.left+20,r.top+52,L"SÉANCE / EXTRATERRESTRIAL CONTACT RESEARCH",ink,19,true);
  moduleCard(dc,{r.left+18,r.top+107,r.right-18,r.top+222},L"RECEIVER ORIGIN",L"UNRESOLVED",L"No live hardware; no generated contact messages");
  text(dc,r.left+20,r.bottom-64,L"EXPERIMENT: not started  /  PROVENANCE: required",muted,14);break;
 case 4:
  moduleCard(dc,{r.left+18,r.top+74,r.right-18,r.top+183},L"ENTITY / HYPOTHESIS REGISTER",L"0 VERIFIED ENTRIES",L"Source identity cannot be inferred from unexplained signals");
  text(dc,r.left+22,r.top+221,L"EVIDENCE TYPES  /  MEASURED | DERIVED | INTERPRETED | SYNTHESIZED",muted,13);break;
 case 5:
  text(dc,r.left+20,r.top+54,L"SPOON FORK  /  CELESTIAL + GEOGRAPHIC",ink,19,true);
  disk(dc,cx,cy,115,RGB(55,131,149));disk(dc,cx,cy,74,RGB(85,186,180));disk(dc,cx,cy,25,cyan);
  line(dc,cx-140,cy,cx+140,cy,RGB(52,98,117));
  text(dc,r.left+20,r.bottom-42,L"Ephemeris and astrocartography databases: not installed",muted,13);break;
 case 6:
  text(dc,r.left+20,r.top+57,L"CHRONOS  /  TEMPORAL RESEARCH",ink,19,true);
  for(int rad=50;rad<=135;rad+=28)disk(dc,cx,cy,rad,RGB(54,120,137));
  line(dc,cx,cy,cx+72,cy-70,cyan,3);line(dc,cx,cy,cx-20,cy-96,ink,2);
  text(dc,r.left+20,r.bottom-39,L"Future-self continuity: planning protocol; no time-message receiver",muted,13);break;
 case 7:
  text(dc,r.left+20,r.top+55,L"DRIFT ANCHOR / SAFETY CONTROLLER",ink,19,true);
  moduleCard(dc,{r.left+18,r.top+105,r.right-18,r.top+236},L"GLOBAL OPERATOR GATE",safety.engaged()?L"HARDSTOP ENGAGED":L"LOCAL SAFETY READY",L"Real hardware emission and acquisition not implemented");
  text(dc,r.left+20,r.bottom-42,L"Use HARDSTOP at any time; reset requires deliberate approval",muted,14);break;
 case 8:
  text(dc,r.left+20,r.top+55,L"GOLF-GUFF / LOCAL GENERATIVE INTELLIGENCE",ink,18,true);
  moduleCard(dc,{r.left+18,r.top+108,r.right-18,r.top+229},L"MODEL BRIDGE",L"NOT CONNECTED",L"Prompt assembly exists; inference is not simulated");
  text(dc,r.left+20,r.bottom-40,L"Repository registry: 21 discovered  |  source index: pending",muted,13);break;
 case 9:
  text(dc,r.left+20,r.top+54,L"RADAR / RESEARCH DISCOVERY + DATA FARM",ink,18,true);
  moduleCard(dc,{r.left+18,r.top+100,r.right-18,r.top+222},L"NETWORK CONNECTOR",L"OFFLINE / INACTIVE",L"No article content fetched in this build");
  text(dc,r.left+20,r.bottom-44,L"Future sources: research feeds, astronomy and academic publications",muted,13);break;
 case 10:
  text(dc,r.left+20,r.top+56,L"AUTOMATIONS / SYSTEM EVOLUTION",ink,18,true);
  moduleCard(dc,{r.left+18,r.top+107,r.right-18,r.top+221},L"JOB QUEUE",L"NO JOBS ACTIVE",L"Scheduling and protected repair approval pending");
  text(dc,r.left+20,r.bottom-45,L"Build-and-test CI is active on GitHub; no local automation daemon",muted,13);break;
 case 11:
  text(dc,r.left+20,r.top+52,L"SETTINGS / SYSTEM BOUNDARIES",ink,18,true);
  moduleCard(dc,{r.left+18,r.top+101,r.right-18,r.top+214},L"RUNTIME MODE",L"OFFLINE-FIRST",L"C++20 / Windows x64 / local control");
  text(dc,r.left+20,r.bottom-44,L"External transmitters: disabled  |  AI model: unavailable",muted,13);break;
 }
}
void paint(HDC dc,int w,int h){
 rect(dc,{0,0,w,h},RGB(7,18,28));
 rect(dc,{0,0,w,58},RGB(12,33,46));line(dc,0,57,w,57,RGB(35,86,97));
 text(dc,22,13,L"AETHER OS",RGB(196,255,246),27,true);
 text(dc,244,20,L"AN APP FOR THE FUTURE  /  SPIRAL OS",muted,15);
 text(dc,w-210,20,L"LOCAL MAINFRAME",cyan,14,true);
 rect(dc,{0,58,226,h},RGB(10,28,40));
 text(dc,20,79,L"WORKSPACES  /  12",RGB(91,159,174),13,true);
 for(int i=0;i<12;++i){
  RECT rr{12,110+i*36,214,143+i*36};if(rr.bottom>h-10)break;
  if(i==selected){rect(dc,rr,RGB(22,73,80));rect(dc,{12,rr.top,16,rr.bottom},cyan);}
  text(dc,26,rr.top+7,(i<9?L"0":L"")+std::to_wstring(i+1)+L"    "+tabs[i],i==selected?ink:muted,14,i==selected);
 }
 text(dc,250,82,tabs[selected],ink,29,true);
 text(dc,251,124,desc[selected],muted,14);
 RECT panel{245,165,w-23,std::max(330,h-192)};
 rect(dc,panel,RGB(9,32,44));frame(dc,panel,RGB(32,87,99));
 paintWorkspace(dc,panel);
 for(int i=0;i<6;++i)button(dc,buttonRect(i,w,h),controls[selected][i],i==2&&safety.engaged());
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
 case WM_SIZE:{RECT r{};GetClientRect(hwnd,&r);MoveWindow(input,357,r.bottom-77,std::max(120,static_cast<int>(r.right)-390),25,TRUE);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
 case WM_TIMER:if(animate){phase=(phase+1)%6;InvalidateRect(hwnd,nullptr,FALSE);}return 0;
 case WM_KEYDOWN:if(wp==VK_RETURN){wchar_t buf[512]{};GetWindowTextW(input,buf,512);SetWindowTextW(input,L"");command(buf);return 0;}break;
 case WM_COMMAND:if(LOWORD(wp)==kEdit && HIWORD(wp)==EN_CHANGE)return 0;break;
 case WM_LBUTTONDOWN:{
  RECT r{};GetClientRect(hwnd,&r);int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);
  if(x>=12&&x<224&&y>=110&&y<110+36*12){selected=std::clamp((y-110)/36,0,11);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
  for(int i=0;i<6;++i){auto b=buttonRect(i,r.right,r.bottom);
   if(x<b.left||x>b.right||y<b.top||y>b.bottom)continue;
   try{tabAction(i);}catch(...){setOutput(L"ERROR: operation failed safely");}
   InvalidateRect(hwnd,nullptr,FALSE);return 0;
  }
  break;
 }
 case WM_PAINT:{
  PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);RECT r{};GetClientRect(hwnd,&r);
  HDC mem=CreateCompatibleDC(dc);HBITMAP bitmap=CreateCompatibleBitmap(dc,std::max(1,static_cast<int>(r.right)),std::max(1,static_cast<int>(r.bottom)));
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
