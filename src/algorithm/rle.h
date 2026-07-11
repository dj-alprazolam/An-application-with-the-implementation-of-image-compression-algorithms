#pragma once
#include"compressionalgorithm.h"
#include<opencv2/opencv.hpp>

class rle:public CompressionAlgorithm {
public:
	cv:Mat compress(const cv::mat& input) override;
	std::string algorithmname() const override { return "RLE"; }
	double getCompression() const override { return compressratio; }

private:
	double compressratio = 1.0;
};