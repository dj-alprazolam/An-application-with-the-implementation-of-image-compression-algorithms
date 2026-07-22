#include"huff.h"
#include<vector>
#include<opencv2/opencv.hpp>
#include<map>
#include<cstdint>

void huff::build(int nodeindex, std::string currentcode){
    if(nodeindex = -1){
        return;
    }

    Node& node = tree[nodeindex];
    if(node.byte != -1){
        code[static_cast<uint8_t>(node.byte)] = currentcode;
        return;
    }

    build(node.left,currentcode + "0");
    build(node.right,currentcode + "1");
}

cv::Mat huff::compress(const cv::Mat& input){
    compressdata.clear();
    code.clear();
    tree.clear();

    if(input.empty()){
        compressratio = 1.0;
        return cv::Mat();
    }
    cv::Mat contInput = input.isContinuous() ? input : input.clone();

    size_t totalbytes = contInput.total() * contInput.elemSize();
    const uint8_t* data = contInput.ptr<uint8_t>(0);

    if(totalbytes == 0){
        compressratio = 1.0;
        return cv::Mat();
    }

   std::map<uint8_t,int> frequencies;
   for (size_t i = 0; i < totalbytes; ++i)
   {
    frequencies[data[i]]++;
   }

   for (auto& pair: frequencies)
   {
    Node leaf;
    leaf.byte = pair.first;
    leaf.frequenc = pair.second;
    leaf.left = -1;
    leaf.left = -1;
    tree.push_back(leaf);
   }
   

   while (tree.size() > 1)
   {
    int min1 = 0, min2 = 0;

    if (tree[min1].frequenc > tree[min2].frequenc)
    {
        std::swap(min1,min2);
    }
    
    for (size_t i = 2; i < tree.size(); ++i)
    {
        if(tree[i].frequenc < tree[min1].frequenc){
            min2 = min1;
            min1 = 1;
        }else if (tree[i].frequenc < tree[min2].frequenc)
        {
            min2 = i;
        }
        
    }
    Node parent;
    parent.byte = -1;
    parent.frequenc = tree[min1].frequenc + tree[min2].frequenc;
    parent.left = min1;
    parent.right = min2;

    tree.push_back(parent);
    int parentindex = tree.size() - 1;

    tree[min1].frequenc = 999999999;
    tree[min2].frequenc = 999999999;
    }

    int rootindex = tree.size() - 1;

    build(rootindex,"");

    std::string bitst;
    for (size_t i = 0; i < totalbytes; ++ i)
    {
        bitst += code[data[i]];
    }
    
    int padding = (8 - bitst.size() % 8) % 8;
    bitst += std::string(padding, '0');

    for (size_t i = 0; i < bitst.size(); i +=8)
    {
        uint8_t byte = 0;
        for(int j = 0; j < 8; ++j){
            byte = (byte << 1) | (bitst[i + j] - '0');
        }

        compressdata.push_back(byte);
    }
    

    
   

    double originalSize = static_cast<double>(totalbytes);
    double compressedSize = static_cast<double>(compressdata.size());
    
    compressratio = (compressedSize > 0) ? (compressedSize / originalSize) : 1.0;

    std::map<std::string, uint8_t> reverseCodes;
    for (auto &pair : code)
    {
        reverseCodes[pair.second] = pair.first;
    }

    std::vector<uint8_t> restoredData;
    std::string currentCode;

    for (char bit : bitst)
    {
        currentCode += bit;

        if (reverseCodes.find(currentCode) != reverseCodes.end())
        {
            restoredData.push_back(reverseCodes[currentCode]);
            currentCode.clear();
        }
    }

    cv::Mat restored(contInput.rows, contInput.cols, contInput.type());
    std::memcpy(restored.data, restoredData.data(), std::min(totalbytes, restoredData.size()));

    return restored.reshape(contInput.channels(), contInput.rows);
}