
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/cyclicarray.h"
#include "../include/math.h"
#include "../include/number.h"



namespace lepra {



str Number::ConvertToPostfixNumber(double number, unsigned decimals) {
	str result_string;
	if (number != 0) {
		struct PrefixInfo {
			int exponent_;
			const char* prefix_;
		};
		PrefixInfo prefix_list[] =
		{
			{12, "T"}, {9, "G"}, {6, "M"}, {3, "k"}, {0, ""},
			{-3, "m"}, {-6, "u"}, {-9, "n"}
		};
		double result = 0;
		int x;
		for (x = 0; x < (int)LEPRA_ARRAY_COUNT(prefix_list)-1; ++x) {
			result = number/::pow(10.0, prefix_list[x].exponent_);
			if (::fabs(result) >= 1) {
				break;
			}
		}
		str format_string = strutil::IntToString(decimals, 10);
		result_string = strutil::Format(("%."+format_string+"f %s").c_str(), result, prefix_list[x].prefix_);
	} else {
		result_string = "0 ";
	}
	return (result_string);
}



}
