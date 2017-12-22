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
#include "ui_webcamdisplay.h"
#include "ui_authenticationdialog.h"

#include <QAuthenticator>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>

MWWebcamDisplay::MWWebcamDisplay(const QString &argWebcamURL,
                                 QWidget *argParent) :
    QMainWindow{argParent},
    ui{new Ui::MWWebcamDisplay},
    webcamURL{new QUrl{argWebcamURL}}
{
    qnam = new QNetworkAccessManager{this};

    ui->setupUi(this);
    scene = new QGraphicsScene{this};
    ui->GVImageDisplay->setScene(scene);
    this->setWindowTitle(tr("WebcamDisplay: ") + webcamURL->toString());

    connect(qnam, &QNetworkAccessManager::authenticationRequired,
            this, &MWWebcamDisplay::AuthenticationRequired);
    connect(qnam, &QNetworkAccessManager::sslErrors,
            this, &MWWebcamDisplay::SSLErrors);

    refreshTimer = new QTimer{this};
    connect(refreshTimer, &QTimer::timeout,
            this, &MWWebcamDisplay::StartRequest);
    refreshTimer->start(1024);
}

MWWebcamDisplay::~MWWebcamDisplay() {
    scene->removeItem(currentImage.get());
    delete ui;
}

void MWWebcamDisplay::AuthenticationRequired(QNetworkReply*, QAuthenticator *argAuthenticator) {
    QDialog dialog;
    Ui::Dialog ui;
    ui.setupUi(&dialog);
    ui.LSiteDescription->setText(tr("%1 at %2").arg(argAuthenticator->realm()).arg(webcamURL->host()));

    ui.LEUsername->setText(webcamURL->userName());
    ui.LEPassword->setText(webcamURL->password());

    if (dialog.exec() == QDialog::Accepted) {
        argAuthenticator->setUser(ui.LEUsername->text());
        argAuthenticator->setPassword(ui.LEPassword->text());
    }
}

void MWWebcamDisplay::HttpFinished() {
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.").arg(reply->errorString()));
    } else if (!redirectionTarget.isNull()) {
        QUrl newURL = webcamURL->resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("Redirect to '%1'?").arg(newURL.toString()),
                                  QMessageBox::No | QMessageBox::Yes)
            == QMessageBox::Yes) {
            webcamURL.reset(new QUrl{newURL});
            reply->deleteLater();
            reply = nullptr;
            return;
        }
    }

    byteArray.reset(new QByteArray{reply->readAll()});
    reply->deleteLater();
    reply = nullptr;

    QPixmap image;
    image.loadFromData(*byteArray);
    recentImage.reset(currentImage.release());
    currentImage.reset(scene->addPixmap(image));
    scene->removeItem(recentImage.get());
    ui->GVImageDisplay->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MWWebcamDisplay::SSLErrors(QNetworkReply*, const QList<QSslError> &errors) {
    QString errorString;
    for (auto cit = errors.constBegin(); cit != errors.constEnd(); ++cit) {
        if ( !errorString.isEmpty() ) {
            errorString += ", ";
        }
        errorString += cit->errorString();
    }

    if (QMessageBox::warning(this, tr("HTTP"),
                               tr("One or more SSL errors occurred: %1").arg(errorString),
                               QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}

void MWWebcamDisplay::StartRequest() {
    reply = qnam->get(QNetworkRequest{*webcamURL});

    connect(reply, &QNetworkReply::finished,
            this, &MWWebcamDisplay::HttpFinished);
}
