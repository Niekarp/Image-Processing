#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <utility>

namespace pb
{
	typedef std::pair<std::string, cv::Mat> img;

	class listbox
	{
	private:
		nana::listbox _lb;
		std::vector<pb::img> _images;
	public:
		listbox();
		void create(nana::window handle);

		nana::window handle();
		unsigned size();
		void focus();
		nana::general_events& events();
		void add_img(std::string img_name, cv::Mat img);
		void del_selected();
		pb::img get_image(unsigned index);
		nana::listbox::index_pairs selected();
	};
}