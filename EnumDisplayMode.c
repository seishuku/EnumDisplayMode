#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"

HWND hWnd=NULL;
HWND hList=NULL;

char string[256];
DEVMODE dm;
LV_COLUMN Column;
LV_ITEM Item;
int i, j, numvidmodes;

typedef struct vidmode_s
{
	char description[255];
	int mode, width, height, bpp;
} vidmode_t;

vidmode_t vidmodes[64], temp;

BOOL CALLBACK DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			ShowWindow(hWndDlg, SW_SHOW);
			SetFocus(hWndDlg);

			hList=GetDlgItem(hWndDlg, IDC_DISPLAYMODES);

			SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)0, (LPARAM)LVS_EX_FULLROWSELECT);

			memset(&Column, 0, sizeof(LV_COLUMN));
			Column.mask=LVCF_TEXT|LVCF_SUBITEM;

			Column.pszText="Mode";
			SendMessage(hList, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&Column);

			i=0;
			j=0;
			while(EnumDisplaySettings(NULL, i++, &dm))
			{
				if(ChangeDisplaySettings(&dm, CDS_TEST)==DISP_CHANGE_SUCCESSFUL)
				{
					vidmodes[j].mode=i;
					vidmodes[j].width=dm.dmPelsWidth;
					vidmodes[j].height=dm.dmPelsHeight;
					vidmodes[j].bpp=dm.dmBitsPerPel;
					sprintf(vidmodes[j].description, "%d x %d @ %dbit", vidmodes[j].width, vidmodes[j].height, vidmodes[j].bpp);
					j++;
				}
			}

			numvidmodes=j;

			for(i=0;i<numvidmodes-1;i++)
			{
				for(j=(i+1);j<numvidmodes;j++)
				{
					if(vidmodes[j].bpp<vidmodes[i].bpp)
					{
						memcpy(&temp, &vidmodes[i], sizeof(vidmode_t));
						memcpy(&vidmodes[i], &vidmodes[j], sizeof(vidmode_t));
						memcpy(&vidmodes[j], &temp, sizeof(vidmode_t));
					}
				}

				for(j=(i+1);j<numvidmodes;j++)
				{
					if(vidmodes[j].width<vidmodes[i].width)
					{
						memcpy(&temp, &vidmodes[i], sizeof(vidmode_t));
						memcpy(&vidmodes[i], &vidmodes[j], sizeof(vidmode_t));
						memcpy(&vidmodes[j], &temp, sizeof(vidmode_t));
					}
				}
			}

			for(i=0;i<numvidmodes;i++)
			{
				memset(&Item, 0, sizeof(LV_ITEM));
				Item.mask=LVIF_TEXT;
				Item.cchTextMax=256;
				Item.iItem=i-1;

				Item.iSubItem=0;
				Item.pszText=vidmodes[i].description;
				SendMessage(hList, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&Item);
			}

			i=0;
			while(SendMessage(hList, LVM_SETCOLUMNWIDTH, (WPARAM)i++, (LPARAM)LVSCW_AUTOSIZE_USEHEADER));

			break;

		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_REFRESH:
					while(SendMessage(hList, LVM_DELETEITEM, (WPARAM)0, (LPARAM)0));

					i=0;
					while(EnumDisplaySettings(NULL, i++, &dm))
					{
						if(ChangeDisplaySettings(&dm, CDS_TEST)==DISP_CHANGE_SUCCESSFUL)
						{
							memset(&Item, 0, sizeof(LV_ITEM));
							Item.mask=LVIF_TEXT;
							Item.cchTextMax=256;
							Item.iItem=i-1;

							Item.iSubItem=0;
							sprintf(string, "%d", dm.dmPelsWidth);
							Item.pszText=string;
							SendMessage(hList, LVM_INSERTITEM, (WPARAM)0, (LPARAM)&Item);

							Item.iSubItem=1;
							sprintf(string, "%d", dm.dmPelsHeight);
							Item.pszText=string;
							SendMessage(hList, LVM_SETITEM, (WPARAM)0, (LPARAM)&Item);

							Item.iSubItem=2;
							sprintf(string, "%d", dm.dmBitsPerPel);
							Item.pszText=string;
							SendMessage(hList, LVM_SETITEM, (WPARAM)0, (LPARAM)&Item);

							Item.iSubItem=3;
							sprintf(string, "%d", dm.dmDisplayFrequency);
							Item.pszText=string;
							SendMessage(hList, LVM_SETITEM, (WPARAM)0, (LPARAM)&Item);
						}
					}

					i=0;
					while(SendMessage(hList, LVM_SETCOLUMNWIDTH, (WPARAM)i++, (LPARAM)LVSCW_AUTOSIZE_USEHEADER));
					break;

				case IDC_CHANGEMODE:
					i=SendMessage(hList, LVM_GETSELECTIONMARK, (WPARAM)0, (LPARAM)0);

					if(i==-1)
						break;

					EnumDisplaySettings(NULL, i, &dm);

					if(ChangeDisplaySettings(&dm, 0)!=DISP_CHANGE_SUCCESSFUL)
						MessageBox(hWndDlg, "ChangeDisplaySettings failed!", "EnumDisplayMode", MB_OK);
					break;

				case IDC_EXIT:
					PostQuitMessage(0);
					break;
			}
			break;

		case WM_DESTROY:
			break;

		case WM_CLOSE:
			DestroyWindow(hWndDlg);
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	InitCommonControls();

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)DialogProc, 0);

	return 0;
}
