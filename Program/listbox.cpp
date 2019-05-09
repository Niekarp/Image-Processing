#include "listbox.h"

namespace pb
{
	listbox::listbox()
	{
		_lb.append_header("Nr.");
		_lb.append_header("Nazwa");
		_lb.column_at(0).width(35);
		_lb.column_at(1).width(150);
		_lb.column_at(1).text_align(nana::align::center);
	}
	void listbox::create(nana::window handle)
	{
		_lb.create(handle);
	}
	nana::window listbox::handle()
	{
		return _lb.handle();
	}
	unsigned listbox::size()
	{
		return _lb.at(0).size();
	}
	void listbox::focus()
	{
		_lb.focus();
	}
	nana::general_events& listbox::events()
	{
		return _lb.events();
	}
	void listbox::add_img(std::string img_name, cv::Mat img)
	{
		_images.push_back(pb::img{img_name, img});
		auto cat = _lb.at(0);
		cat.append({ std::to_string(_lb.at(0).size() + 1), img_name });
	}
	void listbox::del_selected()
	{
		auto selected_imgs = _lb.selected();
		std::reverse(selected_imgs.begin(), selected_imgs.end());
		for (auto it = selected_imgs.begin(); it != selected_imgs.end(); ++it)
		{
			unsigned selected_img_index = it->item;
			_images.erase(_images.begin() + selected_img_index);
		}
		_lb.erase(selected_imgs);
	}
	pb::img listbox::get_image(unsigned index)
	{
		return _images.at(index);
	}
	nana::listbox::index_pairs listbox::selected()
	{
		return _lb.selected();
	}
}