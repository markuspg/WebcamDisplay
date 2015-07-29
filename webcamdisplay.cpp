#include "webcamdisplay.h"
#include "ui_webcamdisplay.h"

MWWebcamDisplay::MWWebcamDisplay( QWidget *argParent ) :
    QMainWindow{ argParent },
    ui{ new Ui::MWWebcamDisplay }
{
    ui->setupUi( this );
}

MWWebcamDisplay::~MWWebcamDisplay() {
    delete ui;
}
