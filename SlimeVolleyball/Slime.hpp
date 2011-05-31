#pragma once
#include "../Lepra/Include/Graphics2D.h"
#include "../Lepra/Include/Log.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/StringUtility.h"
#include "../Lepra/Include/Timer.h"
#include "../UiLepra/Include/UiDisplayManager.h"
#include "../UiTBC/Include/UiPainter.h"



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
	int stringWidth(str s) { return mPainter->GetStringWidth(s); }
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

	Graphics():
		width(0),
		height(0)
	{
	}

	Graphics(const UiLepra::DisplayManager* pDisplayManager,
		UiTbc::Painter* pPainter):
		width(pDisplayManager->GetWidth()),
		height(pDisplayManager->GetHeight()),
		mPainter(pPainter)
	{
	}

	void centerString(str s, int y)
	{
		drawString(s, width / 2 - getFontMetrics().stringWidth(s) / 2, y - getFontMetrics().getAscent()/2);
	}
	void drawString(str s, int x, int y)
	{
		mPainter->PrintText(s, x, y);
	}

	FontMetrics getFontMetrics() { return FontMetrics(mPainter); }

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

private:
	void DrawFan(int x, int y, int rx, int ry, int a1, int a2, bool pFill)
	{
		size_t lCurveCount = std::max(rx, ry) / 3 + 5;
		std::vector<Vector2DF> lCoords;
		const float lMidX = x + rx*0.5f;
		const float lMidY = y + ry*0.5f;
		if (pFill)
		{
			lCoords.push_back(Vector2DF(lMidX, lMidY));
		}
		const float lStartAngle = Lepra::Math::Deg2Rad((float)a1);
		const float lEndAngle = Lepra::Math::Deg2Rad((float)a2);
		const float lDeltaAngle = (lEndAngle-lStartAngle)/(lCurveCount-1);
		const float lXRadius = rx*0.5f;
		const float lYRadius = ry*0.5f;
		float lAngle = lStartAngle;
		for (size_t i = 0; i < lCurveCount; ++i)
		{
			lCoords.push_back(Vector2DF(
				lMidX + cos(lAngle)*lXRadius,
				lMidY - sin(lAngle)*lYRadius));
			lAngle += lDeltaAngle;
		}
		mPainter->DrawFan(lCoords, pFill);
	}

	UiTbc::Painter* mPainter;
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
inline void println(str s)
{
	LogType::GetLog(LogType::SUB_ROOT)->RawPrint(s+_T("\n"));
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
	return Random::Uniform();
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
