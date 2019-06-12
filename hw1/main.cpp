#include<Windows.h>
#include<iostream>
#include<string>
#include<chrono>
#include<fstream>


struct stopWatch {
	int hour, minute, second, microsecond;
}sw;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void UpdateTime(HWND hTxt);
void ResetTime();
//程序入口
int WINAPI WinMain(
	HINSTANCE hInstance,//应用当前实例句柄
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,  //应用程序的命令行 
	int       nShowCmd) {
	std::string appName = "stop watch";
	//CCHAR szAppName[] = TEXT(appName);
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;//

	wndclass.style = CS_HREDRAW | CS_VREDRAW; //水平或竖直变化后重绘
	wndclass.lpfnWndProc = WndProc; //消息处理程序地址
	wndclass.cbClsExtra = 0; //预留空间
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //图标
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);//光标
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //窗口视图背景颜色
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = appName.c_str();

	//注册窗口类
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("ERROR! register wndclass error!"), appName.c_str(), MB_ICONERROR);
		return 0;
	}
	//创建窗口
	/* 创建窗口 1.窗口类名 2.窗口标题 3.窗口风格 4-7x,y初始位置与大小
	8.父窗口句柄 9，窗口菜单句柄 10，程序实例句柄 11.创建参数
	*/
	hwnd = CreateWindow(appName.c_str(), TEXT("The stop watch app"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 330, 280,
		NULL, NULL, hInstance, NULL);

	//显示和更新窗口
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	//获取消息
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	//
	HDC hdc; //设备上下文句柄  用于绘图
	HDC hMemDc;
	int i;
	PAINTSTRUCT ps;
	HPEN hpen;
	HBRUSH hbrush;
	LPDRAWITEMSTRUCT pdis;
	//button
	static HWND hBtnStart, hBtnReset;
	//static text
	static HWND hTxt;
	int wmId, wmEvent;
	//time
	TCHAR current_time[1024];
	//button text
	std::string start_pause = "start";

	switch (message) {
	case WM_CREATE: {
		hBtnReset = CreateWindow(TEXT("button"), TEXT("reset"), WS_CHILD | WS_VISIBLE
			| BS_PUSHBUTTON | BS_OWNERDRAW, 30, 120, 100, 100, hwnd, (HMENU)1, NULL, NULL);
		hBtnStart = CreateWindow(TEXT("button"), start_pause.c_str(), WS_CHILD | WS_VISIBLE
			| BS_OWNERDRAW, 160, 120, 100, 100, hwnd, (HMENU)2, NULL, NULL);
		wsprintf(current_time, "%d:%d:%d:%d", sw.hour, sw.minute, sw.second, sw.microsecond);
		hTxt = CreateWindow(TEXT("static"), current_time, WS_CHILD | WS_VISIBLE | BS_TEXT | BS_CENTER,
			30, 40, 230, 40, hwnd, (HMENU)3, NULL, NULL);
		break;
	}

	case WM_COMMAND: {

		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case 1: {
			//	SendMessage(hwnd, BM_GETSTATE, BN_CLICKED, 0);
				//SendMessage(hwnd, BM_SETSTATE,1 , 0);//设置按键为按下
				//SendMessage(hwnd, BM_SETSTATE, 0, 0);//设置按键恢复到正常状态
				//reset stop watch
			KillTimer(hwnd, 1);
			TCHAR text_buffer[1024];
			GetWindowText(hBtnStart, text_buffer, 1024);
			start_pause = std::string(text_buffer);
			if (start_pause == "pause") {
				SetWindowText(hBtnStart, TEXT("start"));
			}
			ResetTime();
			wsprintf(current_time, "%d:%d:%d:%d", sw.hour, sw.minute, sw.second, sw.microsecond);
			SetWindowText(hTxt, current_time);
			break;
		}
		case 2: {
			TCHAR text_buffer[1024];
			GetWindowText(hBtnStart, text_buffer, 1024);
			start_pause = std::string(text_buffer);

			if (start_pause == "start") {
				SetWindowText(hBtnStart, TEXT("pause"));
				//启动计时器 10ms产生一个WM_TIMER message
				SetTimer(hwnd, 1, 10, NULL);

			}
			else {
				SetWindowText(hBtnStart, TEXT("start"));
				wsprintf(current_time, "%d:%d:%d:%d", sw.hour, sw.minute, sw.second, sw.microsecond);
				SetWindowText(hTxt, current_time);
				KillTimer(hwnd, 1);
			}
			break;
		}
		default:
			break;
		}
		break;
	}

					 /*case BM_SETSTATE:
					 {
						 MessageBox(hwnd, TEXT("clicked"), TEXT("click"), 0);
						 MoveWindow(hwnd, 20, 20, 100, 100, TRUE);
						 break;
					 }*/
	case WM_TIMER: {
		UpdateTime(hTxt);
		break;
	}
	case WM_DRAWITEM: {
		pdis = (LPDRAWITEMSTRUCT)lParam;
		//重绘bkg
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));

		if (pdis->CtlID == 1) {
			if (pdis->itemState) {
				hpen = CreatePen(PS_SOLID, 2, RGB(100, 100, 255));
				hbrush = CreateSolidBrush(RGB(180, 180, 255));
				SetBkColor(pdis->hDC, RGB(180, 180, 255));
			}
			else {
				//创建紫色笔和笔刷
				hpen = CreatePen(PS_SOLID, 2, RGB(150, 150, 255));
				hbrush = CreateSolidBrush(RGB(220, 220, 255));
				SetBkColor(pdis->hDC, RGB(220, 220, 255));
			}


			//指定笔刷

			SelectObject(pdis->hDC, hpen);
			SelectObject(pdis->hDC, hbrush);

			RoundRect(pdis->hDC, 0, 0, 100, 100, 100, 100);
			//设置文本背景颜色和文本颜色

			SetTextColor(pdis->hDC, RGB(0, 0, 50));
			TCHAR text_buffer[1024];
			GetWindowText(pdis->hwndItem, text_buffer, 1024);
			TextOut(pdis->hDC, 30, 40, text_buffer, strlen(text_buffer));

			DeleteObject(hpen);
			DeleteObject(hbrush);

		}
		if (pdis->CtlID == 2) {
			TCHAR text_buffer[1024];
			GetWindowText(pdis->hwndItem, text_buffer, 1024);
			start_pause = std::string(text_buffer);
			if (start_pause == "start") {//RoundRect(pdis->hDC, 10, 100, 100, 200, 40, 40);
				hpen = CreatePen(PS_SOLID, 2, RGB(150, 255, 150));
				hbrush = CreateSolidBrush(RGB(220, 255, 220));

				SetBkColor(pdis->hDC, RGB(220, 255, 220));
				SetTextColor(pdis->hDC, RGB(0, 50, 0));

			}
			else {
				hpen = CreatePen(PS_SOLID, 2, RGB(255, 150, 150));
				hbrush = CreateSolidBrush(RGB(255, 220, 220));

				SetBkColor(pdis->hDC, RGB(255, 220, 220));
				SetTextColor(pdis->hDC, RGB(50, 0, 0));

			}
			SelectObject(pdis->hDC, hpen);
			SelectObject(pdis->hDC, hbrush);
			RoundRect(pdis->hDC, 0, 0, 100, 100, 100, 100);
			TextOut(pdis->hDC, 30, 40, text_buffer, strlen(text_buffer));

			DeleteObject(hpen);
			DeleteObject(hbrush);
		}
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam); //处理WndProc程序不处理的所有消息
}

void UpdateTime(HWND hTxt) {
	sw.microsecond++;
	if (sw.microsecond == 100) {
		sw.microsecond = 0;
		sw.second++;
	}
	if (sw.second == 60) {
		sw.second = 0;
		sw.minute++;
	}if (sw.minute == 60) {
		sw.minute = 0;
		sw.hour++;
	}if (sw.hour == 24) {
		sw.hour = 0;
	}
	TCHAR current_time[1024];
	wsprintf(current_time, "%d:%d:%d:%d", sw.hour, sw.minute, sw.second, sw.microsecond);
	SetWindowText(hTxt, current_time);
}

void ResetTime() {
	sw.hour = 0;
	sw.minute = 0;
	sw.second = 0;
	sw.microsecond = 0;
}
