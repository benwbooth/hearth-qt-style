// Prints the style and window colour the platform theme gave, then opens
// a file dialog after a moment: with test/portal-stub.py on a private bus
// it should reach the FileChooser portal (qt5ct/qt6ct standard_dialogs).
#include <QApplication>
#include <QFileDialog>
#include <QStyle>
#include <QTimer>
#include <cstdio>
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    std::printf("{\"style\": \"%s\", \"window\": \"%s\"}\n",
                qPrintable(app.style()->objectName()),
                qPrintable(app.palette().color(QPalette::Window).name()));
    std::fflush(stdout);
    QTimer::singleShot(500, [] {
        QString f = QFileDialog::getOpenFileName(nullptr, QStringLiteral("hearth test open"));
        std::printf("{\"dialog-returned\": \"%s\"}\n", qPrintable(f));
        std::fflush(stdout);
        QCoreApplication::quit();
    });
    QTimer::singleShot(8000, [] { std::printf("{\"timeout\": true}\n"); QCoreApplication::exit(2); });
    return app.exec();
}
