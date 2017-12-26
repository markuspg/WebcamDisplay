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

#include <QMainWindow>

#include <memory>

class QAuthenticator;
class QGraphicsPixmapItem;
class QGraphicsScene;
class QNetworkAccessManager;
class QNetworkReply;
class QSslError;

namespace Ui {
class MWWebcamDisplay;
}

class MWWebcamDisplay final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MWWebcamDisplay(const QString &argWebcamURL,
                             QWidget *argParent = nullptr);
    MWWebcamDisplay(const MWWebcamDisplay &argWebcamDisplay) = delete;
    MWWebcamDisplay(MWWebcamDisplay &&argWebcamDisplay) = delete;
    virtual ~MWWebcamDisplay();

    MWWebcamDisplay &operator=(const MWWebcamDisplay &argWebcamDisplay) = delete;
    MWWebcamDisplay &operator=(MWWebcamDisplay &&argWebcamDisplay) = delete;

private:
    //! Pointer to the currently displayed image pixmap
    std::unique_ptr<QGraphicsPixmapItem> currentImage;
    QNetworkAccessManager *qnam = nullptr;
    //! Pointer to the previously displayed image pixmap
    std::unique_ptr<QGraphicsPixmapItem> recentImage;
    QTimer *refreshTimer = nullptr;
    //! Surface to store the image data
    QGraphicsScene *scene = nullptr;
    Ui::MWWebcamDisplay *ui = nullptr;
    std::unique_ptr<QUrl> webcamURL;

private slots:
    void AuthenticationRequired(QNetworkReply *argReply, QAuthenticator *argAuthenticator);
    void HttpFinished();
    void SSLErrors(QNetworkReply *argReply, const QList<QSslError> &errors);
    void StartRequest();
};

#endif // WEBCAMDISPLAY_H
