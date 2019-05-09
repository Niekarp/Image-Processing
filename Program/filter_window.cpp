#include "filter_window.h"
#include <string>

namespace pb
{
	pb::filter_window::filter_window()
	{
		//  tworzenie elementow GUI
		_fm.move(nana::API::make_center(250, 290));
		_fm.caption("Filtry");
		_fm.bgcolor(nana::colors::antique_white);
		_fm.icon(nana::paint::image("exe_icon.ico"));
		_pl.bind(_fm);
		_cb.create(_fm);
		_cb.caption("Wlasny");
		_cb.push_back("Wlasny");
		_cb.push_back("Dolnoprzepustowy");
		_cb.push_back("Prewitt 0");
		_cb.push_back("Prewitt 45");
		_cb.push_back("Prewitt 90");
		_cb.push_back("Prewitt 135");
		_cb.push_back("Sobel 0");
		_cb.push_back("Sobel 45");
		_cb.push_back("Sobel 90");
		_cb.push_back("Sobel 135");
		_cb.push_back("Sobel X");
		_cb.push_back("Sobel Y");
		_cb.push_back("Laplace 1");
		_cb.push_back("Laplace 2");
		_cb.push_back("Laplace 3");
		_cb.push_back("Narozniki 1");
		_cb.push_back("Narozniki 2");
		_cb.push_back("Narozniki 3");
		_cb.push_back("Narozniki 4");
		_norm_chbox.create(_fm);
		_norm_chbox.caption("Nrm");
		for (int i = 0; i < 9; ++i)
		{
			_sb[i].create(_fm);
			_sb[i].range(-1000, 1000, 1);
			_sb[i].value("0");
		}
		_btn.create(_fm);
		_btn.caption("Zastosuj");


		// rozmieszczenie elementow GUI
		_pl.div("vertical <up weight=20% <filter_list weight=80%><checkbox>><values grid=[3,3] weight=65% margin=[10,30,10,30]><accept_button weight=15%>");
		_pl.field("filter_list") << _cb;
		_pl.field("checkbox") << _norm_chbox;
		for (int i = 0; i < 9; ++i)
		{
			_pl.field("values") << _sb[i];
		}
		_pl.field("accept_button") << _btn;


		// zdarzenia
		_cb.events().text_changed([this]()
		{
			std::string caption = _cb.caption();
			if (caption == "Wlasny")
			{
				for (int i = 0; i < 9; ++i)
					_sb[i].value(std::to_string(0));
			}
			else if (caption == "Dolnoprzepustowy")
			{
				for (int i = 0; i < 9; ++i)
					_sb[i].value(std::to_string(1));
			}
			else if (caption == "Prewitt 0")
			{
				_sb[0].value(std::to_string(-1));
				_sb[1].value(std::to_string(0));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(-1));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(1));
				_sb[6].value(std::to_string(-1));
				_sb[7].value(std::to_string(0));
				_sb[8].value(std::to_string(1));
			}
			else if (caption == "Prewitt 45")
			{
				_sb[0].value(std::to_string(0));
				_sb[1].value(std::to_string(1));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(-1));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(1));
				_sb[6].value(std::to_string(-1));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(0));
			}
			else if (caption == "Prewitt 90")
			{
				_sb[0].value(std::to_string(1));
				_sb[1].value(std::to_string(1));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(0));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(0));
				_sb[6].value(std::to_string(-1));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(-1));
			}
			else if (caption == "Prewitt 135")
			{
				_sb[0].value(std::to_string(1));
				_sb[1].value(std::to_string(1));
				_sb[2].value(std::to_string(0));
				_sb[3].value(std::to_string(1));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(-1));
				_sb[6].value(std::to_string(0));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(-1));
			}
			else if (caption == "Sobel 0")
			{
				_sb[0].value(std::to_string(-1));
				_sb[1].value(std::to_string(0));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(-2));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(2));
				_sb[6].value(std::to_string(-1));
				_sb[7].value(std::to_string(0));
				_sb[8].value(std::to_string(1));
			}
			else if (caption == "Sobel 45")
			{
				_sb[0].value(std::to_string(0));
				_sb[1].value(std::to_string(1));
				_sb[2].value(std::to_string(2));
				_sb[3].value(std::to_string(-1));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(1));
				_sb[6].value(std::to_string(-2));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(0));
			}
			else if (caption == "Sobel 90")
			{
				_sb[0].value(std::to_string(1));
				_sb[1].value(std::to_string(2));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(0));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(0));
				_sb[6].value(std::to_string(-1));
				_sb[7].value(std::to_string(-2));
				_sb[8].value(std::to_string(-1));
			}
			else if (caption == "Sobel 135")
			{
				_sb[0].value(std::to_string(2));
				_sb[1].value(std::to_string(1));
				_sb[2].value(std::to_string(0));
				_sb[3].value(std::to_string(1));
				_sb[4].value(std::to_string(0));
				_sb[5].value(std::to_string(-1));
				_sb[6].value(std::to_string(0));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(-2));
			}
			else if (caption == "Laplace 1")
			{
				_sb[0].value(std::to_string(0));
				_sb[1].value(std::to_string(-1));
				_sb[2].value(std::to_string(0));
				_sb[3].value(std::to_string(-1));
				_sb[4].value(std::to_string(4));
				_sb[5].value(std::to_string(-1));
				_sb[6].value(std::to_string(0));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(0));
			}
			else if (caption == "Laplace 2")
			{
				_sb[0].value(std::to_string(-1));
				_sb[1].value(std::to_string(-1));
				_sb[2].value(std::to_string(-1));
				_sb[3].value(std::to_string(-1));
				_sb[4].value(std::to_string(8));
				_sb[5].value(std::to_string(-1));
				_sb[6].value(std::to_string(-1));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(-1));
			}
			else if (caption == "Laplace 3")
			{
				_sb[0].value(std::to_string(1));
				_sb[1].value(std::to_string(-2));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(-2));
				_sb[4].value(std::to_string(4));
				_sb[5].value(std::to_string(-2));
				_sb[6].value(std::to_string(1));
				_sb[7].value(std::to_string(-2));
				_sb[8].value(std::to_string(1));
			}
			else if (caption == "Narozniki 1")
			{
				_sb[0].value(std::to_string(1));
				_sb[1].value(std::to_string(1));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(1));
				_sb[4].value(std::to_string(-2));
				_sb[5].value(std::to_string(-1));
				_sb[6].value(std::to_string(1));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(-1));
			}
			else if (caption == "Narozniki 2")
			{
				_sb[0].value(std::to_string(1));
				_sb[1].value(std::to_string(1));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(-1));
				_sb[4].value(std::to_string(-2));
				_sb[5].value(std::to_string(1));
				_sb[6].value(std::to_string(-1));
				_sb[7].value(std::to_string(-1));
				_sb[8].value(std::to_string(1));
			}
			else if (caption == "Narozniki 3")
			{
				_sb[0].value(std::to_string(1));
				_sb[1].value(std::to_string(-1));
				_sb[2].value(std::to_string(-1));
				_sb[3].value(std::to_string(1));
				_sb[4].value(std::to_string(-2));
				_sb[5].value(std::to_string(-1));
				_sb[6].value(std::to_string(1));
				_sb[7].value(std::to_string(1));
				_sb[8].value(std::to_string(1));
			}
			else if (caption == "Narozniki 4")
			{
				_sb[0].value(std::to_string(-1));
				_sb[1].value(std::to_string(-1));
				_sb[2].value(std::to_string(1));
				_sb[3].value(std::to_string(-1));
				_sb[4].value(std::to_string(-2));
				_sb[5].value(std::to_string(1));
				_sb[6].value(std::to_string(1));
				_sb[7].value(std::to_string(1));
				_sb[8].value(std::to_string(1));
			}
		});


		_pl.collocate();
		_fm.show();
	}

	bool pb::filter_window::normalization_checkbox_checked()
	{
		if (_norm_chbox.checked())
			return true;
		else
			return false;
	}
	cv::Mat pb::filter_window::values()
	{
		cv::Mat m{ 3, 3, cv::DataType<int>::type };
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				m.at<int>(i, j) = _sb[j + i * 3].to_int();
			}
		}
		return m;
	}
	nana::button& pb::filter_window::accept_button()
	{
		return _btn;
	}
}

/*
else if (caption == "Sobel X")
{
_sb[0].value(std::to_string(1));
_sb[1].value(std::to_string(2));
_sb[2].value(std::to_string(1));
_sb[3].value(std::to_string(0));
_sb[4].value(std::to_string(0));
_sb[5].value(std::to_string(0));
_sb[6].value(std::to_string(-1));
_sb[7].value(std::to_string(-2));
_sb[8].value(std::to_string(-1));
}
else if (caption == "Sobel Y")
{
_sb[0].value(std::to_string(1));
_sb[1].value(std::to_string(0));
_sb[2].value(std::to_string(-1));
_sb[3].value(std::to_string(2));
_sb[4].value(std::to_string(0));
_sb[5].value(std::to_string(-2));
_sb[6].value(std::to_string(1));
_sb[7].value(std::to_string(0));
_sb[8].value(std::to_string(-1));
}*/