#include <QtGui>
#include <QAction>
#include <QActionGroup>
#include <QContextMenuEvent>
#include <QDebug>
#include <QGridLayout>
#include <QMenu>
#include <QSizePolicy>
#include <QStatusBar>
#include "mainwindow.h"
#include "reportdialog.h"
#include "results.h"
#include "theme.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), showFrame(true)
{
	download = new SpeedMeter("Download");
	upload = new SpeedMeter("Upload");

	startButton = new QPushButton("Begin Test");

	statusMessage = new QLabel;
	clearStatusMessage();

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(download, 0, 0, Qt::AlignCenter);
	layout->addWidget(upload, 0, 1, Qt::AlignCenter);
	layout->addWidget(startButton, 1, 0, 1, 2, Qt::AlignCenter);
	layout->addWidget(statusMessage, 2, 0, 1, 2);

	QWidget *central = new QWidget;
	central->setLayout(layout);

	this->setCentralWidget(central);

	setFixedSize(sizeHint());

	connect(startButton, SIGNAL(clicked()),
		&this->test, SLOT(start()));

	connect(&this->test, SIGNAL(succeeded(double, double)), 
		this, SLOT(testSucceeded(double, double)));

	connect(&this->test, SIGNAL(failed(const QString&)),
		this, SLOT(testFailed(const QString&)));

	connect(&this->test, SIGNAL(started()),
		this, SLOT(testStarted()));
	
	connect(&this->test, SIGNAL(progressed(const QString&, int, double)),
		this, SLOT(testProgressed(const QString&, int, double)));

	Results results;
	if(!results.empty()) {
		QPair<double, double> resPair = results.getLastSpeed();
		this->download->setValue(resPair.first);
		this->upload->setValue(resPair.second);
	}

	// theme Lightblue

	startButton->setStyleSheet(
		"MainWindow QPushButton { "
		"background-color: rgba(255, 255, 0, 0.6); "
		//"border-image: "
		//"url(:/themes/Lightblue_button.png) 20; "
		//"border: 20 #729df5; "
		"font: bold 15px; "
		"}"
		"QPushButton:hover { "
		//"border-image: "
		//"url(:/themes/Lightblue_button_hover.png) 20;"
		"}"
		"QPushButton:pressed { "
		//"border-image: "
		//"url(:/themes/Lightblue_button_click.png) 20;"
		"}"
	);

	setStyleSheet(
		"QMainWindow { "
		"border-image: "
		"url(:/themes/Lightblue_background.png) 40; "
		"border: 40px; "
		"padding: 40px; "
		"}"
	);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu context(this);
	QMenu *themes = context.addMenu("&Themes");
	QAction *showFrameAction = context.addAction("Show window &frame");
	showFrameAction->setCheckable(true);
	showFrameAction->setChecked(showFrame);
	connect(showFrameAction, SIGNAL(toggled(bool)), this, SLOT(toggleShowFrame(bool)));
	context.addSeparator();

	context.addAction("&Report", this, SLOT(report()));
	context.addSeparator();

	context.addAction("&About", this, SLOT(about()));
	context.addAction("About &Qt", qApp, SLOT(aboutQt()));
	context.addAction("&Close", this, SLOT(close()));

	// themes
	QActionGroup *themesGroup = new QActionGroup(this);
    connect(themesGroup, SIGNAL(triggered(QAction*)),
        this, SLOT(switchTheme(QAction*)));

    QList<Theme> themesList = Theme::listThemes();
    for(int i=0; i<themesList.size(); ++i) {
        Theme& cur = themesList[i];
        QAction *action = themes->addAction(cur.name);
        action->setActionGroup(themesGroup);
        action->setCheckable(true);

        if(currentTheme == cur.name) {
            action->setChecked(true);
        }
    }

	context.exec(event->globalPos());
}


void MainWindow::testSucceeded(double downloadSpeed,
	double uploadSpeed)
{
	this->download->setValue(downloadSpeed);
	this->upload->setValue(uploadSpeed);

	clearStatusMessage();
	startButton->setText("Begin Test");

	Results res;
	res.addResult(downloadSpeed, uploadSpeed);
}

void MainWindow::testFailed(const QString& text)
{
	qDebug() << text;
	setStatusMessage("Test failed");
	startButton->setText("Try again");
}

void MainWindow::testProgressed(const QString &testName,
	int /*testProgress*/, double currentSpeed)
{
	QString msg = "%1 in progress";
	if(testName=="download") {
		msg = msg.arg("Download");
	} else {
		msg = msg.arg("Upload");
	}

	if(testName=="download")
		this->download->setValue(currentSpeed);
	else if(testName=="upload")
		this->upload->setValue(currentSpeed);
	setStatusMessage(msg);
}


void MainWindow::testStarted()
{
	setStatusMessage("Test in progress");
	this->download->setEmpty();
	this->upload->setEmpty();
	startButton->setText("Cancel");
}

void MainWindow::setStatusMessage(const QString &str) 
{
	statusMessage->setText(QString("Status: %1").arg(str));
}

void MainWindow::clearStatusMessage()
{
	setStatusMessage("Ready");
}

void MainWindow::switchTheme(QAction *action)
{
    currentTheme = action->text();

    QList<Theme> themeList = Theme::listThemes();
    int index;
    for(index=0; index<themeList.size(); ++index) {
        if(themeList[index].name == currentTheme)
            break;
    }

    Theme& themeObj = themeList[index];
    if(themeObj.size == Theme::Large) {
        rearrangeLarge();
    } else {
        rearrangeSmall();
    }

    loadTheme(themeObj.filenamePrefix);
}

void MainWindow::rearrangeLarge()
{
}
void MainWindow::rearrangeSmall()
{
}
void MainWindow::loadTheme(const QString& text)
{
}

void MainWindow::toggleShowFrame(bool state)
{
	showFrame = state;
	if(showFrame) {
		setWindowFlags(0);
	} else {
		setWindowFlags(Qt::FramelessWindowHint);
	}
	show();
}

void MainWindow::report() {
	ReportDialog *dialog = new ReportDialog(this);
	dialog->show();
}

void MainWindow::about() {}

