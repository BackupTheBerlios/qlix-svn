#include <libmtp.h>
#include <QMainWindow>
#include <QApplication>
#include <QPlastiqueStyle>
#include <QtDebug>
#include <signal.h>



#include "mtp/MtpSubSystem.h"
#include "widgets/QlixMainWindow.h"
#ifdef LINUX_SIGNALS
#include "linuxsignals.h"
#endif

MtpSubSystem _subSystem;
int main(int argc, char* argv[])
{
    installSignalHandlers();
    _subSystem.Initialize(); 
    qDebug() << "Subsystem is up!";
    QApplication app(argc, argv);
    app.setStyle("Plastique");

    QlixMainWindow qmw(&_subSystem);
    qmw.show();

    Q_INIT_RESOURCE(Qlix);
    app.exec();
    return 0;
} 
