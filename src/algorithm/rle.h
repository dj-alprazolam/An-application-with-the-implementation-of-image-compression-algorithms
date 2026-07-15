#pragma once
#include"compressionalgorithm.h"
#include <opencv2/opencv.hpp>
#include<vector>

class rle:public CompressionAlgorithm {
public:
    rle() = default;
	cv::Mat compress(const cv::Mat& input) override;
	std::string algorithmname() const override { return "RLE"; }
	double getCompression() const override { return compressratio; }

private:
	double compressratio = 1.0;
	struct rleznach
	{
		std::uint16_t count;
		std::uint8_t value;
	}; 
	
	std::vector<rleznach> compressData;
};