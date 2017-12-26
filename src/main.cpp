/*
 * Copyright 2015-2017 Markus Prasser
 *
 * This file is part of WebcamDisplay.
 *
 *  WebcamDisplay is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WebcamDisplay is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WebcamDisplay.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "webcamdisplay.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a{argc, argv};

    if (argc != 2) {
        QMessageBox::information(nullptr, QObject::tr("Wrong quantity of arguments"),
                                 QObject::tr("The number of arguments passed was not valid."
                                             " There is exactly one argument which may and must be given:"
                                             " The url of the webcam to be displayed."));
        return 0;
    }

    MWWebcamDisplay w{argv[1]};
    w.show();

    return a.exec();
}
