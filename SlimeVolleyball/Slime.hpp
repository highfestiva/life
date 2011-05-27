#pragma once
#include "../Lepra/Include/Graphics2D.h"
#include "../Lepra/Include/Log.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/StringUtility.h"
#include "../Lepra/Include/Timer.h"



#define CLEAR_MEMBERS(first, last)		\
{						\
	char* lStart = (char*)&first;		\
	char* lEnd = (char*)&last;		\
	lEnd += sizeof(last);			\
	::memset(lStart, 0, lEnd-lStart);	\
}

#define null 0



namespace Slime
{



using namespace Lepra;


class FontMetrics
{
public:
	int stringWidth(str s) { return s.length()*7; }
	int getHeight() { return 15; }
	int getAscent() { return 4; }
};

class Graphics
{
public:
	int width;
	int height;

	void drawString(str s, int x, int y) {s;x;y;}
	FontMetrics getFontMetrics() { return FontMetrics(); }

	void setColor(Color c) {c;}
	void fillRect(int x, int y, int w, int h) {x;y;w;h;}
	void fillArc(int x, int y, int rx, int ry, int a1, int a2) {x;y;rx;ry;a1;a2;}
	void fillOval(int x, int y, int rx, int ry) {x;y;rx;ry;}
	void drawOval(int x, int y, int rx, int ry) {x;y;rx;ry;}
};

class Event
{
public:
	int id;
	int key;
};



namespace System
{
static Timer mTimer;
namespace out
{
void println(str s)
{
	LogType::GetLog(LogType::SUB_ROOT)->RawPrint(s+_T("\n"));
}
}
long currentTimeMillis()
{
	return (long)(mTimer.QueryTimeDiff()*1000L);
}
}



namespace Math
{
double random()
{
	return Random::Uniform();
}
template<class _T> static _T abs(_T v)
{
	return (v > 0)? v : -v;
}
double sqrt(double v)
{
	return ::sqrt(v);
}
double pow(double u, double v)
{
	return ::pow(u, v);
}
}



}
