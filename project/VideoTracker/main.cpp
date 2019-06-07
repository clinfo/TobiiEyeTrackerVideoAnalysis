#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include "StreamCsv.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <sstream>
#include "cmdline.h"
#include "picojson.h"

using namespace cv;
using namespace std;

struct EyetrackingData {
	string name;
	string id;
	vector<double> data_t;
	vector<double> data_x;
	vector<double> data_y;
	double last_t;
	int csv_count;

};

EyetrackingData read_eyetrack_data(const string& filename) {
	ifstream ifs(filename);
	StreamCsv csv(ifs);
	string line;
	EyetrackingData data;
	csv >> endl;
	while (!ifs.eof())
	{
		for (int i = 0; i < 5; i++) {
			csv >> line;
		}
		double t, x, y;
		csv >> t;
		//csv >> line;
		csv >> x;
		csv >> y;
		if (t > 0) {
		data.data_t.push_back(t);
		data.data_x.push_back(x);
		data.data_y.push_back(y);
		data.last_t = t;
		}
		csv >> endl;
		
	}
	return data;
}


int main(int argc, const char* argv[]) {
	// make a new ArgumentParser
	cmdline::parser parser;
	parser.add<string>("data", 'd', "data directory", false, "./data/");
	parser.add<string>("output", '\0', "output video", false, "output.avi");
	parser.add<string>("label", '\0', "label", false, "1_1");
	parser.add<string>("video", 'v', "video", false, "20170421ME0021782_1.avi");
	parser.parse_check(argc, argv);
	
	parser.parse(argc, argv);
	
	//dataset.json
	std::ifstream ifs("dataset_video.json", std::ios::in);
	if (ifs.fail()) {
		std::cerr << "failed to read test.json" << std::endl;
		return 1;
	}
	const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();
	picojson::value v;
	const std::string err = picojson::parse(v, json);
	if (err.empty() == false) {
		std::cerr << err << std::endl;
		return 2;
	}

	// load csv files
	vector<EyetrackingData> dataset;
	namespace sys = std::experimental::filesystem;
	/*
	string path = parser.get<string>("data");
	cout << "[LOAD]" << path << endl;
	//
	sys::path p(path); // 列挙の起点
	std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
		[&](const sys::path& p) {
		if (sys::is_regular_file(p)) { // ファイルなら...
			cout << "file: " << p.filename() << endl;
			stringstream ss;
			ss << path << p.filename();
			EyetrackingData data = read_eyetrack_data(ss.str());
			data.csv_count = 0;
			dataset.push_back(data);
		}
	});
	*/

	picojson::object& obj = v.get<picojson::object>();
	const string target_label= parser.get<string>("label");
	for (auto& p : obj) {
		picojson::object& o = p.second.get<picojson::object>();
		picojson::value v = o["label"];
		string label = "";
		if (v.is<string>()) {
			label = v.get<string>();
		}
		if(label == target_label){
			string filename=p.first;
			sys::path path(filename);
			if (sys::is_regular_file(path)) {
				std::cout << "[LOAD] file: " << p.first << ":" << label << std::endl;
				EyetrackingData data = read_eyetrack_data(filename);
				data.csv_count = 0;
				data.name = filename;
				data.id = o["recording"].get<string>();
				dataset.push_back(data);
			}
			else {
				std::cout << "[SKIP] file: " << p.first << ":" << label << std::endl;

			}
			
		}
	}

	
	ofstream ofs("output.csv");
	ofs << "Time stamp(msec),X(pixel),Y(pixel)" << endl;
	// cv
	{
		cv::VideoWriter writer;
		Mat img,prev_img1, prev_img2,img_view;
		string video_path = parser.get<string>("video");
		VideoCapture cap(video_path);
		int max_frame=cap.get(cv::CAP_PROP_FRAME_COUNT); //フレーム数
		double fps = cap.get(cv::CAP_PROP_FPS);
		cout << "== Movie information =="<< endl;
		cout <<"fps:"<< fps<<endl;
		int cap_width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
		int cap_height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
		cout << "max_frame:" << max_frame << endl;
		int mov_t = max_frame / fps * 1000;
		cout << "movie (ms):" << mov_t << endl;
		for (auto& data : dataset) {
			cout << "first_t:" << data.data_t[0] << endl;
			cout << "last_t:" << data.last_t << endl;
			cout << "csv (ms):" << (data.last_t - data.data_t[0]) << endl;
		}
		//
		int target_x = 0;
		int target_y = 0;
		double movie_x = 0;
		double movie_y = 0;
		for(int i=0; i<max_frame;i++){
			cap >> img; //1フレーム分取り出してimgに保持させる
			img_view = img.clone();
			for (auto& data : dataset) {
				double t = i / fps * 1000 + data.data_t[0];
				double w = img.cols / 1620.0;
				double h = img.rows / 1080.0;
				while (data.data_t[data.csv_count] < t) {
					target_x = (data.data_x[data.csv_count]-150) * w;
					target_y = data.data_y[data.csv_count] * h;
					data.csv_count++;
				}
				cout << data.csv_count << "," << target_x << "," << target_y << "" << endl;
				circle(img_view, Point(target_x, target_y), 10, Scalar(0, 0, 255), 5);//bgr
				cv::putText(img_view, data.id, Point(target_x, target_y-13), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(100, 100, 255), 2);
			}
			{
				stringstream ss;
				int t = i / fps * 1000;
				ss << t;
				cv::putText(img_view, ss.str(), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(100, 100, 100), 2);
			}
			if (i == 0) {
				const string output = parser.get<string>("output");
				writer.open(output, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(cap_width, cap_height));
				//writer.open("outvideo.mp4", VideoWriter::fourcc('M', 'P', '4', 'V'), fps,  cv::Size(cap_width, cap_height));
				//動画ファイルがちゃんと作れたかの判定。
				if (!writer.isOpened()) { return -1; }
			}
			if (i > 1) {
				Mat diff_img,diff_img1, diff_img2, mask,img1,img2, img3, c_img;
				cv::cvtColor(img, img1, cv::COLOR_RGB2GRAY);
				cv::cvtColor(prev_img1, img2, cv::COLOR_RGB2GRAY);
				cv::cvtColor(prev_img2, img3, cv::COLOR_RGB2GRAY);
				cv::absdiff(img1, img2, diff_img1);
				cv::absdiff(img2, img3, diff_img2);
				cv::bitwise_and(diff_img1, diff_img2, diff_img);
				vector<vector<Point> > contours;
				cv::threshold(diff_img, mask, 5, 255, cv::THRESH_BINARY);
				cv::medianBlur(mask, mask, 5);
				cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
				if(contours.size()>0){
					int count = 0;
					movie_x = 0;
					movie_y = 0;
					for (auto& c : contours) {
						for (int k = 0; k < c.size(); k++) {
							movie_x += c.at(k).x;
							movie_y += c.at(k).y;
							count += 1;
						}
					}
					movie_x /= count;
					movie_y /= count;
				}
				circle(img_view, Point(movie_x, movie_y), 10, Scalar(255, 255, 255), 5);//bgr
				int t = i / fps * 1000;
				ofs << t << ",";
				ofs << movie_x << "," << movie_y << std::endl;
				imshow("Video", img_view);
				imshow("Video2", mask);
				prev_img2 = prev_img1.clone();
				prev_img1 = img.clone();
				
			}else{
				imshow("Video", img_view);
				if (i > 0) {
					prev_img2 = prev_img1.clone();
				}
				prev_img1 = img.clone();

			}
			if (writer.isOpened()) {
				writer << img_view;
			}
			waitKey(1); // 表示のために1ms待つ
		}
		return 0;
	}
}

