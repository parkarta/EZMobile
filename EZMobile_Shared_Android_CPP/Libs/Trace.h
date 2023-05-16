#pragma once
#ifndef	__Trace_h__
#define __Trace_h__

#undef TRACE
#undef ATLTRACE
#undef ATLTRACE2
#undef TRACE0
#undef TRACE1
#undef TRACE2
#undef TRACE3


#ifdef _DEBUG

static inline void _Trace(const char * frmt, ...) noexcept {
char	buffer[512];
	va_list args;
	va_start(args, frmt);
	_vsnprintf_s(buffer, sizeof(buffer) / sizeof(char), frmt, args);
	va_end(args);
	::OutputDebugStringA(buffer);
}

  #define TRACE						_Trace
  #define TRACE_BYTE(b)				TRACE(" %02X", b)
  #define TRACEB(b)					TRACE(b)
  #define FOR_DEBUG(exp)			exp
  #define TRACE0(sz)				TRACE("%s", sz)
  #define TRACE1(sz, p1)			TRACE(sz, p1)
  #define TRACE2(sz, p1, p2)		TRACE(sz, p1, p2)
  #define TRACE3(sz, p1, p2, p3)	TRACE(sz, p1, p2, p3)

#else
static inline void _Trace() noexcept {}
  #define FOR_DEBUG(...)
  #define TRACE(...)
  #define TRACE0(...)
  #define TRACE1(...)
  #define TRACE2(...)
  #define TRACE3(...)
  #define TRACE_BYTE(b)
  #define TRACEB(b)
#endif

#endif
