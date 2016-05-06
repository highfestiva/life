/*
	Class:  Edge
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	Class Edge and other Edge-classes.
	Used to store info about a polygon edge during rasterization.

	Edge is responsible of clipping against the left and right
	edges of the screen. This is done by returning clipped coordinates
	when calling a Get() function.

	Clipping against top is accomplished by calling Step(count). The bottom
	edge is the easiest to clip against, and doesn't need any special functionality.
	The triangle renderer is responsible of this.

	I made it like this to avoid the problems that occur if all ScanLine-
	functions would do the clipping instead.
*/

#pragma once

#include "uitbc.h"
#include "UiGradients.h"

namespace uitbc {

class Vertex3D;
class Vertex3DUV;
class Vertex3DRGB;
class Vertex3DUVRGB;
class Vertex3DUVM;
class Vertex3DUVRGBM;

class Vertex2D;
class Vertex2DUV;
class Vertex2DRGBA;

class Edge {
public:

	virtual void Init(const Vertex3D* v0, const Vertex3D* v1);
	void Init(const Vertex2D* v0, const Vertex2D* v1);

	inline static void SetClipLeftRight(int clip_left, int clip_right);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual inline void Reset();

	inline int GetX();
	inline int GetY();
	inline int GetHeight();

protected:

	int x_;				// DDA info for x.
	int x_step_;			// ..
	int numerator_;		// ..
	int denominator_;		// ..
	int error_term_;		// ..

	int y_;				// Current y
	int height_;			// Vertical count...

	static int clip_left_;
	static int clip_right_;

	int start_x_;
	int start_y_;
	int start_height_;
	int start_error_term_;
	float x_prestep_;
	float y_prestep_;
};

void Edge::SetClipLeftRight(int clip_left, int clip_right) {
	clip_left_ = clip_left;
	clip_right_ = clip_right;
}

int Edge::GetX() {
	if (x_ < clip_left_) {
		return clip_left_;
	} else if(x_ > clip_right_) {
		return clip_right_;
	} else {
		return x_;
	}
}

int Edge::GetY() {
	return y_;
}

int Edge::GetHeight() {
	return height_;
}

bool Edge::Step() {
	bool step_extra = false;

	x_ += x_step_;
	y_++;
	height_--;

	error_term_ += numerator_;

	if ((unsigned)error_term_ >= (unsigned)denominator_) {
		step_extra = true;

		x_++;

		error_term_ -= denominator_;
	}

	return step_extra;
}

int Edge::Step(unsigned count) {
	x_ += x_step_ * count;
	y_ += count;
	height_ -= count;

	error_term_ += numerator_ * count;

	if ((unsigned)error_term_ >= (unsigned)denominator_ &&
		denominator_ > 0) {
		int _count = (unsigned)error_term_ / (unsigned)denominator_;
		x_ += _count;

		error_term_ -= ((unsigned)denominator_ * _count);

		return _count;
	}

	return 0;
}

void Edge::Reset() {
	x_ = start_x_;
	y_ = start_y_;
	height_ = start_height_;
	error_term_ = start_error_term_;
}















class EdgeZ : public Edge {
public:

	virtual void Init(const Vertex3D* v0,
					  const Vertex3D* v1,
					  const Gradients* gradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const Gradients* gradients);

	inline float GetOneOverZ();

private:

	float one_over_z_;		// 1 / z
	float one_over_z_step_;
	float one_over_z_step_extra_;

	const Gradients* gradients_;

	float start_one_over_z_;		// 1 / z
};

bool EdgeZ::Step() {
	bool step_extra = Edge::Step();

	one_over_z_ += one_over_z_step_;

	if (step_extra == true) {
		one_over_z_ += one_over_z_step_extra_;
	}

	return step_extra;
}

int EdgeZ::Step(unsigned count) {
	int extra_steps = Edge::Step(count);

	one_over_z_ += one_over_z_step_ * (float)count;

	if (extra_steps > 0) {
		one_over_z_ += one_over_z_step_extra_ * (float)extra_steps;
	}

	return extra_steps;
}

float EdgeZ::GetOneOverZ() {
	if (x_ < clip_left_) {
		return one_over_z_ + gradients_->GetOneOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return one_over_z_ - gradients_->GetOneOverZXStep() * (x_ - clip_right_);
	} else {
		return one_over_z_;
	}
}














class EdgeUV : public Edge {
public:

	virtual void Init(const Vertex3DUV* v0,
					  const Vertex3DUV* v1,
					  const GradientsUV* gradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const GradientsUV* gradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();

private:
	float one_over_z_;		// 1 / z
	float one_over_z_step_;
	float one_over_z_step_extra_;

	float u_over_z_;		// u / z
	float u_over_z_step_;
	float u_over_z_step_extra_;

	float v_over_z_;		// v / z
	float v_over_z_step_;
	float v_over_z_step_extra_;

	const GradientsUV* gradients_;

	float start_one_over_z_;
	float start_u_over_z_;
	float start_v_over_z_;
};

bool EdgeUV::Step() {
	bool step_extra = Edge::Step();

	one_over_z_ += one_over_z_step_;
	u_over_z_   += u_over_z_step_;
	v_over_z_   += v_over_z_step_;

	if (step_extra == true) {
		one_over_z_ += one_over_z_step_extra_;
		u_over_z_   += u_over_z_step_extra_;
		v_over_z_   += v_over_z_step_extra_;
	}

	return step_extra;
}

int EdgeUV::Step(unsigned count) {
	int extra_steps = Edge::Step(count);
	float _count = (float)count;

	one_over_z_ += one_over_z_step_ * _count;
	u_over_z_   += u_over_z_step_ * _count;
	v_over_z_   += v_over_z_step_ * _count;

	if (extra_steps > 0) {
		float extra_steps_float = (float)extra_steps;
		one_over_z_ += one_over_z_step_extra_ * extra_steps_float;
		u_over_z_   += u_over_z_step_extra_ * extra_steps_float;
		v_over_z_   += v_over_z_step_extra_ * extra_steps_float;
	}

	return extra_steps;
}

float EdgeUV::GetOneOverZ() {
	if (x_ < clip_left_) {
		return one_over_z_ + gradients_->GetOneOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return one_over_z_ - gradients_->GetOneOverZXStep() * (x_ - clip_right_);
	} else {
		return one_over_z_;
	}
}

float EdgeUV::GetUOverZ() {
	if (x_ < clip_left_) {
		return u_over_z_ + gradients_->GetUOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return u_over_z_ - gradients_->GetUOverZXStep() * (x_ - clip_right_);
	} else {
		return u_over_z_;
	}
}

float EdgeUV::GetVOverZ() {
	if (x_ < clip_left_) {
		return v_over_z_ + gradients_->GetVOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return v_over_z_ - gradients_->GetVOverZXStep() * (x_ - clip_right_);
	} else {
		return v_over_z_;
	}
}
















class EdgeRGB : public Edge {
public:

	virtual void Init(const Vertex3DRGB* v0,
					  const Vertex3DRGB* v1,
					  const GradientsRGB* gradients,
					  bool gamma_convert);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const GradientsRGB* gradients);

	inline float GetOneOverZ();
	inline float GetROverZ();
	inline float GetGOverZ();
	inline float GetBOverZ();

private:

	float one_over_z_;		// 1 / z
	float one_over_z_step_;
	float one_over_z_step_extra_;

	float r_over_z_;		// r / z
	float r_over_z_step_;
	float r_over_z_step_extra_;

	float g_over_z_;	// g / z
	float g_over_z_step_;
	float g_over_z_step_extra_;

	float b_over_z_;		// b / z
	float b_over_z_step_;
	float b_over_z_step_extra_;

	const GradientsRGB* gradients_;

	float start_one_over_z_;
	float start_r_over_z_;
	float start_g_over_z_;
	float start_b_over_z_;
};

bool EdgeRGB::Step() {
	bool step_extra = Edge::Step();

	one_over_z_ += one_over_z_step_;
	r_over_z_ += r_over_z_step_;
	g_over_z_ += g_over_z_step_;
	b_over_z_ += b_over_z_step_;

	if (step_extra == true) {
		one_over_z_ += one_over_z_step_extra_;
		r_over_z_ += r_over_z_step_extra_;
		g_over_z_ += g_over_z_step_extra_;
		b_over_z_ += b_over_z_step_extra_;
	}

	return step_extra;
}

int EdgeRGB::Step(unsigned count) {
	int extra_steps = Edge::Step(count);
	float _count = (float)count;

	one_over_z_ += one_over_z_step_ * _count;
	r_over_z_ += r_over_z_step_ * _count;
	g_over_z_ += g_over_z_step_ * _count;
	b_over_z_ += b_over_z_step_ * _count;

	if (extra_steps > 0) {
		float extra_steps_float = (float)extra_steps;
		one_over_z_ += one_over_z_step_extra_ * extra_steps_float;
		r_over_z_ += r_over_z_step_extra_ * extra_steps_float;
		g_over_z_ += g_over_z_step_extra_ * extra_steps_float;
		b_over_z_ += b_over_z_step_extra_ * extra_steps_float;
	}

	return extra_steps;
}

float EdgeRGB::GetOneOverZ() {
	if (x_ < clip_left_) {
		return one_over_z_ + gradients_->GetOneOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return one_over_z_ - gradients_->GetOneOverZXStep() * (x_ - clip_right_);
	} else {
		return one_over_z_;
	}
}

float EdgeRGB::GetROverZ() {
	if (x_ < clip_left_) {
		return r_over_z_ + gradients_->GetROverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return r_over_z_ - gradients_->GetROverZXStep() * (x_ - clip_right_);
	} else {
		return r_over_z_;
	}
}

float EdgeRGB::GetGOverZ() {
	if (x_ < clip_left_) {
		return g_over_z_ + gradients_->GetGOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return g_over_z_ - gradients_->GetGOverZXStep() * (x_ - clip_right_);
	} else {
		return g_over_z_;
	}
}

float EdgeRGB::GetBOverZ() {
	if (x_ < clip_left_) {
		return b_over_z_ + gradients_->GetBOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return b_over_z_ - gradients_->GetBOverZXStep() * (x_ - clip_right_);
	} else {
		return b_over_z_;
	}
}
















class EdgeUVRGB : public Edge {
public:

	virtual void Init(const Vertex3DUVRGB* v0,
					  const Vertex3DUVRGB* v1,
					  const GradientsUVRGB* gradients,
					  bool gamma_convert);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const GradientsUVRGB* gradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();
	inline float GetROverZ();
	inline float GetGOverZ();
	inline float GetBOverZ();

private:

	float one_over_z_;		// 1 / z
	float one_over_z_step_;
	float one_over_z_step_extra_;

	float u_over_z_;		// u / z
	float u_over_z_step_;
	float u_over_z_step_extra_;

	float v_over_z_;		// v / z
	float v_over_z_step_;
	float v_over_z_step_extra_;

	float r_over_z_;		// r / z
	float r_over_z_step_;
	float r_over_z_step_extra_;

	float g_over_z_;	// g / z
	float g_over_z_step_;
	float g_over_z_step_extra_;

	float b_over_z_;		// b / z
	float b_over_z_step_;
	float b_over_z_step_extra_;

	const GradientsUVRGB* gradients_;

	float start_one_over_z_;
	float start_u_over_z_;
	float start_v_over_z_;
	float start_r_over_z_;
	float start_g_over_z_;
	float start_b_over_z_;
};

bool EdgeUVRGB::Step() {
	bool step_extra = Edge::Step();

	one_over_z_ += one_over_z_step_;
	u_over_z_ += u_over_z_step_;
	v_over_z_ += v_over_z_step_;
	r_over_z_ += r_over_z_step_;
	g_over_z_ += g_over_z_step_;
	b_over_z_ += b_over_z_step_;

	if (step_extra == true) {
		one_over_z_ += one_over_z_step_extra_;
		u_over_z_ += u_over_z_step_extra_;
		v_over_z_ += v_over_z_step_extra_;
		r_over_z_ += r_over_z_step_extra_;
		g_over_z_ += g_over_z_step_extra_;
		b_over_z_ += b_over_z_step_extra_;
	}

	return step_extra;
}

int EdgeUVRGB::Step(unsigned count) {
	int extra_steps = Edge::Step(count);
	float _count = (float)count;

	one_over_z_ += one_over_z_step_ * _count;
	u_over_z_ += u_over_z_step_ * _count;
	v_over_z_ += v_over_z_step_ * _count;
	r_over_z_ += r_over_z_step_ * _count;
	g_over_z_ += g_over_z_step_ * _count;
	b_over_z_ += b_over_z_step_ * _count;

	if (extra_steps > 0) {
		float extra_steps_float = (float)extra_steps;
		one_over_z_ += one_over_z_step_extra_ * extra_steps_float;
		u_over_z_ += u_over_z_step_extra_ * extra_steps_float;
		v_over_z_ += v_over_z_step_extra_ * extra_steps_float;
		r_over_z_ += r_over_z_step_extra_ * extra_steps_float;
		g_over_z_ += g_over_z_step_extra_ * extra_steps_float;
		b_over_z_ += b_over_z_step_extra_ * extra_steps_float;
	}

	return extra_steps;
}

float EdgeUVRGB::GetOneOverZ() {
	if (x_ < clip_left_) {
		return one_over_z_ + gradients_->GetOneOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return one_over_z_ - gradients_->GetOneOverZXStep() * (x_ - clip_right_);
	} else {
		return one_over_z_;
	}
}

float EdgeUVRGB::GetUOverZ() {
	if (x_ < clip_left_) {
		return u_over_z_ + gradients_->GetUOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return u_over_z_ - gradients_->GetUOverZXStep() * (x_ - clip_right_);
	} else {
		return u_over_z_;
	}
}

float EdgeUVRGB::GetVOverZ() {
	if (x_ < clip_left_) {
		return v_over_z_ + gradients_->GetVOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return v_over_z_ - gradients_->GetVOverZXStep() * (x_ - clip_right_);
	} else {
		return v_over_z_;
	}
}

float EdgeUVRGB::GetROverZ() {
	if (x_ < clip_left_) {
		return r_over_z_ + gradients_->GetROverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return r_over_z_ - gradients_->GetROverZXStep() * (x_ - clip_right_);
	} else {
		return r_over_z_;
	}
}

float EdgeUVRGB::GetGOverZ() {
	if (x_ < clip_left_) {
		return g_over_z_ + gradients_->GetGOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return g_over_z_ - gradients_->GetGOverZXStep() * (x_ - clip_right_);
	} else {
		return g_over_z_;
	}
}

float EdgeUVRGB::GetBOverZ() {
	if (x_ < clip_left_) {
		return b_over_z_ + gradients_->GetBOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return b_over_z_ - gradients_->GetBOverZXStep() * (x_ - clip_right_);
	} else {
		return b_over_z_;
	}
}

















class EdgeUVM : public Edge {
public:

	virtual void Init(const Vertex3DUVM* v0,
					  const Vertex3DUVM* v1,
					  const GradientsUVM* gradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const GradientsUVM* gradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();
	inline float GetMOverZ();

private:

	float one_over_z_;		// 1 / z
	float one_over_z_step_;
	float one_over_z_step_extra_;

	float u_over_z_;		// u / z
	float u_over_z_step_;
	float u_over_z_step_extra_;

	float v_over_z_;		// v / z
	float v_over_z_step_;
	float v_over_z_step_extra_;

	float m_over_z_;		// m / z
	float m_over_z_step_;
	float m_over_z_step_extra_;

	const GradientsUVM* gradients_;

	float start_one_over_z_;
	float start_u_over_z_;
	float start_v_over_z_;
	float start_m_over_z_;
};

bool EdgeUVM::Step() {
	bool step_extra = Edge::Step();

	one_over_z_ += one_over_z_step_;
	u_over_z_   += u_over_z_step_;
	v_over_z_   += v_over_z_step_;
	m_over_z_   += m_over_z_step_;

	if (step_extra == true) {
		one_over_z_ += one_over_z_step_extra_;
		u_over_z_   += u_over_z_step_extra_;
		v_over_z_   += v_over_z_step_extra_;
		m_over_z_   += m_over_z_step_extra_;
	}

	return step_extra;
}

int EdgeUVM::Step(unsigned count) {
	int extra_steps = Edge::Step(count);
	float _count = (float)count;

	one_over_z_ += one_over_z_step_ * _count;
	u_over_z_ += u_over_z_step_ * _count;
	v_over_z_ += v_over_z_step_ * _count;
	m_over_z_ += m_over_z_step_ * _count;

	if (extra_steps > 0) {
		float extra_steps_float = (float)extra_steps;
		one_over_z_ += one_over_z_step_extra_ * extra_steps_float;
		u_over_z_ += u_over_z_step_extra_ * extra_steps_float;
		v_over_z_ += v_over_z_step_extra_ * extra_steps_float;
		m_over_z_ += m_over_z_step_extra_ * extra_steps_float;
	}

	return extra_steps;
}

float EdgeUVM::GetOneOverZ() {
	if (x_ < clip_left_) {
		return one_over_z_ + gradients_->GetOneOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return one_over_z_ - gradients_->GetOneOverZXStep() * (x_ - clip_right_);
	} else {
		return one_over_z_;
	}
}

float EdgeUVM::GetUOverZ() {
	if (x_ < clip_left_) {
		return u_over_z_ + gradients_->GetUOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return u_over_z_ - gradients_->GetUOverZXStep() * (x_ - clip_right_);
	} else {
		return u_over_z_;
	}
}

float EdgeUVM::GetVOverZ() {
	if (x_ < clip_left_) {
		return v_over_z_ + gradients_->GetVOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return v_over_z_ - gradients_->GetVOverZXStep() * (x_ - clip_right_);
	} else {
		return v_over_z_;
	}
}

float EdgeUVM::GetMOverZ() {
	if (x_ < clip_left_) {
		return m_over_z_ + gradients_->GetMOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return m_over_z_ - gradients_->GetMOverZXStep() * (x_ - clip_right_);
	} else {
		return m_over_z_;
	}
}
















class EdgeUVRGBM : public Edge {
public:

	virtual void Init(const Vertex3DUVRGBM* v0,
					  const Vertex3DUVRGBM* v1,
					  const GradientsUVRGBM* gradients,
					  bool gamma_convert);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const GradientsUVRGBM* gradients);

	inline float GetOneOverZ();
	inline float GetUOverZ();
	inline float GetVOverZ();
	inline float GetROverZ();
	inline float GetGOverZ();
	inline float GetBOverZ();
	inline float GetMOverZ();

private:

	float one_over_z_;		// 1 / z
	float one_over_z_step_;
	float one_over_z_step_extra_;

	float u_over_z_;		// u / z
	float u_over_z_step_;
	float u_over_z_step_extra_;

	float v_over_z_;		// v / z
	float v_over_z_step_;
	float v_over_z_step_extra_;

	float r_over_z_;		// r / z
	float r_over_z_step_;
	float r_over_z_step_extra_;

	float g_over_z_;	// g / z
	float g_over_z_step_;
	float g_over_z_step_extra_;

	float b_over_z_;		// b / z
	float b_over_z_step_;
	float b_over_z_step_extra_;

	float m_over_z_;		// m / z
	float m_over_z_step_;
	float m_over_z_step_extra_;

	const GradientsUVRGBM* gradients_;

	float start_one_over_z_;
	float start_u_over_z_;
	float start_v_over_z_;
	float start_r_over_z_;
	float start_g_over_z_;
	float start_b_over_z_;
	float start_m_over_z_;
};

bool EdgeUVRGBM::Step() {
	bool step_extra = Edge::Step();

	one_over_z_ += one_over_z_step_;
	u_over_z_ += u_over_z_step_;
	v_over_z_ += v_over_z_step_;
	r_over_z_ += r_over_z_step_;
	g_over_z_ += g_over_z_step_;
	b_over_z_ += b_over_z_step_;
	m_over_z_ += m_over_z_step_;

	if (step_extra == true) {
		one_over_z_ += one_over_z_step_extra_;
		u_over_z_ += u_over_z_step_extra_;
		v_over_z_ += v_over_z_step_extra_;
		r_over_z_ += r_over_z_step_extra_;
		g_over_z_ += g_over_z_step_extra_;
		b_over_z_ += b_over_z_step_extra_;
		m_over_z_ += m_over_z_step_extra_;
	}

	return step_extra;
}

int EdgeUVRGBM::Step(unsigned count) {
	int extra_steps = Edge::Step(count);
	float _count = (float)count;

	one_over_z_ += one_over_z_step_ * _count;
	u_over_z_ += u_over_z_step_ * _count;
	v_over_z_ += v_over_z_step_ * _count;
	r_over_z_ += r_over_z_step_ * _count;
	g_over_z_ += g_over_z_step_ * _count;
	b_over_z_ += b_over_z_step_ * _count;
	m_over_z_ += m_over_z_step_ * _count;

	if (extra_steps > 0) {
		float extra_steps_float = (float)extra_steps;
		one_over_z_ += one_over_z_step_extra_ * extra_steps_float;
		u_over_z_ += u_over_z_step_extra_ * extra_steps_float;
		v_over_z_ += v_over_z_step_extra_ * extra_steps_float;
		r_over_z_ += r_over_z_step_extra_ * extra_steps_float;
		g_over_z_ += g_over_z_step_extra_ * extra_steps_float;
		b_over_z_ += b_over_z_step_extra_ * extra_steps_float;
		m_over_z_ += m_over_z_step_extra_ * extra_steps_float;
	}

	return extra_steps;
}

float EdgeUVRGBM::GetOneOverZ() {
	if (x_ < clip_left_) {
		return one_over_z_ + gradients_->GetOneOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return one_over_z_ - gradients_->GetOneOverZXStep() * (x_ - clip_right_);
	} else {
		return one_over_z_;
	}
}

float EdgeUVRGBM::GetUOverZ() {
	if (x_ < clip_left_) {
		return u_over_z_ + gradients_->GetUOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return u_over_z_ - gradients_->GetUOverZXStep() * (x_ - clip_right_);
	} else {
		return u_over_z_;
	}
}

float EdgeUVRGBM::GetVOverZ() {
	if (x_ < clip_left_) {
		return v_over_z_ + gradients_->GetVOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return v_over_z_ - gradients_->GetVOverZXStep() * (x_ - clip_right_);
	} else {
		return v_over_z_;
	}
}

float EdgeUVRGBM::GetROverZ() {
	if (x_ < clip_left_) {
		return r_over_z_ + gradients_->GetROverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return r_over_z_ - gradients_->GetROverZXStep() * (x_ - clip_right_);
	} else {
		return r_over_z_;
	}
}

float EdgeUVRGBM::GetGOverZ() {
	if (x_ < clip_left_) {
		return g_over_z_ + gradients_->GetGOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return g_over_z_ - gradients_->GetGOverZXStep() * (x_ - clip_right_);
	} else {
		return g_over_z_;
	}
}

float EdgeUVRGBM::GetBOverZ() {
	if (x_ < clip_left_) {
		return b_over_z_ + gradients_->GetBOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return b_over_z_ - gradients_->GetBOverZXStep() * (x_ - clip_right_);
	} else {
		return b_over_z_;
	}
}

float EdgeUVRGBM::GetMOverZ() {
	if (x_ < clip_left_) {
		return m_over_z_ + gradients_->GetMOverZXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return m_over_z_ - gradients_->GetMOverZXStep() * (x_ - clip_right_);
	} else {
		return m_over_z_;
	}
}


















class Edge2DUV : public Edge {
public:

	virtual void Init(const Vertex2DUV* v0,
					  const Vertex2DUV* v1,
					  const Gradients2DUV* gradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const Gradients2DUV* gradients);

	inline float GetU();
	inline float GetV();

private:
	float u_;
	float u_step_;
	float u_step_extra_;

	float v_;
	float v_step_;
	float v_step_extra_;

	const Gradients2DUV* gradients_;

	float start_u_;
	float start_v_;
};

bool Edge2DUV::Step() {
	bool step_extra = Edge::Step();

	u_ += u_step_;
	v_ += v_step_;

	if (step_extra == true) {
		u_ += u_step_extra_;
		v_ += v_step_extra_;
	}

	return step_extra;
}

int Edge2DUV::Step(unsigned count) {
	int extra_steps = Edge::Step(count);
	float _count = (float)count;

	u_ += u_step_ * _count;
	v_ += v_step_ * _count;

	if (extra_steps > 0) {
		float extra_steps_float = (float)extra_steps;
		u_ += u_step_extra_ * extra_steps_float;
		v_ += v_step_extra_ * extra_steps_float;
	}

	return extra_steps;
}

float Edge2DUV::GetU() {
	if (x_ < clip_left_) {
		return u_ + gradients_->GetUXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return u_ - gradients_->GetUXStep() * (x_ - clip_right_);
	} else {
		return u_;
	}
}

float Edge2DUV::GetV() {
	if (x_ < clip_left_) {
		return v_ + gradients_->GetVXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return v_ - gradients_->GetVXStep() * (x_ - clip_right_);
	} else {
		return v_;
	}
}











class Edge2DRGBA : public Edge {
public:

	virtual void Init(const Vertex2DRGBA* v0,
					  const Vertex2DRGBA* v1,
					  const Gradients2DRGBA* gradients);

	virtual inline bool Step();
	virtual inline int Step(unsigned count);

	virtual void Reset(const Gradients2DRGBA* gradients);

	inline float GetR();
	inline float GetG();
	inline float GetB();
	inline float GetA();

private:
	float r_;
	float r_step_;
	float r_step_extra_;

	float g_;
	float g_step_;
	float g_step_extra_;

	float b_;
	float b_step_;
	float b_step_extra_;

	float a_;
	float a_step_;
	float a_step_extra_;

	const Gradients2DRGBA* gradients_;

	float start_r_;
	float start_g_;
	float start_b_;
	float start_a_;
};

bool Edge2DRGBA::Step() {
	bool step_extra = Edge::Step();

	r_ += r_step_;
	g_ += g_step_;
	b_ += b_step_;
	a_ += a_step_;

	if (step_extra == true) {
		r_ += r_step_extra_;
		g_ += g_step_extra_;
		b_ += b_step_extra_;
		a_ += a_step_extra_;
	}

	return step_extra;
}

int Edge2DRGBA::Step(unsigned count) {
	int extra_steps = Edge::Step(count);
	float _count = (float)count;

	r_ += r_step_ * _count;
	g_ += g_step_ * _count;
	b_ += b_step_ * _count;
	a_ += a_step_ * _count;

	if (extra_steps > 0) {
		float extra_steps_float = (float)extra_steps;
		r_ += r_step_extra_ * extra_steps_float;
		g_ += g_step_extra_ * extra_steps_float;
		b_ += b_step_extra_ * extra_steps_float;
		a_ += a_step_extra_ * extra_steps_float;
	}

	return extra_steps;
}

float Edge2DRGBA::GetR() {
	if (x_ < clip_left_) {
		return r_ + gradients_->GetRXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return r_ - gradients_->GetRXStep() * (x_ - clip_right_);
	} else {
		return r_;
	}
}

float Edge2DRGBA::GetG() {
	if (x_ < clip_left_) {
		return g_ + gradients_->GetGXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return g_ - gradients_->GetGXStep() * (x_ - clip_right_);
	} else {
		return g_;
	}
}

float Edge2DRGBA::GetB() {
	if (x_ < clip_left_) {
		return b_ + gradients_->GetBXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return b_ - gradients_->GetBXStep() * (x_ - clip_right_);
	} else {
		return b_;
	}
}

float Edge2DRGBA::GetA() {
	if (x_ < clip_left_) {
		return a_ + gradients_->GetAXStep() * (clip_left_ - x_);
	} else if(x_ > clip_right_) {
		return a_ - gradients_->GetAXStep() * (x_ - clip_right_);
	} else {
		return a_;
	}
}

}
