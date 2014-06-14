/*-------------------------------------------
  utl.c - KAZUBON 1997-1999
  misc functions
---------------------------------------------*/
#include "tcdll.h"
extern HINSTANCE hInstance;

char g_bIniSetting = 0;
char g_inifile[MAX_PATH];

/*-------------------------------------------
	パス名にファイル名をつける
---------------------------------------------*/
void add_title(char* path, const char* title)
{
	char* p;
	
	p = path;
	
	if(*title && *(title + 1) == ':') ;
	else if(*title == '\\') {
		if(*p && *(p + 1) == ':') p += 2;
	} else {
		while(*p) {
			if((*p == '\\' || *p == '/') && *(p + 1) == 0) {
				break;
			}
			p = (char*)CharNext(p);
		}
		*p++ = '\\';
	}
	while(*title) *p++ = *title++;
	*p = 0;
}

/*-------------------------------------------
	パス名からファイル名をとりのぞく
---------------------------------------------*/
void del_title(char* path)
{
	char* p, *ep;
	
	p = ep = path;
	while(*p) {
		if(*p == '\\' || *p == '/') {
			if(p > path && *(p - 1) == ':') ep = p + 1;
			else ep = p;
		}
		p = (char*)CharNext(p);
	}
	*ep = 0;
}

/*------------------------------------------------
	カンマで区切られた文字列を取り出す
--------------------------------------------------*/
void parse(char* dst, const char* src, int n)
{
	char* dp;
	int i;
	
	for(i = 0; i < n; i++) {
		while(*src && *src != ',') src++;
		if(*src == ',') src++;
	}
	if(*src == 0) {
		*dst = 0; return;
	}
	
	while(*src == ' ') src++;
	
	dp = dst;
	while(*src && *src != ',') *dst++ = *src++;
	*dst = 0;
	
	while(dst != dp) {
		dst--;
		if(*dst == ' ') *dst = 0;
		else break;
	}
}

/*-------------------------------------------
  returns a resource string
---------------------------------------------*/
char* MyString(UINT id)
{
	static char buf[80];
	
	if(LoadString(hInstance, id, buf, 80) == 0)
		buf[0] = 0;
		
	return buf;
}

const char mykey[] = "Software\\Stoic Joker's\\T-Clock 2010";

/*-------------------------------------------
	レジストリに文字列を書き込む
---------------------------------------------*/
BOOL SetMyRegStr(const char* section, const char* entry, const char* val)
{
	HKEY hkey;
	BOOL r;
	char key[80];
	
	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);
	
	if(section && *section) {
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	} else {
		if(g_bIniSetting) strcpy(key, "Main");
	}
	
	if(g_bIniSetting) {
		r = FALSE;
		if(WritePrivateProfileString(key, entry, val, g_inifile))
			r = TRUE;
	} else {
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0) {
			if(RegSetValueEx(hkey, entry, 0, REG_SZ,
							 (CONST BYTE*)val, (INT)strlen(val)) == 0) {
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*------------------------------------------------
	自分のレジストリから文字列を得る
--------------------------------------------------*/
int GetMyRegStr(const char* section, const char* entry, char* val, int len, const char* defval)
{
	char key[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	int r=0;
	
	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);
	
	if(section && *section) {
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	} else {
		if(g_bIniSetting) strcpy(key, "Main");
	}
	
	if(g_bIniSetting) {
		r = GetPrivateProfileString(key,
									entry, defval,
									val,
									len,
									g_inifile);
	} else {
		BOOL b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0) {
			size = len;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
							   (LPBYTE)val, &size) == 0) {
				if(size == 0) *val = 0;
				r = size;
				b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE) {
			strcpy(val, defval);
			r = (INT)strlen(defval);
		}
	}
	
	return r;
}

int GetMyRegStrEx(const char* section, const char* entry, char* val, int len,
				  const char* defval)
{
	char key[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	int r=0;
	
	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);
	
	if(section && *section) {
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	} else {
		if(g_bIniSetting) strcpy(key, "Main");
	}
	
	if(g_bIniSetting) {
		r = GetPrivateProfileString(key, entry, defval, val,
									len, g_inifile);
		if(r == len)
			SetMyRegStr(section, entry, defval);
	} else {
		BOOL b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0) {
			size = len;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
							   (LPBYTE)val, &size) == 0) {
				if(size == 0) *val = 0;
				r = size;
				b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE) {
			SetMyRegStr(section, entry, defval);
			strcpy(val, defval);
			r = (INT)strlen(defval);
		}
	}
	
	return r;
}

/*-------------------------------------------
	レジストリにDWORD値を書き込む
---------------------------------------------*/
BOOL SetMyRegLong(const char* section, const char* entry, DWORD val)
{
	HKEY hkey;
	BOOL r;
	char key[80];
	
	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);
	
	if(section && *section) {
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	} else {
		if(g_bIniSetting) strcpy(key, "Main");
	}
	
	if(g_bIniSetting) {
		char s[20];
		wsprintf(s, "%d", val);
		r = FALSE;
		if(WritePrivateProfileString(key, entry, s, g_inifile))
			r = TRUE;
	} else {
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0) {
			if(RegSetValueEx(hkey,entry,0,REG_DWORD,(CONST BYTE*)&val,4)==ERROR_SUCCESS) {
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*------------------------------------------------
	自分のレジストリからLONG値を得る
--------------------------------------------------*/
LONG GetMyRegLong(const char* section, const char* entry, LONG defval)
{
	char key[80];
	HKEY hkey;
	
	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);
	
	if(section && *section) {
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	} else {
		if(g_bIniSetting) strcpy(key, "Main");
	}
	
	if(g_bIniSetting) {
		return GetPrivateProfileInt(key, entry, defval, g_inifile);
	} else {
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0) {
			DWORD regtype,size=sizeof(LONG);
			LONG dw=0;
			if(RegQueryValueEx(hkey,entry,0,&regtype,(LPBYTE)&dw,&size)==ERROR_SUCCESS && regtype==REG_DWORD)
				defval=dw;
			RegCloseKey(hkey);
		}
	}
	return defval;
}
LONG GetMyRegLongEx(const char* section, const char* entry, LONG defval)
{
	char key[80];
	HKEY hkey;
	
	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);
	
	if(section && *section) {
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	} else {
		if(g_bIniSetting) strcpy(key, "Main");
	}
	
	if(g_bIniSetting) {
		return GetPrivateProfileInt(key,entry,defval,g_inifile);
	} else {
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0) {
			DWORD regtype,size=sizeof(LONG);
			LONG dw=0;
			if(RegQueryValueEx(hkey,entry,0,&regtype,(LPBYTE)&dw,&size)==ERROR_SUCCESS && regtype==REG_DWORD){
				defval=dw;
			}else{
				SetMyRegLong(section,entry,defval);
			}
			RegCloseKey(hkey);
		}
	}
	return defval;
}

COLORREF GetMyRegColor(const char* section, const char* entry, COLORREF defval)
{
	char key[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	LONG r=0;
	
	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);
	
	if(section && *section) {
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	} else {
		if(g_bIniSetting) strcpy(key, "Main");
	}
	
	if(g_bIniSetting) {
		r = GetPrivateProfileInt(key, entry, defval, g_inifile);
	} else {
		BOOL b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0) {
			size = 4;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
							   (LPBYTE)&r, &size) == 0) {
				if(size == 4) b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE) r = defval;
	}
	if(r & 0x80000000) r = GetSysColor(r & 0x00ffffff);
	
	return r;
}

/*------------------------------------------------
  get DWORD value from registry
--------------------------------------------------*/
/*
LONG GetRegLong(HKEY rootkey, char* subkey, char* entry, LONG defval)
{
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r=0;
	
	b = FALSE;
	if(RegOpenKey(rootkey, subkey, &hkey) == 0) {
		size = 4;
		regtype = REG_DWORD;
		if(RegQueryValueEx(hkey, entry, 0, &regtype,
						   (LPBYTE)&r, &size) == 0) {
			if(size == 4) b = TRUE;
		}
		RegCloseKey(hkey);
	}
	if(b == FALSE) r = defval;
	return r;
}// */

/*------------------------------------------------
	レジストリから文字列を得る
--------------------------------------------------*/
int GetRegStr(HKEY rootkey, const char* subkey, const char* entry,
			  char* val, int len, const char* defval)
{
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r=0;
	
	b = FALSE;
	if(RegOpenKey(rootkey, subkey, &hkey) == 0) {
		size = len;
		if(RegQueryValueEx(hkey, entry, 0, &regtype,
						   (LPBYTE)val, &size) == 0) {
			if(size == 0) *val = 0;
			b = TRUE;
		}
		RegCloseKey(hkey);
	}
	if(b == FALSE) {
		strcpy(val, defval);
		r = (INT)strlen(defval);
	}
	return r;
}

// tile an image vertically
void VerticalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest,
					 HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc,
					 BOOL ReverseBlt, BOOL useTrans)
{
	int y;
	
	if(ReverseBlt) {
		for(y = cyDest - cySrc; y > yDest - cySrc; y -= cySrc) {
			TC2DrawBlt(hdcDest,
					   xDest,
					   y,
					   cxDest,
					   cySrc,
					   hdcSrc,
					   xSrc,
					   ySrc,
					   cxSrc,
					   cySrc,
					   useTrans);
		}
	} else {
		for(y = 0; y < cyDest; y += cySrc) {
			TC2DrawBlt(hdcDest,
					   xDest,
					   yDest + y,
					   cxDest,
					   __min(cyDest - y, cySrc),
					   hdcSrc,
					   xSrc,
					   ySrc,
					   cxSrc,
					   __min(cyDest - y, cySrc),
					   useTrans);
		}
	}
}

// tile an image horizontally and vertically
void FillTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp)
{
	int x, y;
	
	for(y = 0; y < cyDest; y += cySrc) {
		for(x = 0; x < cxDest; x += cxSrc) {
			BitBlt(hdcDest,
				   xDest + x,
				   yDest + y,
				   cxSrc,
				   cySrc,
				   hdcSrc,
				   xSrc,
				   ySrc,
				   rasterOp);
		}
	}
}

void TileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc,
			 int xSrc, int ySrc, int cxSrc, int cySrc, BOOL useTrans)
{
	int y, x;
	
	for(y = yDest; y < cyDest; y = y + cySrc) {
		for(x = xDest; x < cxDest; x = x + cxSrc) {
			TC2DrawBlt(hdcDest, x, y, cxSrc, cySrc,
					   hdcSrc, xSrc, ySrc, cxSrc, cySrc, useTrans);
		}
	}
}

/*--------------------------------------------------------
  Retreive a file name and option from a command string
----------------------------------------------------------*/
void GetFileAndOption(const char* command, char* fname, char* opt)
{
	const char* p, *pe;
	char* pd;
	WIN32_FIND_DATA fd;
	HANDLE hfind;
	
	p = command; pd = fname;
	pe = NULL;
	for(; ;) {
		if(*p == ' ' || *p == 0) {
			*pd = 0;
			hfind = FindFirstFile(fname, &fd);
			if(hfind != INVALID_HANDLE_VALUE) {
				FindClose(hfind);
				pe = p;
			}
			if(*p == 0) break;
		}
		*pd++ = *p++;
	}
	if(pe == NULL) pe = p;
	
	p = command; pd = fname;
	for(; p != pe;) {
		*pd++ = *p++;
	}
	*pd = 0;
	if(*p == ' ') p++;
	
	pd = opt;
	for(; *p;) *pd++ = *p++;
	*pd = 0;
}

/*------------------------------------------------
  Open a file
--------------------------------------------------*/
BOOL ExecFile(HWND hwnd, const char* command)
{
	char fname[MAX_PATH], opt[MAX_PATH];
	if(*command){
		GetFileAndOption(command,fname,opt);
		if((intptr_t)ShellExecute(hwnd,NULL,fname,*opt?opt:NULL,NULL,SW_SHOW)>32)
			return TRUE;
	}
	return FALSE;
}


BOOL IsXPStyle()
{
	char temp[1024];
	
	GetRegStr(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", "ThemeActive", temp, 1024, "0");
	if(_strnicmp(temp, "1", 1) == 0) return TRUE;
	else return FALSE;
}
/*
void Pause(HWND hWnd, LPCTSTR pszArgs)
{
	LONG lInterval = atoi(pszArgs);
	LONG lTime = GetTickCount();
	MSG msg;
	
	if(lInterval > 0) {
		while((LONG)(GetTickCount() - lTime) < lInterval) {
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}// */