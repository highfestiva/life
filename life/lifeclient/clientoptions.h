
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <math.h>



namespace life {
namespace options {



struct CamControl {
	enum Direction {
		kCamdirUp = 0,
		kCamdirDown,
		kCamdirLeft,
		kCamdirRight,
		kCamdirForward,
		kCamdirBackward,
		kCamdirCount
	};
	float control_[kCamdirCount];

	inline CamControl() {
		::memset(control_, 0, sizeof(control_));
	}

	inline void operator=(const CamControl& control) {
		::memcpy(control_, control.control_, sizeof(control_));
	}
};



struct Steering {
	enum Control {
		kControlForward = 0,
		kControlBackward,
		kControlForward3D,
		kControlBackward3D,
		kControlBrakeAndBack,
		kControlLeft,
		kControlRight,
		kControlLeft3D,
		kControlRight3D,
		kControlUp,
		kControlDown,
		kControlUp3D,
		kControlDown3D,
		kControlHandbrake,
		kControlBrake,
		kControlYawAngle,
		kControlPitchAngle,
		kControlCount
	};
	float control_[kControlCount];

	inline Steering() {
		::memset(control_, 0, sizeof(control_));
	}

	inline void operator=(const Steering& vehicle) {
		::memcpy(control_, vehicle.control_, sizeof(control_));
	}

	inline float operator-(const Steering& vehicle) const {
		float sum = 0;
		for (int x = 0; x < kControlCount; ++x) {
			sum += fabs(control_[x]-vehicle.control_[x]);
		}
		return (sum);
	}
};



struct FireControl {
	enum Control {
		kFire0 = 0,
		kFire1,
		kFire2,
		kFireCount
	};
	float control_[kFireCount];

	inline FireControl() {
		::memset(control_, 0, sizeof(control_));
	}

	inline void operator=(const FireControl& shooting) {
		::memcpy(control_, shooting.control_, sizeof(control_));
	}
};



}
}
