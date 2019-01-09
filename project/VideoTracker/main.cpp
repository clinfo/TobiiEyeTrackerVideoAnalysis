#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include "StreamCsv.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <sstream>
#include "cmdline.h"

using namespace cv;
using namespace std;

struct EyetrackingData {
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
		for (int i = 0; i < 19; i++) {
			csv >> line;
		}
		double t, x, y;
		csv >> t;
		csv >> line;
		csv >> x;
		csv >> y;
		data.data_t.push_back(t);
		data.data_x.push_back(x);
		data.data_y.push_back(y);
		csv >> endl;
		data.last_t = t;
	}
	return data;
}


int main(int argc, const char* argv[]){
	// make a new ArgumentParser
	cmdline::parser parser;
	parser.add<string>("data", 'd', "data directory", false, "./data/");
	parser.add<string>("video", 'v', "video", false,"test.avi");
	parser.parse_check(argc, argv);

	parser.parse(argc, argv);
	// parse the command-line arguments - throws if invalid format
	string path = parser.get<string>("data");

	// load csv files
	cout << "[LOAD]" << path << endl;
	namespace sys = std::experimental::filesystem;
	vector<EyetrackingData> dataset;
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

	// cv
	{
		Mat img,prev_img1, prev_img2,img_view;
		string video_path = parser.get<string>("video");
		VideoCapture cap(video_path);
		int max_frame=cap.get(cv::CAP_PROP_FRAME_COUNT); //フレーム数
		double fps = cap.get(cv::CAP_PROP_FPS);
		cout <<"fps:"<< fps<<endl;
		for (auto& data : dataset) {
			cout << "first_t:" << data.data_t[0] << endl;
			cout << "last_t:" << data.last_t << endl;
			cout << "max_frame:" << max_frame << endl;
			//
			int mov_t = max_frame / fps * 1000;
			cout << "movie (ms):" << mov_t << endl;
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
				imshow("Video", img_view);
				prev_img2 = prev_img1.clone();
				prev_img1 = img.clone();
				
			}else{
				imshow("Video", img_view);
				if (i > 0) {
					prev_img2 = prev_img1.clone();
				}
				prev_img1 = img.clone();

			}
			waitKey(1); // 表示のために1ms待つ
		}
		return 0;
	}
}

