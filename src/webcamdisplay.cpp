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

#include "webcamdisplay.h"
#include "ui_webcamdisplay.h"
#include "ui_authenticationdialog.h"

MWWebcamDisplay::MWWebcamDisplay( const QString &argWebcamURL, QWidget *argParent ) :
    QMainWindow{ argParent },
    pingTimer{ this },
    ui{ new Ui::MWWebcamDisplay },
    webcamURL{ argWebcamURL }
{
    fileInfo.setFile( webcamURL.path() );
    fileName = fileInfo.fileName();

    ui->setupUi( this );
    ui->GVImageDisplay->setScene( &scene );
    this->setWindowTitle( tr( "WebcamDisplay: " ) + webcamURL.toString() );

    connect( &qnam, SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ),
             this, SLOT( AuthenticationRequired( QNetworkReply*, QAuthenticator* ) ) );
    connect( &qnam, SIGNAL( sslErrors( QNetworkReply*, QList< QSslError > ) ),
             this, SLOT( SSLErrors( QNetworkReply*, QList< QSslError > ) ) );

    connect( &pingTimer, &QTimer::timeout,
             this, &MWWebcamDisplay::DownloadFile );
    DownloadFile();
    pingTimer.start( 1000 );
}

MWWebcamDisplay::~MWWebcamDisplay() {
    delete ui;
    delete tempItem;
}

void MWWebcamDisplay::DownloadFile() {
    if (fileName.isEmpty())
        fileName = "index.html";

    file = new QFile{fileName};
    if (!file->open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("HTTP"), tr("Unable to save the file %1: %2.").arg(fileName).arg(file->errorString()));
        delete file;
        file = nullptr;
        return;
    }

    httpRequestAborted = false;
    StartRequest( webcamURL );
}

void MWWebcamDisplay::httpFinished() {
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = nullptr;
        }

        reply->deleteLater();
        return;
    }

    file->flush();
    file->close();

    QVariant redirection_target = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        file->remove();
        QMessageBox::information(this, tr("HTTP"), tr("Download failed: %1.").arg(reply->errorString()));
    }
    else if (!redirection_target.isNull()) {
        QUrl new_url = webcamURL.resolved(redirection_target.toUrl());
        if (QMessageBox::question(this, tr("HTTP"), tr("Redirect to '%1'?").arg(new_url.toString()),
                                  QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes) {
            webcamURL = new_url;
            reply->deleteLater();
            file->open(QIODevice::Truncate | QIODevice::WriteOnly);
            file->resize(0);
            StartRequest( webcamURL );
            return;
        }
    }

    reply->deleteLater();
    reply = nullptr;

    QGraphicsPixmapItem *temp = tempItem;

    file->open(QIODevice::ReadOnly);
    QPixmap image;
    image.loadFromData(file->readAll());
    tempItem = scene.addPixmap(image);
    ui->GVImageDisplay->fitInView(scene.itemsBoundingRect(), Qt::KeepAspectRatio);

    delete temp;

    file->close();
    delete file;
    file = nullptr;
}

void MWWebcamDisplay::httpReadyRead() {
    if (file)
        file->write(reply->readAll());
}

void MWWebcamDisplay::StartRequest(QUrl argWebcamURL) {
    reply = qnam.get(QNetworkRequest(argWebcamURL));

    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
}

void MWWebcamDisplay::AuthenticationRequired(QNetworkReply*, QAuthenticator *authenticator) {
    QDialog dialog;
    Ui::Dialog ui;
    ui.setupUi(&dialog);
    ui.LSiteDescription->setText( tr( "%1 at %2" ).arg( authenticator->realm() ).arg( webcamURL.host() ) );

    ui.LEUsername->setText( webcamURL.userName() );
    ui.LEPassword->setText( webcamURL.password() );

    if (dialog.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.LEUsername->text());
        authenticator->setPassword(ui.LEPassword->text());
    }
}

void MWWebcamDisplay::SSLErrors(QNetworkReply*, const QList<QSslError> &errors) {
    QString error_string;
    foreach (const QSslError &error, errors) {
        if (!error_string.isEmpty())
            error_string += ", ";
        error_string += error.errorString();
    }

    if (QMessageBox::warning(this, tr("HTTP"), tr("One or more SSL errors have occurred: %1").arg(error_string),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
