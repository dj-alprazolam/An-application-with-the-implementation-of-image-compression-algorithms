#include"compressionalgorithm.h"
#include<opencv2/opencv.hpp>
#include<vector>
#include<string>
#include<cstdint>
#include<map>

class huff:public CompressionAlgorithm {
public:
    huff() = default;
	cv::Mat compress(const cv::Mat& input) override;
	std::string algorithmname() const override { return "huffman"; }
	double getCompression() const override { return compressratio; }

private:
	double compressratio = 1.0;
	
    struct Node
    {
        int byte;
        int left;
        int right;
        int frequenc; 
    };
    
    std::vector<Node> tree;
    std::map<uint8_t, std::string> code;
    std::vector<uint8_t> compressdata;

    void build(int nodeindex, std::string currentcode);
};