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
    refreshTimer{ this },
    ui{ new Ui::MWWebcamDisplay },
    webcamURL{ argWebcamURL }
{

    ui->setupUi( this );
    ui->GVImageDisplay->setScene( &scene );
    this->setWindowTitle( tr( "WebcamDisplay: " ) + webcamURL.toString() );

    connect( &qnam, SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ),
             this, SLOT( AuthenticationRequired( QNetworkReply*, QAuthenticator* ) ) );
    connect( &qnam, SIGNAL( sslErrors( QNetworkReply*, QList< QSslError > ) ),
             this, SLOT( SSLErrors( QNetworkReply*, QList< QSslError > ) ) );

    connect( &refreshTimer, &QTimer::timeout,
             this, &MWWebcamDisplay::StartRequest );
    StartRequest();
    refreshTimer.start( 1000 );
}

MWWebcamDisplay::~MWWebcamDisplay() {
    delete ui;
    delete currentImage;
    delete recentImage;
}

void MWWebcamDisplay::AuthenticationRequired( QNetworkReply*, QAuthenticator *authenticator ) {
    QDialog dialog;
    Ui::Dialog ui;
    ui.setupUi( &dialog );
    ui.LSiteDescription->setText( tr( "%1 at %2" ).arg( authenticator->realm() ).arg( webcamURL.host() ) );

    ui.LEUsername->setText( webcamURL.userName() );
    ui.LEPassword->setText( webcamURL.password() );

    if ( dialog.exec() == QDialog::Accepted ) {
        authenticator->setUser( ui.LEUsername->text() );
        authenticator->setPassword( ui.LEPassword->text() );
    }
}

void MWWebcamDisplay::httpFinished() {
    QVariant redirection_target = reply->attribute( QNetworkRequest::RedirectionTargetAttribute );
    if ( reply->error() ) {
        QMessageBox::information( this, tr( "HTTP" ), tr( "Download failed: %1." ).arg( reply->errorString() ) );
    }
    else if ( !redirection_target.isNull() ) {
        QUrl newURL = webcamURL.resolved( redirection_target.toUrl() );
        if ( QMessageBox::question( this, tr( "HTTP" ), tr( "Redirect to '%1'?" ).arg( newURL.toString() ),
                                    QMessageBox::No | QMessageBox::Yes ) == QMessageBox::Yes ) {
            webcamURL = newURL;
            reply->deleteLater();
            reply = nullptr;
            StartRequest();
            return;
        }
    }

    byteArray = new QByteArray{ reply->readAll() };
    reply->deleteLater();
    reply = nullptr;

    QPixmap image;
    image.loadFromData( *byteArray );
    recentImage = currentImage;
    currentImage = scene.addPixmap( image );
    delete recentImage;
    ui->GVImageDisplay->fitInView( scene.itemsBoundingRect(), Qt::KeepAspectRatio );

    delete byteArray;
    byteArray = nullptr;
}

void MWWebcamDisplay::SSLErrors( QNetworkReply*, const QList<QSslError> &errors ) {
    QString error_string;
    foreach ( const QSslError &error, errors ) {
        if ( !error_string.isEmpty() ) {
            error_string += ", ";
        }
        error_string += error.errorString();
    }

    if ( QMessageBox::warning( this, tr( "HTTP" ),
                               tr( "One or more SSL errors occurred: %1" ).arg( error_string ),
                               QMessageBox::Ignore | QMessageBox::Abort ) == QMessageBox::Ignore ) {
        reply->ignoreSslErrors();
    }
}

void MWWebcamDisplay::StartRequest() {
    reply = qnam.get( QNetworkRequest( webcamURL ) );

    connect( reply, SIGNAL( finished() ), this, SLOT( httpFinished() ) );
}
