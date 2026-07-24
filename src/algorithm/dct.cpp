#include "dct.h"
#include <opencv2/opencv.hpp>

cv::Mat dct::compress(const cv::Mat& input) {
    if (input.empty()) {
        compressratio = 1.0;
        return cv::Mat();
    }

    cv::Mat workImg;
    if (input.channels() == 3) {
        cv::cvtColor(input, workImg, cv::COLOR_BGR2GRAY);
    } else {
        workImg = input.clone();
    }


    cv::Mat floatImg;
    workImg.convertTo(floatImg, CV_32F);

    float qMatrixData[8][8] = {
        {64, 44, 40, 64, 96, 160, 204, 244},
        {48, 48, 56, 76, 104, 232, 240, 220},
        {56, 52, 64, 96, 160, 228, 276, 224},
        {56, 68, 88, 116, 204, 348, 320, 248},
        {72, 88, 148, 224, 272, 436, 412, 308},
        {96, 140, 220, 256, 324, 416, 452, 368},
        {196, 256, 312, 348, 412, 484, 480, 404},
        {288, 368, 380, 392, 448, 400, 412, 396}};
        
    cv::Mat Q(8, 8, CV_32F, qMatrixData);

    int validRows = (workImg.rows / 8) * 8;
    int validCols = (workImg.cols / 8) * 8;

    cv::Mat compressedFloat = cv::Mat::zeros(validRows, validCols, CV_32F);
    int zeroCount = 0; 
    int totalBlocks = (validRows / 8) * (validCols / 8);


    for (int y = 0; y < validRows; y += 8) {
        for (int x = 0; x < validCols; x += 8) {
            cv::Rect roi(x, y, 8, 8);
            cv::Mat block = floatImg(roi);
            cv::Mat dctBlock;
            cv::dct(block, dctBlock);
            cv::Mat quantized;
            cv::divide(dctBlock, Q, quantized);
            quantized.convertTo(quantized, CV_32S); 

            zeroCount += cv::countNonZero(quantized == 0);

            quantized.copyTo(compressedFloat(roi));
        }
    }
    cv::Mat restoredFloat = cv::Mat::zeros(validRows, validCols, CV_32F);
    for (int y = 0; y < validRows; y += 8) {
        for (int x = 0; x < validCols; x += 8) {
            cv::Rect roi(x, y, 8, 8);
            cv::Mat quantizedBlock = compressedFloat(roi);
            cv::Mat idctInput;
            cv::multiply(quantizedBlock, Q, idctInput);
            cv::Mat restoredBlock;
            cv::idct(idctInput, restoredBlock);

            restoredBlock.copyTo(restoredFloat(roi));
        }
    }


    cv::Mat finalResult;
    restoredFloat.convertTo(finalResult, CV_8U);

    if (input.channels() == 3) {
        cv::cvtColor(finalResult, finalResult, cv::COLOR_GRAY2BGR);
    }

    double totalCoeffs = totalBlocks * 64.0;
    double nonZeroCoeffs = totalCoeffs - zeroCount;

    if (nonZeroCoeffs > 0)
    {
        compressratio = nonZeroCoeffs / totalCoeffs;
    }
    else
    {
        compressratio = 0.01; 
    }

    return finalResult;
}