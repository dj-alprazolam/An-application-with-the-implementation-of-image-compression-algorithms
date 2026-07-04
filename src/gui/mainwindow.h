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

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *patern = nullptr);
	~MainWindow();

private slots:
	void openimage();
	void compressimage();
	void saveimage();

private:
	void setupUI();
	void displayimage(const cv::Mat& image);
	void imageInMat(const cv::Mat& mat);

	QLabel* imageLable;
	QPushButton* openButton;
	QPushButton* compressButton;
	QPushButton* saveButton;
	QComboBox* algorithmCombox;
	QLabel* compressLable;

	cv::Mat originalImage;
	cv::Mat compressedImage;
};