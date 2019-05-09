#pragma once

#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <numeric>

#include "listbox.h"
#include "filter_window.h"


#define key_enter 13
#define key_esc 27

namespace pb
{
	class window
	{
		enum class path_tb_mode_t { set_directory, add_img, show_pixel_info, change_pixel,
									scale_image, save_image_as, global_treshold_binarization,
									local_treshold_binarization_niblack, median_filter};
	private:
		nana::form _fm;
		nana::place _pl;
		nana::menubar _mn;
		nana::textbox _path_tb;
		path_tb_mode_t _path_tb_mode;
		pb::listbox _lb;
		nana::slider _sl;

		std::string _img_dir_path;
		std::fstream _database;

		void histogram(unsigned x, unsigned y, cv::Mat image, unsigned channel, cv::Mat& hist, bool grayscale=false);
		void streach(cv::Mat image, unsigned channels, cv::Mat& out, int a = -1, int b = 256);
		void equalize(cv::Mat image, cv::Mat& out);
		void treshold_binarization(cv::Mat& image, unsigned treshold);
		// funkcje pomocnicze
		std::string extract_filename(std::string filepath);
		std::string add_to_filename(std::string filename, std::string addition);
		void add_image_to_tmp(std::string image_filename, cv::Mat image, bool show);
		void change_path_tb_mode(pb::window::path_tb_mode_t new_mode);
	public:
		window();
	};
}