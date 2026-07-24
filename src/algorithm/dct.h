#pragma once
#include"compressionalgorithm.h"
#include <opencv2/opencv.hpp>
#include<vector>
#include<stdint.h>

class dct :public CompressionAlgorithm {
public:
    dct() = default;
	cv::Mat compress(const cv::Mat& input) override;
	std::string algorithmname() const override { return "DCT"; }
	double getCompression() const override { return compressratio; }

private:
	double compressratio = 1.0;
};