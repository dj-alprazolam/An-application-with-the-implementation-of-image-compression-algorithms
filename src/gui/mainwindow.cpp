#include"mainwindow.h"
#include<qimage.h>
#include<qfiledialog.h>
#include<qfileinfo.h>
#include<qmessagebox.h>
#include<qpixmap.h>

MainWindow::MainWindow(QWidget* patern):QMainWindow(patern) {
	setupUI();
}

MainWindow::~MainWindow() {

}

void MainWindow::setupUI() {
	setWindowTitle("Compressoe APP");
	setMinimumSize(800, 600);

	auto* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	QVBoxLayout* mainlayout = new QVBoxLayout(centralWidget);

	imageLable = new QLabel("Загрузите приложение");
	imageLable->setStyleSheet("QLabel { "
		"border: 2px dashed #aaa; "
		"border-radius: 10px; "
		"background-color: #f5f5f5; "
		"color: #666; "
		"font-size: 14px; "
		"padding: 20px; "
		"}");
	imageLable->setMinimumHeight(400);
	mainlayout->addWidget(imageLable);

	QHBoxLayout* controllayout = new QHBoxLayout();


	openButton = new QPushButton("Открыть изображение");
	compressButton = new QPushButton("Сжать изображение");
	saveButton = new QPushButton("Сохранить изображение");




	

}
