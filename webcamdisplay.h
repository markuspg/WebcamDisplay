#ifndef WEBCAMDISPLAY_H
#define WEBCAMDISPLAY_H

#include <QMainWindow>

namespace Ui {
class MWWebcamDisplay;
}

class MWWebcamDisplay : public QMainWindow
{
    Q_OBJECT

public:
    explicit MWWebcamDisplay( QWidget *argParent = nullptr );
    ~MWWebcamDisplay();

private:
    Ui::MWWebcamDisplay *ui;
};

#endif // WEBCAMDISPLAY_H
