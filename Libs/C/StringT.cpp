/*
* StringT.cpp
*/

#include <Windows.h>
#include <WinUser.h>
#include "StringT.h"
#pragma warning(disable:4996)

#ifdef LOCATION_INFO
int __WcharToUTF(const wchar_t * wsrc, char * dest, int size, int line, const char * file) {
	int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wsrc, -1, dest, 0, 0, 0);
	if (sizeRequired > size) {
		CStringA s;
		s.Format("WCharToUTF in %s at %d: destination buffer is too small for %S", file, line, wsrc);
		MessageBoxA(0, s, "", MB_ICONERROR);
		sizeRequired = size;
	}
	return WideCharToMultiByte(CP_UTF8, 0, wsrc, sizeRequired, dest, sizeRequired, 0, 0);
}	// int WcharToUTF(const wchar_t * wsrc, char * dest, int size)

int __UTFToWchar(const char * src, wchar_t *dest, int size, int line, const char * file) {
	int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, 0);
	if (sizeRequired > size) {
		CStringA s;
		s.Format("UTFtoWchar in %s at %d: destination buffer is too small for %s", file, line, src);
		MessageBoxA(0, s, "", MB_ICONERROR);
		sizeRequired = size;
	}
	return MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, sizeRequired);
}	// int UTFtoWchar(const char * src, wchar_t *dest, int size)

char * __xstrncpy(char * dest, unsigned int size, const char * src, int line, const char * file) {
size_t srcLength = strlen(src);
	if (srcLength >= size) {
		CStringA s;
		s.Format("xstrncpy in %s at %d: destination buffer is too small for %s", file, line, src);
		MessageBoxA(0, s, "", MB_ICONERROR);
		char * p = strncpy(dest, src, size - 1);
		dest[size] = '\0';
		return p;
	} else
		return strcpy(dest, src);
}	// char * __xstrncpy(char * dest, int size, const char * src, int line, const char * file)

wchar_t * __xstrncpy(wchar_t * dest, unsigned int size, const wchar_t * src, int line, const char * file) {
size_t srcLength = _tcslen(src);
	if (srcLength >= size) {
		CString s;
		s.Format(_T("xstrncpy in %s at %d: destination buffer is too small for %S"), file, line, src);
		MessageBox(0, s, _T(""), MB_ICONERROR);
		wchar_t * p = wcsncpy(dest, src, size - 1);
		dest[size] = '\0';
		return p;
	} else
		return wcscpy(dest, src);
}	// char * __xstrncpy(char * dest, int size, const char * src, int line, const char * file)

#else
int __WcharToUTF(const wchar_t * wsrc, char * dest, int size) {
	int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wsrc, -1, dest, 0, 0, 0);
	if (sizeRequired > size) {
		MessageBox(0, _T("WcharToUTF: destination buffer is too small"), _T(""), MB_ICONERROR);
		sizeRequired = size;
	}
	return WideCharToMultiByte(CP_UTF8, 0, wsrc, sizeRequired, dest, sizeRequired, 0, 0);
}	// int WcharToUTF(const wchar_t * wsrc, char * dest, int size)

int __UTFToWchar(const char * src, wchar_t *dest, int size) {
	int length = static_cast<int>(strlen(src)); 
	int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, src, length + 1, dest, 0);
	if (sizeRequired > size) {
		MessageBox(0, _T("UTFtoWchar: destination buffer is too small"), _T(""), MB_ICONERROR);
		sizeRequired = size;
	}
	return MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, sizeRequired);
}	// int UTFtoWchar(const char * src, wchar_t *dest, int size)

char * __xstrncpy(char * dest, unsigned int size, const char * src) {
int srcLength = strlen(src);
	if (srcLength >= size) {
		CStringA s;
		s.Format("xstrncpy: destination buffer is too small for %s", src);
		MessageBoxA(0, s, "", MB_ICONERROR);
		char * p = strncpy(dest, src, size - 1);
		dest[size] = '\0';
		return p;
	} else
		return strcpy(dest, src);
}	// char * __xstrncpy(char * dest, int size, const char * src)

wchar_t * __xstrncpy(wchar_t * dest, unsigned int size, const wchar_t * src) {
int srcLength = _tcslen(src);
	if (srcLength >= size) {
		CStringA s;
		s.Format("xstrncpy: destination buffer is too small for %s", src);
		MessageBoxA(0, s, "", MB_ICONERROR);
		wchar_t * p = wcsncpy(dest, src, size - 1);
		dest[size] = '\0';
		return p;
	} else
		return wcscpy(dest, src);
}	// char * __xstrncpy(char * dest, int size, const char * src, int line)


#endif	// LOCATION_INFO
