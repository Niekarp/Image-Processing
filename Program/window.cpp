#include "window.h"

namespace pb
{
	window::window()
	{
		// tworzenie elementow okna
		_fm.move(nana::API::make_center(500, 300));
		_fm.icon(nana::paint::image("exe_icon.ico"));
		_pl.bind(_fm);
		_mn.create(_fm);
		_path_tb.create(_fm);
		_path_tb_mode = path_tb_mode_t::add_img;
		_lb.create(_fm);
		_sl.create(_fm);
		// _img_dir_path ==> sciezka do domyslnego folderu z dodawanymi obrazami
			//przy dodawaniu nowych obrazow zachodzi:
			//absolutna sciezka do dodawanego obrzu = _img_dir_path + napis wpisany w textbox
		_database.open("database.txt", std::ios::in);
		std::getline(_database, _img_dir_path);
		_database.close();


		// uklad okna
		_pl.div("vertical <height=25 menubox> <path_textbox height=25> <vertical height=25 images_listbox>");
		_pl.field("menubox") << _mn << _sl;
		_pl.field("path_textbox") << _path_tb;
		_pl.field("images_listbox") << _lb.handle();

		// wyglad okna glownego
		_fm.caption("Podstawy Biometrii");
		_fm.bgcolor(nana::colors::light_green);

		// zakladki w menu
		_mn.push_back("Plik");
		_mn.push_back("Edytuj");
		_mn.push_back("Zaznaczenie");


		if (_img_dir_path == "")
			_path_tb.tip_string("podaj sciezke do zdjecia");
		else
			_path_tb.tip_string(_img_dir_path);
		_path_tb.multi_lines(false);


		// wczytywanie obrazow zapisanych w pliku database.txt do pamieci programu
		_database.open("database.txt", std::ios::in);
		std::string line;
		std::getline(_database, line); // pominiecie pierwszej linii (zawiera ona juz wczytana img_dir_path)
		while (_database.eof() == false)
		{
			std::getline(_database, line);
			std::string img_filepath = line;
			std::string img_filename = extract_filename(img_filepath);
			cv::Mat img = cv::imread(img_filepath);
			_lb.add_img(img_filename, img);

			std::string _lb_height_str = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height_str.c_str());
		}
		_database.close();


		// definiowanie zdarzen dla elementow gui
			// zdarzenia menubox'a
			// plik
		_mn.at(0).append("Ustaw folder obrazow", [this](nana::menu::item_proxy& ip)
		{
			_path_tb_mode = path_tb_mode_t::set_directory;
			_path_tb.tip_string("podaj sciezke do folderu obrazow");
			_fm.focus();
		});
		_mn.at(0).append("Zapisz", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			std::string img_filename = _lb.get_image(selected_img_index).first;
			std::string img_filepath = _img_dir_path + img_filename;
			_database.open("database.txt", std::ios::app);
			_database << '\n' + img_filepath;
			_database.close();

			cv::Mat image = _lb.get_image(selected_img_index).second;
			cv::imwrite(img_filepath, image);
		});
		_mn.at(0).append("Zapisz jako", [this](nana::menu::item_proxy& ip)
		{
			_path_tb_mode = path_tb_mode_t::save_image_as;
			_path_tb.caption("");
			_path_tb.tip_string("podaj: sciezka_do_pliku/nazwa_pliku.format");
			_fm.focus();
		});
		_mn.at(0).append("Zamknij otwarte okna", [this](nana::menu::item_proxy& ip)
		{
			cv::destroyAllWindows();
		});
		_mn.at(0).append("Zakoncz", [this](nana::menu::item_proxy& ip)
		{
			_fm.close();
		});
			// edytuj
		_mn.at(1).append("Powieksz", [this](nana::menu::item_proxy& ip)
		{
			_path_tb_mode = path_tb_mode_t::scale_image;
			_path_tb.caption("");
			_path_tb.tip_string("podaj wspolczynnik powiekszenia");
			_fm.focus();
		});
		_mn.at(1).append("Zobacz pixel", [this](nana::menu::item_proxy& ip)
		{
			_path_tb_mode = path_tb_mode_t::show_pixel_info;
			_path_tb.caption("");
			_path_tb.tip_string("wprowadz polozenie pixela: w h");
		});
		_mn.at(1).append("Zmien pixel", [this](nana::menu::item_proxy& ip)
		{
			_path_tb_mode = path_tb_mode_t::change_pixel;
			_path_tb.caption("");
			_path_tb.tip_string("podaj: w h B G R");
		});
		_mn.at(1).append("Zmiana jasnosci", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);

			cv::Mat image = im.second;
			cv::Mat new_image = cv::Mat::zeros(image.rows, image.cols, image.type());
			std::array<unsigned, 256> lookup;
			double factor = ((double)_sl.value() / ((double)_sl.maximum() / 2));
			for (int i = 0; i < 256; ++i)
			{
				lookup[i] = i;
				//auto tmp = std::pow(i, factor);
				auto tmp = 42 * std::log(i);
				if (tmp > 255)		tmp = 255;
				else if (tmp < 0)	tmp = 0;
				lookup[i] = tmp;
			}
			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					new_image.at<cv::Vec3b>(i, j)[0] = lookup[image.at<cv::Vec3b>(i, j)[0]];
					new_image.at<cv::Vec3b>(i, j)[1] = lookup[image.at<cv::Vec3b>(i, j)[1]];
					new_image.at<cv::Vec3b>(i, j)[2] = lookup[image.at<cv::Vec3b>(i, j)[2]];
				}
			}


			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << factor;
			std::string factor_str = stream.str();
			std::string image_filename = im.first;
			std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
			std::string image_extension = image_filename.substr(image_filename_without_extension.size());
			std::string new_img_filename = image_filename_without_extension
				+ "-pow" + factor_str + image_extension;

			_lb.add_img(new_img_filename, new_image);
			std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height.c_str());
			_pl.collocate();
			cv::imshow(new_img_filename, new_image);
		});
		_mn.at(1).append("Skala szarosci");
		auto grayscale_sub_mn = _mn.at(1).create_sub_menu(4);
		grayscale_sub_mn->append("1 kanal", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);
			cv::Mat image = im.second;
			cv::Mat im_gray;
			cv::cvtColor(image, im_gray, cv::COLOR_BGR2GRAY);

			std::string image_filename = im.first;
			std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
			std::string image_extension = image_filename.substr(image_filename_without_extension.size());
			std::string new_img_filename = image_filename_without_extension
				+ "-grayscale-1ch" + image_extension;

			_lb.add_img(new_img_filename, im_gray);
			std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height.c_str());
			_pl.collocate();
			cv::imshow(new_img_filename, im_gray);
		});
		grayscale_sub_mn->append("3 kanaly", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);
			cv::Mat image = im.second;
			cv::Mat tmp;
			cv::cvtColor(image, tmp, cv::COLOR_BGR2GRAY);
			cv::Mat im_gray;
			cv::cvtColor(tmp, im_gray, cv::COLOR_GRAY2BGR);

			std::string image_filename = im.first;
			std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
			std::string image_extension = image_filename.substr(image_filename_without_extension.size());
			std::string new_img_filename = image_filename_without_extension
				+ "-grayscale-3ch" + image_extension;

			_lb.add_img(new_img_filename, im_gray);
			std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height.c_str());
			_pl.collocate();
			cv::imshow(new_img_filename, im_gray);
		});
		grayscale_sub_mn->append("1 -> 3", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);
			cv::Mat image = im.second;
			cv::Mat im_gray;
			cv::cvtColor(image, im_gray, cv::COLOR_GRAY2BGR);

			std::string image_filename = im.first;
			std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
			std::string image_extension = image_filename.substr(image_filename_without_extension.size());
			std::string new_img_filename = image_filename_without_extension
				+ "-grayscale-3ch" + image_extension;

			_lb.add_img(new_img_filename, im_gray);
			std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height.c_str());
			_pl.collocate();
			cv::imshow(new_img_filename, im_gray);
		});
		_mn.at(1).append("Rozciagnij histogram", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);
			cv::Mat image = im.second.clone();
			cv::Mat im_str = cv::Mat::zeros(image.rows, image.cols, image.type());
			int a, b;
			std::stringstream ss;
			ss << _path_tb.caption();
			if (_path_tb.caption() != "")
			{
				ss >> a >> b;
				window::streach(image, 3, im_str, a, b);
			}
			else
			{
				window::streach(image, 3, im_str);
			}

			std::string image_filename = im.first;
			std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
			std::string image_extension = image_filename.substr(image_filename_without_extension.size());
			std::string new_img_filename = image_filename_without_extension
				+ "-str" + image_extension;
			_lb.add_img(new_img_filename, im_str);
			std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height.c_str());
			_pl.collocate();
			cv::imshow(new_img_filename, im_str);
		});
		_mn.at(1).append("Wyrownaj histogram", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);
			cv::Mat image = im.second.clone();
			cv::Mat im_eq = cv::Mat::zeros(image.rows, image.cols, image.type());

			window::equalize(image, im_eq);

			std::string image_filename = im.first;
			std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
			std::string image_extension = image_filename.substr(image_filename_without_extension.size());
			std::string new_img_filename = image_filename_without_extension
				+ "-equ" + image_extension;
			_lb.add_img(new_img_filename, im_eq);
			std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height.c_str());
			_pl.collocate();
			cv::imshow(new_img_filename, im_eq);
		});
		_mn.at(1).append("Binaryzacja");
		auto binarization_sub_mn = _mn.at(1).create_sub_menu(7);
		binarization_sub_mn->append("Globalny reczny prog", [this](nana::menu::item_proxy& ip)
		{
			// implementacja binaryzacji znajduje sie w zdarzeniach textbox'a
			_path_tb_mode = path_tb_mode_t::global_treshold_binarization;
			_path_tb.caption("");
			_path_tb.tip_string("podaj wartosc progu");
			_fm.focus();
		});
		binarization_sub_mn->append("Metoda Otsu", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);
			cv::Mat image = im.second.clone();

			// tworzenie histogramu znormalizowanego
			std::array<double, 256> hist;
			for (int i = 0; i < 256; ++i)
				hist[i] = 0;
			for (int w = 0; w < image.rows; ++w)
			{
				for (int h = 0; h < image.cols; ++h)
				{
					++hist[image.at<uchar>(w, h)];
				}
			}
			double pixels = image.rows * image.cols;
			for (int i = 0; i < 256; ++i)
				hist[i] /= pixels;
			// wyznaczanie wariancji miedzyklasowej "sigm_B" kla kazdej wartosci progu
			std::array<double, 256> sigm_B;
			for (unsigned t = 0; t < 256; ++t)
			{
				double p_ob, p_b;
				p_ob = std::accumulate(hist.begin(), hist.begin() + t, 0.0);
				p_b  = std::accumulate(hist.begin() + (t + 1), hist.end(), 0.0);
				double sigm_ob = 0, sigm_b = 0;
				for (int k = 0; k < t; ++k)
				{
					sigm_ob += (k * hist[k]) / p_ob;
				}
				for (int k = t; k < 256; ++k)
				{
					sigm_b += (k * hist[k]) / p_b;
				}
				sigm_B[t] = p_ob * p_b * std::pow(sigm_ob - sigm_b, 2);
			}
			// odczytanie wartosci maksymalnej wariancji miedzyklasowej
			auto max_sigm_B_it = std::max_element(sigm_B.begin(), sigm_B.end());
			unsigned optimal_treshold = std::distance(sigm_B.begin(), max_sigm_B_it);

			// binaryzacja
			window::treshold_binarization(image, optimal_treshold);

			std::string image_filename = im.first;
			std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
			std::string image_extension = image_filename.substr(image_filename_without_extension.size());
			std::string new_img_filename = image_filename_without_extension
				+ "-otsu" + image_extension;

			_lb.add_img(new_img_filename, image);
			std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
			_pl.modify("images_listbox", _lb_height.c_str());
			_pl.collocate();
			cv::imshow(new_img_filename, image);
		});
		binarization_sub_mn->append("Niblack", [this](nana::menu::item_proxy& ip)
		{
			// implementacja algorytmu niblack'a znajduje sie w zdarzeniach textbox'a
			_path_tb_mode = path_tb_mode_t::local_treshold_binarization_niblack;
			_path_tb.caption("");
			_path_tb.tip_string("podaj: rozmiar_okna parametr_k");
			_fm.focus();
		});
		_mn.at(1).append("Filtracja");
		auto filtration_sub_mn = _mn.at(1).create_sub_menu(8);
		filtration_sub_mn->append("Filtry konwolucyjne", [this](nana::menu::item_proxy& ip)
		{
			pb::filter_window* fw = new pb::filter_window;
			nana::button& accept_btn = fw->accept_button();
			accept_btn.events().click([this, fw]()
			{
				auto selected_img = _lb.selected();
				auto it = selected_img.begin();
				unsigned selected_img_index = it->item;
				pb::img im = _lb.get_image(selected_img_index);
				cv::Mat image = im.second;
				cv::Mat new_image = image.clone();

				// zasadnicza czesc
				cv::Mat kernel = fw->values();
				unsigned window_size = 3;
				unsigned start_w, start_h, end_w, end_h;
				unsigned window_pixels = std::pow(window_size, 2);
				start_w = 0 + std::floor(window_size / 2.0);
				start_h = start_w;
				end_w = new_image.rows - start_w;
				end_h = new_image.cols - start_h;
				int new_pixel_value_ch[3];
				for (int w = start_w; w < end_w; ++w)
				{
					for (int h = start_h; h < end_h; ++h)
					{
						for (int i = 0; i < 3; ++i)
						{
							new_pixel_value_ch[i] = 0;
						}
						for (int window_w = w - start_w, i = 0; window_w < w + start_w + 1; ++window_w, ++i)
						{
							for (int window_h = h - start_h, j = 0; window_h < h + start_h + 1; ++window_h, ++j)
							{
								for (int k = 0; k < 3; ++k)
								{
									new_pixel_value_ch[k] += kernel.at<int>(i, j) * image.at<cv::Vec3b>(window_w, window_h)[k];
								}
							}
						}
						for (int i = 0; i < 3; ++i)
						{
							if(fw->normalization_checkbox_checked())
								new_pixel_value_ch[i] /= window_pixels;

							if (new_pixel_value_ch[i] > 255)
								new_pixel_value_ch[i] = 255;
							else if (new_pixel_value_ch[i] < 0)
								new_pixel_value_ch[i] = 0;

							new_image.at<cv::Vec3b>(w, h)[i] = new_pixel_value_ch[i];
						}
					}
				}


				std::string new_image_filename = add_to_filename(im.first, "-filtr");
				add_image_to_tmp(new_image_filename, new_image, true);
			});
		});
		filtration_sub_mn->append("Kuwahar 5x5", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			auto it = selected_img.begin();
			unsigned selected_img_index = it->item;
			pb::img im = _lb.get_image(selected_img_index);
			cv::Mat image = im.second;
			cv::Mat new_image = image.clone();

			unsigned window_size = 5;
			unsigned start_w, start_h, end_w, end_h;
			unsigned window_pixels = std::pow(window_size, 2);
			start_w = 0 + std::floor(window_size / 2.0);
			start_h = start_w;
			end_w = new_image.rows - start_w;
			end_h = new_image.cols - start_h;
			for (int w = start_w; w < end_w; ++w)
			{
				for (int h = start_h; h < end_h; ++h)
				{
					std::array<int, 4> mean;
					std::array<int, 4> std_dev;
					for (int i = 0; i < 4; ++i)
					{
						mean[i] = 0;
						std_dev[i] = 0;
					}
					for (int index = 0; index < 4; ++index)
					{
						int start_reg_x = w - start_w + (index / 2) * 2;
						int start_reg_y = h - start_h + (index % 2) * 2;

						for (int x = start_reg_x; x < start_reg_x + 3; ++x)
						{
							for (int y = start_reg_y; y < start_reg_y + 3; ++y)
							{
								mean[index] += image.at<uchar>(x, y);
							}
						}
						mean[index] /= 9;

						for (int x = start_reg_x; x < start_reg_x + 3; ++x)
						{
							for (int y = start_reg_y; y < start_reg_y + 3; ++y)
							{
								std_dev[index] += std::pow(image.at<uchar>(x, y) - mean[index], 2);
							}
						}
						std_dev[index] /= 9;
						std_dev[index] = std::sqrt(std_dev[index]);
					}
					auto min_std_dev = std::min_element(std_dev.begin(), std_dev.end());
					int selected_mean = mean[min_std_dev._Idx];
					new_image.at<uchar>(w, h) = selected_mean;
				}
			}

			std::string new_image_filename = add_to_filename(im.first, "-Kuwahara");
			add_image_to_tmp(new_image_filename, new_image, true);
		});
		filtration_sub_mn->append("Medianowy", [this](nana::menu::item_proxy& ip)
		{
			// implementacja filtru medianowego znajduje sie w zdarzeniach textbox'a
			_path_tb_mode = path_tb_mode_t::median_filter;
			_path_tb.caption("");
			_path_tb.tip_string("3x3 czy 5x5");
			_fm.focus();
		});
		_mn.at(2).append("Rysuj", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			for (auto it = selected_img.begin(); it != selected_img.end(); ++it)
			{
				unsigned selected_img_index = it->item;
				pb::img im = _lb.get_image(selected_img_index);
				cv::imshow(im.first, im.second);
			}
		});
		_mn.at(2).append("Histogram");
		auto histogram_sub_mn = _mn.at(2).create_sub_menu(1);
		histogram_sub_mn->append("Histogram - grayscale", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			for (auto it = selected_img.begin(); it != selected_img.end(); ++it)
			{
				unsigned selected_img_index = it->item;
				pb::img im = _lb.get_image(selected_img_index);
				cv::Mat image = im.second;
				cv::Mat hist{ 250, 350, CV_8UC3, cv::Scalar(0) };
				unsigned x, y;
				x = 50;
				y = 200;
				histogram(x, y, image, 0, hist, true);

				std::string image_filename = im.first;
				std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
				std::string image_extension = image_filename.substr(image_filename_without_extension.size());
				std::string new_img_filename = image_filename_without_extension
					+ "-grayscale" + image_extension;
				cv::imshow(new_img_filename, hist);
				cv::imshow(im.first, image);
			}
		});
		histogram_sub_mn->append("Histogram - BGR", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			for (auto it = selected_img.begin(); it != selected_img.end(); ++it)
			{
				unsigned selected_img_index = it->item;
				pb::img im = _lb.get_image(selected_img_index);
				cv::Mat image = im.second;
				cv::Mat hist{ 220 * 3, 350, CV_8UC3, cv::Scalar(0, 0, 0) };
				unsigned x, y;
				x = 50;
				y = 200;
				histogram(x, y, image, 0, hist);
				y = 400;
				histogram(x, y, image, 1, hist);
				y = 600;
				histogram(x, y, image, 2, hist);



				std::string image_filename = im.first;
				std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
				std::string image_extension = image_filename.substr(image_filename_without_extension.size());
				std::string new_img_filename = image_filename_without_extension
					+ "-BGR" + image_extension;
				cv::imshow(new_img_filename, hist);
				cv::imshow(im.first, image);
			}
		});
		histogram_sub_mn->append("Histogram - B+G+R", [this](nana::menu::item_proxy& ip)
		{
			auto selected_img = _lb.selected();
			for (auto it = selected_img.begin(); it != selected_img.end(); ++it)
			{
				unsigned selected_img_index = it->item;
				pb::img im = _lb.get_image(selected_img_index);
				cv::Mat image = im.second;
				cv::Mat hist{ 250 , 350, CV_8UC3, cv::Scalar(0, 0, 0) };
				unsigned x, y;
				x = 50;
				y = 200;
				histogram(x, y, image, 4, hist);
				cv::imshow("histogram - B+G+R", hist);
				cv::imshow(im.first, image);
			}
		});
		//zdarzenia slider'a
		_sl.scheme().vernier_text_margin = 0;
		_sl.vernier([](unsigned maximum, unsigned cursor_value)
		{
			return std::to_string(cursor_value * 100 / maximum) + "%";
		});
		// zdarzenia textbox'a
		_path_tb.events().key_press([this](const nana::arg_keyboard& ak)
		{
			wchar_t pressed_key = ak.key;
			if (pressed_key == key_enter) // enter zostal wcisniety
			{
				std::string path_tb_content = _path_tb.caption();
				// ustawianie sciezki do domyslnego folderu z obrazami
				if (_path_tb_mode == path_tb_mode_t::set_directory)
				{
					_path_tb_mode = path_tb_mode_t::add_img;
					_img_dir_path = path_tb_content;
					_database.open("database.txt", std::ios::out | std::ios::trunc);
					_database << _img_dir_path;
					_database.close();
					if (_img_dir_path != "")
						_path_tb.tip_string(_img_dir_path);
					else
						_path_tb.tip_string("podaj sciezke do zdjecia");
					_fm.focus();
					_path_tb.caption("");
				}
				else if (_path_tb_mode == path_tb_mode_t::add_img)
				{
					std::string tb_caption = path_tb_content;
					std::string img_filepath;
					std::string img_filename;
					if (_img_dir_path == "")
					{
						img_filepath = tb_caption;
						img_filename = extract_filename(img_filepath);
					}
					else
					{
						img_filename = tb_caption;
						img_filepath = _img_dir_path + img_filename;
					}
					_database.open("database.txt", std::ios::app);
					_database << '\n' + img_filepath;
					_database.close();

					cv::Mat img = cv::imread(img_filepath);
					_lb.add_img(img_filename, img);
					std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
					_pl.modify("images_listbox", _lb_height.c_str());
					_path_tb.caption("");
				}
				else if (_path_tb_mode == path_tb_mode_t::show_pixel_info)
				{
					unsigned w, h;
					std::stringstream coords(path_tb_content);
					coords >> w >> h;
					auto selected_img = _lb.selected();
					auto it = selected_img.begin();
					unsigned selected_img_index = it->item;
					cv::Mat image = _lb.get_image(selected_img_index).second;
					unsigned R, G, B;
					B = image.at<cv::Vec3b>(w, h)[0];
					G = image.at<cv::Vec3b>(w, h)[1];
					R = image.at<cv::Vec3b>(w, h)[2];
					_path_tb.caption("Pixel[" + std::to_string(w) + ", " + std::to_string(h) + "] => "
						+ "[R: " + std::to_string(R) + ", G: " + std::to_string(G)
						+ ", B: " + std::to_string(B) + "]");

					_path_tb_mode = path_tb_mode_t::add_img;
					_path_tb.tip_string(_img_dir_path);
					_fm.focus();
				}
				else if (_path_tb_mode == path_tb_mode_t::change_pixel)
				{
					unsigned w, h;
					std::stringstream whBGR(path_tb_content);
					whBGR >> w >> h;
					auto selected_img = _lb.selected();
					auto it = selected_img.begin();
					unsigned selected_img_index = it->item;
					cv::Mat image = _lb.get_image(selected_img_index).second;
					unsigned B, G, R;
					whBGR >> B >> G >> R;
					image.at<cv::Vec3b>(w, h)[0] = B;
					image.at<cv::Vec3b>(w, h)[1] = G;
					image.at<cv::Vec3b>(w, h)[2] = R;
					_path_tb.caption("zmieniono");

					std::string image_filename = _lb.get_image(selected_img_index).first;
					std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
					std::string image_extension = image_filename.substr(image_filename_without_extension.size());
					std::string new_img_filename = image_filename_without_extension
						+ "-zmieniony" + image_extension;
					_lb.add_img(new_img_filename, image);
					std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
					_pl.modify("images_listbox", _lb_height.c_str());

					_path_tb_mode = path_tb_mode_t::add_img;
					_path_tb.tip_string(_img_dir_path);
					_fm.focus();
				}
				else if (_path_tb_mode == path_tb_mode_t::scale_image)
				{
					unsigned scale_factor = std::stoi(path_tb_content);

					auto selected_img = _lb.selected();
					auto it = selected_img.begin();
					unsigned selected_img_index = it->item;

					cv::Mat image = _lb.get_image(selected_img_index).second;
					cv::Mat new_image = cv::Mat::zeros(image.rows * scale_factor, image.cols * scale_factor, image.type());
					// algorytm pixel replication
					for (int i = 0; i < image.rows; ++i)
					{
						for (int x = 0; x < scale_factor; ++x)
						{
							for (int j = 0; j < image.cols; ++j)
							{
								for (int y = 0; y < scale_factor; ++y)
								{
									new_image.at<cv::Vec3b>(i * scale_factor + x, j * scale_factor + y) = image.at<cv::Vec3b>(i, j);
								}
							}
							std::cout << std::endl;
						}
					}
					std::string image_filename = _lb.get_image(selected_img_index).first;
					std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
					std::string image_extension = image_filename.substr(image_filename_without_extension.size());
					std::string new_img_filename = image_filename_without_extension
						+ "-x" + std::to_string(scale_factor) + image_extension;
					_lb.add_img(new_img_filename, new_image);
					std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
					_pl.modify("images_listbox", _lb_height.c_str());
					cv::imshow(new_img_filename, new_image);

					_path_tb_mode == path_tb_mode_t::add_img;
					_path_tb.caption("");
					if (_img_dir_path != "")
						_path_tb.tip_string(_img_dir_path);
					else
						_path_tb.tip_string("podaj sciezke do zdjecia");
					_fm.focus();
				}
				else if (_path_tb_mode == path_tb_mode_t::save_image_as)
				{
					auto selected_img = _lb.selected();
					auto it = selected_img.begin();
					unsigned selected_img_index = it->item;
					cv::Mat image = _lb.get_image(selected_img_index).second;
					std::string img_filepath = path_tb_content;
					cv::imwrite(img_filepath, image);

					_path_tb_mode == path_tb_mode_t::add_img;
					_path_tb.caption("");
					if (_img_dir_path != "")
						_path_tb.tip_string(_img_dir_path);
					else
						_path_tb.tip_string("podaj sciezke do zdjecia");
					_fm.focus();
				}
				else if (_path_tb_mode == path_tb_mode_t::global_treshold_binarization)
				{
					auto selected_img = _lb.selected();
					auto it = selected_img.begin();
					unsigned selected_img_index = it->item;
					pb::img im = _lb.get_image(selected_img_index);
					cv::Mat image = im.second.clone();
					
					unsigned treshold = std::stoi(path_tb_content);
					window::treshold_binarization(image, treshold);

					std::string image_filename = im.first;
					std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
					std::string image_extension = image_filename.substr(image_filename_without_extension.size());
					std::string new_img_filename = image_filename_without_extension
						+ "-global_treshold" + image_extension;

					_lb.add_img(new_img_filename, image);
					std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
					_pl.modify("images_listbox", _lb_height.c_str());
					_pl.collocate();
					cv::imshow(new_img_filename, image);


					_path_tb_mode == path_tb_mode_t::add_img;
					_path_tb.caption("");
					if (_img_dir_path != "")
						_path_tb.tip_string(_img_dir_path);
					else
						_path_tb.tip_string("podaj sciezke do zdjecia");
					_fm.focus();
				}
				else if (_path_tb_mode == path_tb_mode_t::local_treshold_binarization_niblack)
				{
					auto selected_img = _lb.selected();
					auto it = selected_img.begin();
					unsigned selected_img_index = it->item;
					pb::img im = _lb.get_image(selected_img_index);
					cv::Mat image = im.second.clone();

					std::stringstream textbox_content(path_tb_content);
					int window_size, k;
					textbox_content >> window_size >> k;

					// niblack
					unsigned start_w, start_h, end_w, end_h;
					unsigned window_pixels = std::pow(window_size, 2);
					start_w = 0 + std::floor(window_size / 2.0);
					start_h = start_w;
					end_w = image.rows - start_w;
					end_h = image.cols - start_h;
					for (int w = start_w; w < end_w; ++w)
					{
						for (int h = start_h; h < end_h; ++h)
						{
							double mean = 0;
							for (int window_w = w - start_w; window_w < w + start_w + 1; ++window_w)
							{
								for (int window_h = h - start_h; window_h < h + start_h + 1; ++window_h)
								{
									mean += image.at<uchar>(window_w, window_h);
								}
							}
							mean /= window_pixels;

							double std_dev = 0;
							for (int window_w = w - start_w; window_w < w + start_w + 1; ++window_w)
							{
								for (int window_h = h - start_h; window_h < h + start_h + 1; ++window_h)
								{
									std_dev += std::pow(image.at<uchar>(window_w, window_h) - mean, 2);
								}
							}
							std_dev /= window_pixels;
							std_dev = std::sqrt(std_dev);

							unsigned treshold = mean + k * std_dev;
							uchar pixel = image.at<uchar>(w, h);
							if (pixel < treshold)
								image.at<uchar>(w, h) = 0;
							else
								image.at<uchar>(w, h) = 255;
						}
					}


					std::string image_filename = im.first;
					std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
					std::string image_extension = image_filename.substr(image_filename_without_extension.size());
					std::string new_img_filename = image_filename_without_extension
						+ "-niblack" + image_extension;

					_lb.add_img(new_img_filename, image);
					std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
					_pl.modify("images_listbox", _lb_height.c_str());
					_pl.collocate();
					cv::imshow(new_img_filename, image);


					_path_tb_mode == path_tb_mode_t::add_img;
					_path_tb.caption("");
					if (_img_dir_path != "")
						_path_tb.tip_string(_img_dir_path);
					else
						_path_tb.tip_string("podaj sciezke do zdjecia");
					_fm.focus();
				}
				else if (_path_tb_mode == path_tb_mode_t::median_filter)
				{
					auto selected_img = _lb.selected();
					auto it = selected_img.begin();
					unsigned selected_img_index = it->item;
					pb::img im = _lb.get_image(selected_img_index);
					cv::Mat image = im.second;
					cv::Mat new_image = im.second.clone();

					std::stringstream textbox_content(path_tb_content);
					int window_size;
					textbox_content >> window_size;

					// filtr medianowy
					unsigned start_w, start_h, end_w, end_h;
					unsigned window_pixels = std::pow(window_size, 2);
					start_w = 0 + std::floor(window_size / 2.0);
					start_h = start_w;
					end_w = image.rows - start_w;
					end_h = image.cols - start_h;
					for (int w = start_w; w < end_w; ++w)
					{
						for (int h = start_h; h < end_h; ++h)
						{
							std::vector<uchar> pixels;
							pixels.clear();
							for (int window_w = w - start_w; window_w < w + start_w + 1; ++window_w)
							{
								for (int window_h = h - start_h; window_h < h + start_h + 1; ++window_h)
								{
									uchar value = image.at<uchar>(window_w, window_h);
									pixels.push_back(value);
								}
							}
							std::sort(pixels.begin(), pixels.end());
							unsigned mid_index;
							if (window_size == 3)
								mid_index = 4;
							else if (window_size == 5)
								mid_index = 12;
							
							uchar median = pixels.at(mid_index);
							new_image.at<uchar>(w, h) = median;
						}
					}


					std::string new_image_filename = add_to_filename(im.first, "-filtr medianowy");
					add_image_to_tmp(new_image_filename, new_image, true);

					change_path_tb_mode(path_tb_mode_t::add_img);
				}
			}
			// zmiana focusu z textbox'a do listbox'a po wcisnieciu esc
			else if (pressed_key == key_esc)
			{
				_lb.focus();
			}
			_pl.collocate();
		});
		// zdarzenia labelbox'a
		_lb.events().key_press([this](const nana::arg_keyboard& ak)
		{
			wchar_t pressed_key = ak.key;
			// uwuswanie zdjecia z programu
			if (pressed_key == 127) // del zostal wcisniety
			{
				auto selected_images = _lb.selected();
				_database.open("database.txt", std::ios::in);
				std::vector<std::string> tmp;
				std::string line;
				std::getline(_database, line);
				tmp.push_back(line);
				int i = 0;
				auto it = selected_images.begin();
				while (std::getline(_database, line))
				{
					if (i != it->item)
					{
						tmp.push_back(line);
					}
					if (it != (selected_images.end() - 1))
						++it;
					++i;
				}
				_database.close();
				_database.open("database.txt", std::ios::out);
				for (auto ii = tmp.begin(); ii != tmp.end(); ++ii)
				{
					_database << ii->c_str();
					if (ii != (tmp.end() - 1))
						_database << std::endl;
				}
				_database.close();

				// dopasowanie rozmiaru listbox'a po usunieciu obrazu
				_lb.del_selected();
				std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
				_pl.modify("images_listbox", _lb_height.c_str());
			}
			_pl.collocate();
		});

		_pl.collocate();
		_fm.show();
	}
	void window::histogram(unsigned x, unsigned y, cv::Mat image, unsigned channel, cv::Mat& hist, bool grayscale)
	{
		std::array<unsigned, 256> pixels;
		for (int i = 0; i < 256; ++i)
			pixels[i] = 0;
		for (int i = 0; i < image.rows; ++i)
		{
			for (int j = 0; j < image.cols; ++j)
			{
				if (channel == 4)
				{
					unsigned avg = (image.at<cv::Vec3b>(i, j)[0] + image.at<cv::Vec3b>(i, j)[1] + image.at<cv::Vec3b>(i, j)[2]) / 3;
					++pixels[avg];
				}
				else
					++pixels[image.at<cv::Vec3b>(i, j)[channel]];
			}
		}
		unsigned max_pixels = *std::max_element(pixels.begin(), pixels.end());
		//cv::Mat hist{ 270, 350, CV_8UC3, cv::Scalar(0, 0, 0) };
		float scale = 1.5;
		for (int j = y - 10; j <= y + 10; ++j)
		{
			hist.at<cv::Vec3b>(j, x - 1)[0] = 255;
			hist.at<cv::Vec3b>(j, x - 1)[1] = 255;
			hist.at<cv::Vec3b>(j, x - 1)[2] = 255;
		}
		for (int j = y - 10; j <= y + 10; ++j)
		{
			hist.at<cv::Vec3b>(j, (x + 256))[0] = 255;
			hist.at<cv::Vec3b>(j, (x + 256))[1] = 255;
			hist.at<cv::Vec3b>(j, (x + 256))[2] = 255;
		}

		unsigned max_col_size = 1 * 100 * scale;
		for (int i = x; i < (x + 256); ++i)
		{
			hist.at<cv::Vec3b>(y - max_col_size, i)[0] = 255;
			hist.at<cv::Vec3b>(y - max_col_size, i)[1] = 255;
			hist.at<cv::Vec3b>(y - max_col_size, i)[2] = 255;
			unsigned col_size = ((float)pixels[i - x] / max_pixels) * 100 * scale;
			for (int j = y; j > (y - col_size); --j)
			{
				if (channel == 4 || grayscale)
				{
					hist.at<cv::Vec3b>(j, i)[0] = 255;
					hist.at<cv::Vec3b>(j, i)[1] = 255;
					hist.at<cv::Vec3b>(j, i)[2] = 255;
				}
				else
					hist.at<cv::Vec3b>(j, i)[channel] = 255;
			}
		}
	}
	void window::streach(cv::Mat image, unsigned channels, cv::Mat& out, int a, int b)
	{
		for (int channel = 0; channel < channels; ++channel)
		{
			double min = image.at<cv::Vec3b>(0, 0)[channel];
			double max = image.at<cv::Vec3b>(0, 0)[channel];
			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					auto val = image.at<cv::Vec3b>(i, j)[channel];
					if (val < a)
					{
						image.at<cv::Vec3b>(i, j)[channel] = 0;
						continue;
					}
					else if (val > b)
					{
						image.at<cv::Vec3b>(i, j)[channel] = 255;
						continue;
					}

					if (val > max && val < 255)
						max = val;
					else if (val < min && val > 0)
						min = val;
				}
			}
			std::array<int, 256> lookup;
			double k_min, k_max, zk;
			k_min = min;
			k_max = max;
			zk = 256;
			for (int i = 0; i < 256; ++i)
			{
				lookup[i] = i;
				lookup[i] = ((lookup[i] - k_min) / (k_max - k_min)) * zk;
				if (lookup[i] < 0) lookup[i] = 0;
				else if (lookup[i] > 255) lookup[i] = 255;
			}
			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					out.at<cv::Vec3b>(i, j)[channel] = lookup[image.at<cv::Vec3b>(i, j)[channel]];
				}
			}
		}
	}
	void window::equalize(cv::Mat image, cv::Mat& out)
	{
		for (int channel = 0; channel < 3; ++channel)
		{
			std::array<unsigned, 256> hist;
			for (int i = 0; i < 256; ++i)
				hist[i] = 0;
			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					++hist[image.at<cv::Vec3b>(i, j)[channel]];
				}
			}

			std::array<double, 256> S;
			double total_pixels = image.rows * image.cols;
			for (int k = 0; k < 256; ++k)
			{
				S[k] = hist[k] / total_pixels;
				for (int i = k - 1; i >= 0; --i)
				{
					S[k] += hist[i] / total_pixels;
				}
			}

			std::array<unsigned, 256> lookup;
			unsigned m = 256;
			for (int k = 0; k < 256; ++k)
			{
				lookup[k] = (S[k] - S[0]) / (1 - S[0]) * (256 - 1);
			}

			for (int i = 0; i < image.rows; ++i)
			{
				for (int j = 0; j < image.cols; ++j)
				{
					out.at<cv::Vec3b>(i, j)[channel] = lookup[image.at<cv::Vec3b>(i, j)[channel]];
				}
			}
		}
	}
	void window::treshold_binarization(cv::Mat& image, unsigned treshold)
	{
		for (int w = 0; w < image.rows; ++w)
		{
			for (int h = 0; h < image.cols; ++h)
			{
				uchar pixel = image.at<uchar>(w, h);
				if (pixel < treshold)
					image.at<uchar>(w, h) = 0;
				else
					image.at<uchar>(w, h) = 255;
			}
		}
	}
	std::string window::extract_filename(std::string filepath)
	{
		// funkcja pomocnicza do wydobywania nazwy pliku z sciezki absolutnej do niego
		std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);
		return filename;
	}
	std::string window::add_to_filename(std::string filename, std::string addition)
	{
		std::string image_filename = filename;
		std::string image_filename_without_extension = image_filename.substr(0, image_filename.find("."));
		std::string image_extension = image_filename.substr(image_filename_without_extension.size());
		std::string new_img_filename = image_filename_without_extension + addition + image_extension;

		return new_img_filename;
	}
	void window::add_image_to_tmp(std::string image_filename, cv::Mat image, bool show)
	{
		_lb.add_img(image_filename, image);
		std::string _lb_height = "height=" + std::to_string((_lb.size() + 1) * 26);
		_pl.modify("images_listbox", _lb_height.c_str());
		_pl.collocate();
		if (show == true)
			cv::imshow(image_filename, image);
	}
	void window::change_path_tb_mode(pb::window::path_tb_mode_t new_mode)
	{
		if (new_mode == path_tb_mode_t::add_img)
		{
			_path_tb_mode == path_tb_mode_t::add_img;
			_path_tb.caption("");
			if (_img_dir_path != "")
				_path_tb.tip_string(_img_dir_path);
			else
				_path_tb.tip_string("podaj sciezke do zdjecia");
			_fm.focus();
		}
	}
}