#pragma once

#include <nana\gui\widgets\form.hpp>
#include <nana\gui\place.hpp>
#include <nana\gui\widgets\spinbox.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\combox.hpp>
#include <nana\gui\widgets\checkbox.hpp>
#include <opencv2\opencv.hpp>

namespace pb
{
	class filter_window
	{
	private:
		nana::form _fm;
		nana::place _pl;
		nana::combox _cb;
		nana::checkbox _norm_chbox;
		nana::spinbox _sb[9];
		nana::button _btn;
	public:
		filter_window();
		cv::Mat values();
		bool normalization_checkbox_checked();
		nana::button& accept_button();
	};
}