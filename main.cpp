#include "webcamdisplay.h"
#include <QApplication>
#include <QMessageBox>
#include <QObject>

int main( int argc, char *argv[] )
{
    QApplication a{ argc, argv };

    if ( argc != 2 ) {
        QMessageBox::information(nullptr, QObject::tr("Wrong quantity of arguments"),
                                 QObject::tr("The number of arguments passed was not valid."
                                             " There is exactly one argument which may and must be given:"
                                             " The url of the webcam to be displayed."));
        return 0;
    }

    MWWebcamDisplay w;
    w.show();

    return a.exec();
}
