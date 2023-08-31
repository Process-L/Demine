#include <iostream>
#include <Windows.h>

BYTE width,height;
BYTE mines[720]={};
bool need_update=false;
HWND hwnd,button3,button4;

BYTE scan(unsigned char op,BYTE w=0,BYTE h=0){
	DWORD Pid;
	HANDLE hProcess = 0;
	DWORD Address_width = 0x01005334;
	DWORD Address_height = 0x01005338;
	DWORD Address_mine = 0x01005361;
	DWORD Address_minesn = 0x01005194;
	DWORD Address_mines = 0x01005330;
	DWORD Address_time = 0x0100579C;
	HWND hWnd = FindWindow(NULL,L"É¨À×");
	if (hWnd == 0){
		return 0;
	}
	GetWindowThreadProcessId(hWnd, &Pid);
	hProcess = OpenProcess(0x1F0FFF, FALSE, Pid);
	if (hProcess == 0){
		return 0;
	}
	SIZE_T dwNumberOfBytesRead;
	BYTE result;
	switch (op){
		case 0:
			ReadProcessMemory(hProcess,(LPCVOID)Address_width,&result,1,&dwNumberOfBytesRead);
			break;
		case 1:
			ReadProcessMemory(hProcess,(LPCVOID)Address_height,&result,1,&dwNumberOfBytesRead);
			break;
		default:
			for (int y=0;y<h;y++){
				for (int x=0;x<w;x++){
					ReadProcessMemory(hProcess,(LPCVOID)Address_mine,&result,1,&dwNumberOfBytesRead);
					Address_mine+=1;
					if ((result==0x8A)|(result==0x8D)|(result==0x8E)|(result==0x8F)|(result==0xCC)){
						mines[y*32+x]=1;
					}
					else{
						mines[y*32+x]=0;
					}
				}
				Address_mine+=(32-w);
			}
			result=-1;
			break;
	}
	return result;
}

void autom(BYTE w,BYTE h){
	HANDLE hProcess = 0;
	HWND hWnd = FindWindow(NULL,L"É¨À×");
	if (hWnd==0){
		return ;
	}
	DWORD Pid;
	GetWindowThreadProcessId(hWnd, &Pid);
	hProcess = OpenProcess(0x1F0FFF, FALSE, Pid);
	DWORD Address_mine = 0x01005361;
	SIZE_T dwNumberOfBytesRead;
	BYTE result;
	for (int y=0;y<h;y++){
		for (int x=0;x<w;x++){
			ReadProcessMemory(hProcess,(LPCVOID)Address_mine,&result,1,&dwNumberOfBytesRead);
			Address_mine+=1;
			if (!((result==0x8A)|(result==0x8D)|(result==0x8E)|(result==0x8F)|(result==0xCC))){
				SendMessage(hWnd,WM_LBUTTONDOWN,0,(0x00400010+16*x+((16*y)<<16)));
				SendMessage(hWnd,WM_LBUTTONUP,0,(0x00400010+16*x+((16*y)<<16)));
			}
		}
		Address_mine+=(32-w);
	}
}

bool pausetime(bool op){
	HANDLE hProcess = 0;
	HWND hWnd = FindWindow(NULL,L"É¨À×");
	if (hWnd==0){
		return false;
	}
	DWORD Pid;
	GetWindowThreadProcessId(hWnd, &Pid);
	hProcess = OpenProcess(0x1F0FFF, FALSE, Pid);
	DWORD Address_inc=0x01002ff5;
	DWORD Address_incs=0x01003830;
	BYTE wb[6]={0x90,0x90,0x90,0x90,0x90,0x90};
	BYTE wb_[6]={0xff,0x05,0x9c,0x57,0x00,0x01};
	DWORD i=0;
	switch (op){
		case false:
			WriteProcessMemory(hProcess,(LPVOID)Address_inc, wb_, 6, &i);
			WriteProcessMemory(hProcess,(LPVOID)Address_incs, wb_, 6, &i);
			break;
		case true:
			WriteProcessMemory(hProcess,(LPVOID)Address_inc, wb, 6, &i);
			WriteProcessMemory(hProcess,(LPVOID)Address_incs, wb, 6, &i);
			break;
	}
	return true;
}

void attach(bool op){
	if (op){
		HWND hWnd = FindWindow(NULL,L"É¨À×");
		if (hWnd==0){
			return ;
		}
		RECT rt;
		GetWindowRect(hWnd,&rt);
		LONG FormStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
		SetWindowLong(hwnd, GWL_EXSTYLE, FormStyle|WS_EX_LAYERED);
		SetLayeredWindowAttributes(hwnd, 0, 96, LWA_ALPHA);
		MoveWindow(hwnd,rt.left+7,rt.top+70,496,461,TRUE);
	}
	else{
		LONG FormStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
		SetWindowLong(hwnd, GWL_EXSTYLE, FormStyle^WS_EX_LAYERED);
		SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
	}
}

void post(UINT msg,WPARAM wParam, LPARAM lParam){
	if (SendMessage(button4, BM_GETCHECK, 0, 0)==BST_CHECKED){
		HWND hWnd = FindWindow(NULL,L"É¨À×");
		if (hWnd==0){
			return ;
		}
		SendMessage(hWnd,msg,wParam,lParam+7+(49<<16));
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
   {
       case WM_CLOSE:
           DestroyWindow(hwnd);
           break;
       case WM_DESTROY:
           PostQuitMessage(0);
           break;
       case WM_PAINT:
           {
               PAINTSTRUCT ps;
               HDC hdc = BeginPaint(hwnd, &ps);
               
               RECT rect;
               GetClientRect(hwnd, &rect);
			   SetDCBrushColor(hdc, RGB(240,240,240));
               FillRect(hdc, &rect,(HBRUSH)GetStockObject(DC_BRUSH));
               
			    SetDCBrushColor(hdc, RGB(196,196,196));
			    SelectObject(ps.hdc, GetStockObject(DC_BRUSH));
				Rectangle(hdc,5,5,485,389);
				if (need_update){
					for (int y=0;y<height;y++){
						for (int x=0;x<width;x++){
							if (mines[y*32+x]==0){
								SetDCBrushColor(hdc, RGB(240,240,240));
								SelectObject(ps.hdc, GetStockObject(DC_BRUSH));
							}
							else{
								SetDCBrushColor(hdc, RGB(255,0,0));
								SelectObject(ps.hdc, GetStockObject(DC_BRUSH));
							}
						Rectangle(hdc,5+16*x,5+16*y,21+16*x,21+16*y);
						}
					}
				}

				EndPaint(hwnd, &ps);
           }
           break;
       case WM_COMMAND:
           {
               int wmId = LOWORD(wParam);
               int wmEvent = HIWORD(wParam);
               if (wmEvent == BN_CLICKED)
               {
				   switch(wmId){
						case 1:
						   width=scan(0);
						   height=scan(1);
						   if (width*height!=0){
							   scan(3,width,height);
								InvalidateRgn(hwnd,NULL,FALSE);
								need_update=true;
						   }
						   else{
								need_update=false;
								SendMessage(button3, BM_SETCHECK, BST_UNCHECKED, 0);
								SendMessage(button4, BM_SETCHECK, BST_UNCHECKED, 0);
								attach(false);
						   }
						   break;
					   case 2:
						   width=scan(0);
						   height=scan(1);
						   if (width*height!=0){
							   scan(3,width,height);
							   InvalidateRgn(hwnd,NULL,FALSE);
							   need_update=true;
							   UpdateWindow(hwnd);
							   autom(width,height);
							   break;
						   }
						   else{
								need_update=false;
								SendMessage(button3, BM_SETCHECK, BST_UNCHECKED, 0);
						   }
						   break;
					   case 3:
						   if (SendMessage(button3, BM_GETCHECK, 0, 0)==BST_CHECKED){
							   if (!pausetime(true)){
									SendMessage(button3, BM_SETCHECK, BST_UNCHECKED, 0);
							   }
						   }
						   else{
								pausetime(false);
						   }
						   break;
					   case 4:
						   if (SendMessage(button4, BM_GETCHECK, 0, 0)==BST_CHECKED){
							   attach(true);
						   }
						   else{
								attach(false);
						   }
				   }
               }
           }
           break;
	   case WM_LBUTTONDOWN:
			post(msg,wParam,lParam);
			break;
		case WM_LBUTTONUP:
			post(msg,wParam,lParam);
			break;
		case WM_RBUTTONDOWN:
			post(msg,wParam,lParam);
			break;
		case WM_RBUTTONUP:
			post(msg,wParam,lParam);
			break;
		case WM_MBUTTONDOWN:
			post(msg,wParam,lParam);
			break;
		case WM_MBUTTONUP:
			post(msg,wParam,lParam);
			break;
		case WM_MOUSEMOVE:
			post(msg,wParam,lParam);
			break;
	   default:
           return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   const wchar_t CLASS_NAME[] = L"MyWindowClass";

   WNDCLASS wc = {};
   wc.lpfnWndProc = WndProc;
   wc.hInstance = hInstance;
   wc.lpszClassName = CLASS_NAME;
   wc.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);
   
   RegisterClass(&wc);
   
   hwnd = CreateWindowEx(
       0,
       CLASS_NAME,
       L"Demine",
       WS_TILED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
       CW_USEDEFAULT,
       CW_USEDEFAULT,
       496,
       461,
       NULL,
       0,
       hInstance,
       NULL
   );
   
   if (hwnd == NULL)
   {
       return 0;
   }

   HWND button1 = CreateWindow(
       L"BUTTON",
       L"É¨Ãè",
       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
       5,
       394,
       160,
       30,
       hwnd,
       (HMENU)1,
       hInstance,
       NULL
   );
   
   HWND button2 = CreateWindow(
       L"BUTTON",
       L"×Ô¶¯É¨À×",
       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
       175,
       394,
       160,
       30,
       hwnd,
       (HMENU)2,
       hInstance,
       NULL
   );

   button3 = CreateWindow(
       L"BUTTON",
       L"Ê±¼äÔÝÍ£",
       WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
       405,
       394,
       80,
       30,
       hwnd,
       (HMENU)3,
       hInstance,
       NULL
   );

   button4 = CreateWindow(
       L"BUTTON",
       L"Îü¸½",
       WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
       345,
       394,
       50,
       30,
       hwnd,
       (HMENU)4,
       hInstance,
       NULL
   );

   HWND area1 = CreateWindow(
       L"STATIC",
       L"",
       WS_CHILD | BS_AUTOCHECKBOX, 
       5,
       5,
       480,
       384,
       hwnd,
       (HMENU)5,
       hInstance,
       NULL
   );

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   MSG msg = {};
   while (GetMessage(&msg, NULL, 0, 0))
   {
       TranslateMessage(&msg);
       DispatchMessage(&msg);
   }
   
   return msg.wParam;
}