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

#ifndef WEBCAMDISPLAY_H
#define WEBCAMDISPLAY_H

#include <QGraphicsScene>
#include <QMainWindow>

#include <QtNetwork>
#include <QtWidgets>

#include <memory>

namespace Ui {
class MWWebcamDisplay;
}

class MWWebcamDisplay final : public QMainWindow {
    Q_OBJECT

public:
    explicit MWWebcamDisplay(const QString &argWebcamURL,
                             QWidget *argParent = nullptr);
    MWWebcamDisplay( const MWWebcamDisplay &argWebcamDisplay ) = delete;
    MWWebcamDisplay( MWWebcamDisplay &&argWebcamDisplay ) = delete;
    ~MWWebcamDisplay();

    MWWebcamDisplay& operator=( const MWWebcamDisplay &argWebcamDisplay ) = delete;
    MWWebcamDisplay& operator=( MWWebcamDisplay &&argWebcamDisplay ) = delete;

private:
    std::unique_ptr< QByteArray > byteArray = nullptr;
    std::unique_ptr< QGraphicsPixmapItem > currentImage = nullptr;  //! Pointer to the currently displayed image pixmap
    bool httpRequestAborted = false;
    QNetworkAccessManager qnam;
    std::unique_ptr< QGraphicsPixmapItem > recentImage = nullptr;   //! Pointer to the previously displayed image pixmap
    QTimer *refreshTimer = nullptr;
    QNetworkReply *reply = nullptr;
    QGraphicsScene scene;                           //! Surface to store the image data
    Ui::MWWebcamDisplay *ui = nullptr;
    QUrl webcamURL;

private slots:
    void AuthenticationRequired( QNetworkReply*, QAuthenticator *argAuthenticator );
    void HttpFinished();
    void SSLErrors( QNetworkReply*, const QList<QSslError> &errors );
    void StartRequest();
};

#endif // WEBCAMDISPLAY_H
