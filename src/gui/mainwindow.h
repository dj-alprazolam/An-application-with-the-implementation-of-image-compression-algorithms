#pragma once

#include<qmainwindow.h>
#include <QApplication>
#include<opencv2/opencv.hpp>
#include<qpushbutton.h>
#include <QLabel>
#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include<memory>
#include"rle.h"
#include"compressionalgorithm.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void openimage();
	void compressimage();
	void saveimage();
	void onAlgorithmChanged(int index);

private:
	void setupUI();
	void displayimage(const cv::Mat& image);
	QImage imageInMat(const cv::Mat& mat);
	void updateAlgorithm();

	QLabel* imageLable;
	QPushButton* openButton;
	QPushButton* compressButton;
	QPushButton* saveButton;
	QComboBox* algorithmCombox;
	QLabel* compressLable;

	cv::Mat originalImage;
	cv::Mat compressedImage;

	std::unique_ptr<CompressionAlgorithm> currentAlgorithm;
};