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

	imageLable = new QLabel("Загрузите изображение");
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
	openButton->setMinimumHeight(40);
	connect(openButton, &QPushButton::clicked, this, &MainWindow::openimage);
	controllayout->addWidget(openButton);

	compressButton = new QPushButton("Сжать изображение");
	compressButton->setMinimumHeight(40);
	compressButton->setEnabled(false);
	connect(compressButton, &QPushButton::clicked, this, &MainWindow::compressimage);
	controllayout->addWidget(compressButton);


	saveButton = new QPushButton("Сохранить изображение");
	saveButton->setMinimumHeight(40);
	connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveimage);
	saveButton->setEnabled(false);
	controllayout->addWidget(saveButton);

	//выбор алгоритмов 
	algorithmCombox = new QComboBox();
	algorithmCombox->addItem("RLE");
	algorithmCombox->addItem("Huffman");
	algorithmCombox->addItem("LWZ");
	algorithmCombox->addItem("DCT");
	algorithmCombox->addItem("Fractal");
	algorithmCombox->setMinimumHeight(40);
	controllayout->addWidget(algorithmCombox);

	mainlayout->addLayout(controllayout);

	compressLable = new QLabel("Коэффициент сжатия: -");
	compressLable->setAlignment(Qt::AlignCenter);
	compressLable->setStyleSheet(
		"QLabel { "
		"font-size: 12px; "
		"padding: 10px; "
		"background-color: #e8f4f8; "
		"border-radius: 5px; "
		"}"
	);
	mainlayout->addWidget(compressLable);
}


void MainWindow::openimage()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,
		"Открыть изображение",
		"",
		"Изображения (*.bmp *.tiff *.tif);;Все файлы (*.*)"
	);

	if (fileName.isEmpty())
		return;

	originalImage = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);

	if (originalImage.empty()) {
		QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение");
		return;
	}

	displayimage(originalImage);
	compressButton->setEnabled(true);
	compressedImage = cv::Mat();
	compressLable->setText("Коэффициент сжатия: -");
}

void MainWindow::compressimage()
{
	if (originalImage.empty()) {
		QMessageBox::warning(this, "Ошибка", "Сначала загрузите изображение!");
		return;
	}

	int algorithmIndex = algorithmCombox->currentIndex();
	compressedImage = originalImage.clone();
	displayimage(compressedImage);
	saveButton->setEnabled(true);
	compressLable->setText("nan");
}

void MainWindow::saveimage()
{
	if (compressedImage.empty()) {
		QMessageBox::warning(this, "Ошибка", "Нет изображения для сохранения");
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(
		this,
		"Сохранить изображение",
		"",
		"JPEG (*.jpg *.jpeg);;Все файлы (*.*)"
	);

	if (fileName.isEmpty())
		return;

	bool saved = cv::imwrite(fileName.toStdString(), compressedImage);

	if (saved) {
		QMessageBox::information(this, "Удачно", "Изображение сохранено");
	}
	else {
		QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изображение");
	}
}

QImage MainWindow::imageInMat(const cv::Mat& mat) {
	if (mat.empty())
		return QImage();

	if (mat.type() == CV_8UC1) {
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
		return image.copy();
	}
	else if (mat.type() == CV_8UC3) {
		cv::Mat rgb;
		cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
		QImage image(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
		return image.copy();
	}

	return QImage();
}
void MainWindow::displayimage(const cv::Mat& image) {
	QImage qImage = imageInMat(image);
	QPixmap pixmap = QPixmap::fromImage(qImage);

	QPixmap scaledPixmap = pixmap.scaled(
		imageLable->size(),
		Qt::KeepAspectRatio,
		Qt::SmoothTransformation
	);

	imageLable->setPixmap(scaledPixmap);
}