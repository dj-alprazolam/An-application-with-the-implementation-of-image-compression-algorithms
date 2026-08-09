#include"mainwindow.h"
#include<qimage.h>
#include<qfiledialog.h>
#include<qfileinfo.h>
#include<qmessagebox.h>
#include<qpixmap.h>
#include<qstring.h>


MainWindow::MainWindow(QWidget* patern):QMainWindow(patern) {
	setupUI();
	connect(algorithmCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onAlgorithmChanged);
	
	updateAlgorithm();		
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

void MainWindow::compressimage() {
    if (originalImage.empty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите изображение!");
        return;
    }

    if (!currentAlgorithm) {
        QMessageBox::warning(this, "Ошибка", "Алгоритм сжатия не выбран!");
        return;
    }

    compressedImage = currentAlgorithm->compress(originalImage);

    if (compressedImage.empty()) {
        QMessageBox::critical(this, "Ошибка", "Ошибка при сжатии изображения");
        return;
    }

    double ratio = currentAlgorithm->getCompression();

    QString compressionInfo;

    if (currentAlgorithm->algorithmname() == "DCT") {
        if (ratio < 1.0) {
            double factor = 1.0 / ratio;
            double percent = ratio * 100.0;
            compressionInfo = QString("Уменьшение в %1 раз (%2% от исходного)")
                                  .arg(factor, 0, 'f', 2)
                                  .arg(percent, 0, 'f', 1);
        } else {
            compressionInfo = QString("Размер не изменился (%1% от исходного)")
                                  .arg(ratio * 100.0, 0, 'f', 1);
        }
    } else {
        if (ratio < 1.0) {
            double factor = 1.0 / ratio;
            double percent = ratio * 100.0;
            compressionInfo = QString("Уменьшение в %1 раз (%2% от исходного)")
                                  .arg(factor, 0, 'f', 2)
                                  .arg(percent, 0, 'f', 1);
        } else if (ratio > 1.0) {
            compressionInfo = QString(" Увеличение в %1 раз (алгоритм неэффективен)")
                                  .arg(ratio, 0, 'f', 2);
        } else {
            compressionInfo = "Размер не изменился";
        }
    }

    compressLable->setText(QString("Коэффициент сжатия: %1").arg(compressionInfo));
    displayimage(compressedImage);
    saveButton->setEnabled(true);
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
void MainWindow:: updateAlgorithm(){
	QString algo = algorithmCombox->currentText();
	if(algo == "RLE"){
		currentAlgorithm = std::make_unique<rle>();
	} 
	else if(algo == "LWZ"){
		currentAlgorithm = std::make_unique<lwz>();
	}else if(algo == "HUFFMAN" ){
		currentAlgorithm = std::make_unique<huff>();
	}else if(algo == "DCT"){
		currentAlgorithm = std::make_unique<dct>();
	}else if(algo == "FRACTAL"){
		currentAlgorithm = std::make_unique<Fractal>();
	}
	
}

void MainWindow::onAlgorithmChanged(int index) {
    updateAlgorithm();
    if (!originalImage.empty()) {
        compressButton->setEnabled(true);
        compressLable->setText("Коэффициент сжатия: - (алгоритм изменен)");
        saveButton->setEnabled(false);
    }
}