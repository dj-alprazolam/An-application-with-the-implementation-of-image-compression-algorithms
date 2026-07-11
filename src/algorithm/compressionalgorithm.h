#pragma once

#include<opencv2/opencv.hpp>
#include<string>
// базовый класс для алгоритмов
class CompressionAlgorithm {
public:
	virtual ~CompressionAlgorithm() = default; //  виртуальный деструктор
	virtual cv::Mat compress(const cv::Mat& input) = 0;// виртуальная функция принимает изображение и возвращает сжатое
	virtual std::string algorithmname() const = 0; // виртуальная функция имя алгоритма
	virtual double getCompression() const = 0; // виртуальная функция показывает коэффициент сжатия  

};