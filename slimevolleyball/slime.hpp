#pragma once
#include "../lepra/include/graphics2d.h"
#include "../lepra/include/log.h"
#include "../lepra/include/logger.h"
#include "../lepra/include/random.h"
#include "../lepra/include/stringutility.h"
#include "../lepra/include/timer.h"
#include "../uitbc/include/uipainter.h"



#define CLEAR_MEMBERS(first, last)		\
{						\
	char* start = (char*)&first;		\
	char* __end = (char*)&last;		\
	__end += sizeof(last);			\
	::memset(start, 0, __end-start);	\
}

#define null 0



namespace slime {



using namespace lepra;


class FontMetrics {
public:
	FontMetrics(uitbc::Painter* painter):
		painter_(painter) {
	}
	int stringWidth(str s) { return painter_->GetStringWidth(wstrutil::Encode(s)); }
	int getHeight() { return painter_->GetFontHeight(); }
	int getAscent() { return getHeight()*4/5; }

private:
	uitbc::Painter* painter_;
};

class Graphics {
public:
	int width;
	int height;
	uitbc::Painter* painter_;

	Graphics():
		width(0),
		height(0) {
	}

	Graphics(int w, int h,
		uitbc::Painter* painter):
		width(w),
		height(h),
		painter_(painter) {
	}

	void centerString(str s, int y) {
		typedef strutil::strvec svec;
		svec ss = strutil::Split(s, "\n");
		for (svec::iterator x = ss.begin(); x != ss.end(); ++x) {
			drawString(*x, width / 2 - getFontMetrics().stringWidth(*x) / 2, y - getFontMetrics().getAscent()/2);
			y += getFontMetrics().getHeight();
		}
	}
	void drawString(str s, int x, int y) {
		painter_->PrintText(wstrutil::Encode(s), x, y);
	}

	FontMetrics getFontMetrics() {
		return FontMetrics(painter_);
	}

	void setColor(Color c) {
		painter_->SetColor(c, 0);
		painter_->SetColor(c, 1);
	}
	void fillRect(int x, int y, int w, int h) {
		painter_->FillRect(x, y, x+w, y+h);
	}
	void fillArc(int x, int y, int rx, int ry, int a1, int a2) {
		DrawFan(x, y, rx, ry, a1, a2, true);
	}
	void fillOval(int x, int y, int rx, int ry) {
		DrawFan(x, y, rx, ry, 0, 360, true);
	}
	void drawOval(int x, int y, int rx, int ry) {
		DrawFan(x, y, rx, ry, 0, 360, false);
	}
	void drawArc(int x, int y, int rx, int ry, int a1, int a2) {
		DrawFan(x, y, rx, ry, a1, a2, false);
	}

private:
	void DrawFan(int x, int y, int rx, int ry, int a1, int a2, bool fill) {
		const size_t curve_count = ((rx*2 + ry*2) / 20 + std::abs(a1-a2)/20 + 12) & (~7);
		std::vector<vec2> coords;
		const float mid_x = x + rx*0.5f;
		const float mid_y = y + ry*0.5f;
		if (fill) {
			coords.push_back(vec2(mid_x, mid_y));
		}
		const float start_angle = lepra::Math::Deg2Rad((float)a1);
		const float end_angle = lepra::Math::Deg2Rad((float)a2);
		const float delta_angle = (end_angle-start_angle)/(curve_count-1);
		const float x_radius = rx*0.5f;
		const float y_radius = ry*0.5f;
		float angle = start_angle;
		for (size_t i = 0; i < curve_count; ++i) {
			coords.push_back(vec2(
				mid_x + cos(angle)*x_radius,
				mid_y - sin(angle)*y_radius));
			angle += delta_angle;
		}
		painter_->DrawFan(coords, fill);
	}
};

class Event {
public:
	int id;
	int key;
};



namespace system {
static Timer timer_;
namespace out {
inline void println(str s) {
	LogType::GetLogger(LogType::kRoot)->RawPrint(s+"\n");
}
}
inline long currentTimeMillis() {
	return (long)(timer_.QueryTimeDiff()*1000L);
}
}



namespace Math {
inline double random() {
	return Random::Uniform(0.0, 1.0);
}
template<class _T> static _T abs(_T v) {
	return (v > 0)? v : -v;
}
inline double sqrt(double v) {
	return ::sqrt(v);
}
inline double pow(double u, double v) {
	return ::pow(u, v);
}
}



}
