// A widget gallery for checking the Hearth style:
//   hearth-style-gallery-qt6 [--style Hearth] [--shot out.png [--menu|--combo]]
// --shot renders the window (and, with --menu/--combo, an open menu or
// combo box popup composited over it, as a compositor would) to a PNG and
// quits; without it the window stays open.
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPainter>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>

static QMenu *fileMenu(QWidget *parent)
{
    auto m = new QMenu(QStringLiteral("&File"), parent);
    m->addAction(QIcon::fromTheme(QStringLiteral("document-new")), QStringLiteral("New Window"))->setShortcut(QKeySequence(QStringLiteral("Ctrl+N")));
    m->addAction(QIcon::fromTheme(QStringLiteral("document-open")), QStringLiteral("Open…"))->setShortcut(QKeySequence(QStringLiteral("Ctrl+O")));
    auto recent = m->addMenu(QStringLiteral("Open Recent"));
    recent->addAction(QStringLiteral("notes.txt"));
    m->addSeparator();
    auto a = m->addAction(QStringLiteral("Show Sidebar"));
    a->setCheckable(true);
    a->setChecked(true);
    a->setShortcut(QKeySequence(QStringLiteral("Ctrl+Alt+S")));
    m->addAction(QStringLiteral("Save"))->setShortcut(QKeySequence(QStringLiteral("Ctrl+S")));
    auto d = m->addAction(QStringLiteral("Revert to Saved"));
    d->setEnabled(false);
    m->addSeparator();
    m->addAction(QStringLiteral("Close Window"))->setShortcut(QKeySequence(QStringLiteral("Ctrl+W")));
    return m;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    const QStringList args = app.arguments();
    const int shotAt = args.indexOf(QStringLiteral("--shot"));
    const QString shot = shotAt >= 0 && shotAt + 1 < args.size() ? args.at(shotAt + 1) : QString();
    const bool withMenu = args.contains(QStringLiteral("--menu"));
    const bool withCombo = args.contains(QStringLiteral("--combo"));

    QMainWindow win;
    win.setWindowTitle(QStringLiteral("Hearth style — Qt %1").arg(QLatin1String(qVersion())));
    QMenu *file = fileMenu(&win);
    win.menuBar()->addMenu(file);
    win.menuBar()->addMenu(QStringLiteral("&Edit"));
    win.menuBar()->addMenu(QStringLiteral("&View"));

    auto tb = win.addToolBar(QStringLiteral("Main"));
    tb->setMovable(false);
    tb->addAction(QIcon::fromTheme(QStringLiteral("go-previous")), QStringLiteral("Back"));
    tb->addAction(QIcon::fromTheme(QStringLiteral("go-next")), QStringLiteral("Forward"));
    tb->addSeparator();
    auto grid = tb->addAction(QIcon::fromTheme(QStringLiteral("view-grid")), QStringLiteral("Grid"));
    grid->setCheckable(true);
    grid->setChecked(true);
    tb->addAction(QIcon::fromTheme(QStringLiteral("view-list-details")), QStringLiteral("List"));

    auto central = new QWidget;
    auto outer = new QHBoxLayout(central);
    outer->setContentsMargins(16, 12, 16, 12);
    outer->setSpacing(16);

    // Left column: controls.
    auto left = new QVBoxLayout;
    left->setSpacing(10);
    auto buttons = new QHBoxLayout;
    auto ok = new QPushButton(QStringLiteral("OK"));
    ok->setDefault(true);
    buttons->addWidget(new QPushButton(QStringLiteral("Cancel")));
    buttons->addWidget(ok);
    auto off = new QPushButton(QStringLiteral("Disabled"));
    off->setEnabled(false);
    buttons->addWidget(off);
    left->addLayout(buttons);

    auto form = new QFormLayout;
    form->setHorizontalSpacing(10);
    form->setVerticalSpacing(8);
    auto name = new QLineEdit(QStringLiteral("Hearth"));
    form->addRow(QStringLiteral("Name:"), name);
    auto search = new QLineEdit;
    search->setPlaceholderText(QStringLiteral("Search"));
    search->setClearButtonEnabled(true);
    form->addRow(QStringLiteral("Search:"), search);
    auto combo = new QComboBox;
    combo->addItems({QStringLiteral("Automatic"), QStringLiteral("Light"), QStringLiteral("Dark")});
    combo->setCurrentIndex(2);
    form->addRow(QStringLiteral("Appearance:"), combo);
    auto editable = new QComboBox;
    editable->setEditable(true);
    editable->addItems({QStringLiteral("Inter"), QStringLiteral("Noto Sans")});
    form->addRow(QStringLiteral("Font:"), editable);
    auto spin = new QSpinBox;
    spin->setRange(6, 72);
    spin->setValue(13);
    form->addRow(QStringLiteral("Size:"), spin);
    auto slider = new QSlider(Qt::Horizontal);
    slider->setValue(62);
    form->addRow(QStringLiteral("Glass:"), slider);
    auto progress = new QProgressBar;
    progress->setValue(40);
    form->addRow(QStringLiteral("Progress:"), progress);
    left->addLayout(form);

    auto checks = new QHBoxLayout;
    auto c1 = new QCheckBox(QStringLiteral("Show hidden"));
    c1->setChecked(true);
    checks->addWidget(c1);
    checks->addWidget(new QCheckBox(QStringLiteral("Previews")));
    auto r1 = new QRadioButton(QStringLiteral("Icons"));
    r1->setChecked(true);
    checks->addWidget(r1);
    checks->addWidget(new QRadioButton(QStringLiteral("List")));
    checks->addStretch();
    left->addLayout(checks);

    auto tabs = new QTabWidget;
    auto general = new QWidget;
    auto gl = new QVBoxLayout(general);
    gl->addWidget(new QLabel(QStringLiteral("Tabs are a segmented control over a rounded box.")));
    auto group = new QGroupBox(QStringLiteral("Sidebar"));
    auto grl = new QVBoxLayout(group);
    grl->addWidget(new QCheckBox(QStringLiteral("Recents")));
    auto c3 = new QCheckBox(QStringLiteral("Tags"));
    c3->setTristate(true);
    c3->setCheckState(Qt::PartiallyChecked);
    grl->addWidget(c3);
    gl->addWidget(group);
    gl->addStretch();
    tabs->addTab(general, QStringLiteral("General"));
    tabs->addTab(new QWidget, QStringLiteral("Tags"));
    tabs->addTab(new QWidget, QStringLiteral("Advanced"));
    left->addWidget(tabs, 1);
    outer->addLayout(left, 3);

    // Right column: views.
    auto right = new QVBoxLayout;
    auto list = new QListWidget;
    for (const char *s : {"Applications", "Desktop", "Documents", "Downloads", "Music", "Pictures", "Videos",
                          "src", "Games", "Projects", "Backups", "Archive"})
        list->addItem(new QListWidgetItem(QIcon::fromTheme(QStringLiteral("folder")), QString::fromLatin1(s)));
    list->setCurrentRow(2);
    right->addWidget(list, 1);
    auto tree = new QTreeWidget;
    tree->setHeaderLabels({QStringLiteral("Name"), QStringLiteral("Size"), QStringLiteral("Kind")});
    auto top = new QTreeWidgetItem(tree, {QStringLiteral("hearth"), QStringLiteral("—"), QStringLiteral("Folder")});
    new QTreeWidgetItem(top, {QStringLiteral("shell.qml"), QStringLiteral("4 KB"), QStringLiteral("QML")});
    auto sel = new QTreeWidgetItem(top, {QStringLiteral("Theme.qml"), QStringLiteral("9 KB"), QStringLiteral("QML")});
    new QTreeWidgetItem(tree, {QStringLiteral("grove"), QStringLiteral("—"), QStringLiteral("Folder")});
    top->setExpanded(true);
    tree->setCurrentItem(sel);
    tree->header()->resizeSection(0, 140);
    right->addWidget(tree, 1);
    outer->addLayout(right, 2);

    win.setCentralWidget(central);
    win.statusBar()->showMessage(QStringLiteral("Style: %1").arg(app.style()->objectName()));
    win.resize(900, 600);
    win.show();
    win.activateWindow();
    if (args.contains(QStringLiteral("--focus-list")))
        list->setFocus();
    else
        name->setFocus();

    if (shot.isEmpty())
        return app.exec();

    QTimer::singleShot(400, [&] {
        QWidget *popup = nullptr;
        if (withMenu) {
            file->popup(win.mapToGlobal(QPoint(300, 70)));
            file->setActiveAction(file->actions().at(1));
            popup = file;
        } else if (withCombo) {
            combo->showPopup();
            popup = QApplication::activePopupWidget();
        }
        QTimer::singleShot(400, [&, popup] {
            QPixmap img = win.grab();
            if (popup) {
                QPainter p(&img);
                const QPoint at = win.mapFromGlobal(popup->mapToGlobal(QPoint(0, 0)));
                // A soft shadow, as the compositor draws.
                for (int i = 8; i > 0; --i) {
                    p.setPen(Qt::NoPen);
                    p.setBrush(QColor(0, 0, 0, 6));
                    p.setRenderHint(QPainter::Antialiasing);
                    p.drawRoundedRect(QRectF(at.x() - i, at.y() - i + 6, popup->width() + 2 * i, popup->height() + 2 * i), 14 + i, 14 + i);
                }
                p.drawPixmap(at, popup->grab());
            }
            img.save(shot);
            QApplication::quit();
        });
    });
    return app.exec();
}
