#pragma once
#include "../Lepra/Include/Graphics2D.h"
#include "../Lepra/Include/Log.h"
#include "../Lepra/Include/Logger.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/StringUtility.h"
#include "../Lepra/Include/Timer.h"
#include "../UiTbc/Include/UiPainter.h"



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
	FontMetrics(UiTbc::Painter* pPainter):
		mPainter(pPainter)
	{
	}
	int stringWidth(astr s) { return mPainter->GetStringWidth(strutil::Encode(s)); }
	int getHeight() { return mPainter->GetFontHeight(); }
	int getAscent() { return getHeight()*4/5; }

private:
	UiTbc::Painter* mPainter;
};

class Graphics
{
public:
	int width;
	int height;
	UiTbc::Painter* mPainter;

	Graphics():
		width(0),
		height(0)
	{
	}

	Graphics(int w, int h,
		UiTbc::Painter* pPainter):
		width(w),
		height(h),
		mPainter(pPainter)
	{
	}

	void centerString(astr s, int y)
	{
		typedef astrutil::strvec svec;
		svec ss = astrutil::Split(s, "\n");
		for (svec::iterator x = ss.begin(); x != ss.end(); ++x)
		{
			drawString(*x, width / 2 - getFontMetrics().stringWidth(*x) / 2, y - getFontMetrics().getAscent()/2);
			y += getFontMetrics().getHeight();
		}
	}
	void drawString(astr s, int x, int y)
	{
		mPainter->PrintText(strutil::Encode(s), x, y);
	}

	FontMetrics getFontMetrics()
	{
		return FontMetrics(mPainter);
	}

	void setColor(Color c)
	{
		mPainter->SetColor(c, 0);
		mPainter->SetColor(c, 1);
	}
	void fillRect(int x, int y, int w, int h)
	{
		mPainter->FillRect(x, y, x+w, y+h);
	}
	void fillArc(int x, int y, int rx, int ry, int a1, int a2)
	{
		DrawFan(x, y, rx, ry, a1, a2, true);
	}
	void fillOval(int x, int y, int rx, int ry)
	{
		DrawFan(x, y, rx, ry, 0, 360, true);
	}
	void drawOval(int x, int y, int rx, int ry)
	{
		DrawFan(x, y, rx, ry, 0, 360, false);
	}
	void drawArc(int x, int y, int rx, int ry, int a1, int a2)
	{
		DrawFan(x, y, rx, ry, a1, a2, false);
	}

private:
	void DrawFan(int x, int y, int rx, int ry, int a1, int a2, bool pFill)
	{
		const size_t lCurveCount = ((rx*2 + ry*2) / 20 + std::abs(a1-a2)/20 + 12) & (~7);
		std::vector<vec2> lCoords;
		const float lMidX = x + rx*0.5f;
		const float lMidY = y + ry*0.5f;
		if (pFill)
		{
			lCoords.push_back(vec2(lMidX, lMidY));
		}
		const float lStartAngle = Lepra::Math::Deg2Rad((float)a1);
		const float lEndAngle = Lepra::Math::Deg2Rad((float)a2);
		const float lDeltaAngle = (lEndAngle-lStartAngle)/(lCurveCount-1);
		const float lXRadius = rx*0.5f;
		const float lYRadius = ry*0.5f;
		float lAngle = lStartAngle;
		for (size_t i = 0; i < lCurveCount; ++i)
		{
			lCoords.push_back(vec2(
				lMidX + cos(lAngle)*lXRadius,
				lMidY - sin(lAngle)*lYRadius));
			lAngle += lDeltaAngle;
		}
		mPainter->DrawFan(lCoords, pFill);
	}
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
inline void println(astr s)
{
	LogType::GetLogger(LogType::SUB_ROOT)->RawPrint(strutil::Encode(s)+_T("\n"));
}
}
inline long currentTimeMillis()
{
	return (long)(mTimer.QueryTimeDiff()*1000L);
}
}



namespace Math
{
inline double random()
{
	return Random::Uniform(0.0, 1.0);
}
template<class _T> static _T abs(_T v)
{
	return (v > 0)? v : -v;
}
inline double sqrt(double v)
{
	return ::sqrt(v);
}
inline double pow(double u, double v)
{
	return ::pow(u, v);
}
}



}
