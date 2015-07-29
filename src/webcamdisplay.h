/*
 * Copyright 2015 Markus Prasser
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

#ifndef WEBCAMDISPLAY_H
#define WEBCAMDISPLAY_H

#include <QGraphicsScene>
#include <QMainWindow>

#include <QtNetwork>
#include <QtWidgets>

namespace Ui {
class MWWebcamDisplay;
}

class MWWebcamDisplay : public QMainWindow
{
    Q_OBJECT

public:
    explicit MWWebcamDisplay( const QString &argWebcamURL, QWidget *argParent = nullptr );
    ~MWWebcamDisplay();

private:
    QFile *file = nullptr;
    QFileInfo fileInfo;
    QString fileName;
    bool httpRequestAborted = false;
    QTimer pingTimer;
    QNetworkAccessManager qnam;
    QNetworkReply *reply = nullptr;
    QGraphicsScene scene;                       //! Surface to store the image data
    QGraphicsPixmapItem *tempItem = nullptr;    //! Pointer to the recently displayed image pixmap
    Ui::MWWebcamDisplay *ui;
    QUrl webcamURL;

private slots:
    void AuthenticationRequired( QNetworkReply*, QAuthenticator *authenticator );
    void DownloadFile();
    void httpFinished();
    void httpReadyRead();
    void SSLErrors( QNetworkReply*, const QList<QSslError> &errors );
    void StartRequest( QUrl argWebcamURL );
};

#endif // WEBCAMDISPLAY_H
