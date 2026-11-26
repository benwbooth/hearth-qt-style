// Kirigami's platform plugin for the org.hearth.desktop Qt Quick Controls
// style. Kirigami looks for a platform plugin named after the controls
// style (kf6/kirigami/platform/<style>.so) and without one falls back to
// built-in colours and units that ignore the desktop. This one hands
// Kirigami what org.kde.desktop's plugin (qqc2-desktop-style) gives:
// colours from kdeglobals (which hearth writes), units, icon colouring,
// with hearth's corner radius.
#include <Kirigami/Platform/PlatformPluginFactory>
#include <Kirigami/Platform/PlatformTheme>
#include <Kirigami/Platform/Units>

class HearthKirigamiPlugin : public Kirigami::Platform::PlatformPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PlatformPluginFactory_iid FILE "hearth-kirigami.json")
    Q_INTERFACES(Kirigami::Platform::PlatformPluginFactory)

public:
    using PlatformPluginFactory::PlatformPluginFactory;

    Kirigami::Platform::PlatformTheme *createPlatformTheme(QObject *parent) override
    {
        if (auto desktop = findPlugin(QStringLiteral("org.kde.desktop")))
            return desktop->createPlatformTheme(parent);
        return nullptr;
    }

    Kirigami::Platform::Units *createUnits(QObject *parent) override
    {
        Kirigami::Platform::Units *units = nullptr;
        if (auto desktop = findPlugin(QStringLiteral("org.kde.desktop")))
            units = desktop->createUnits(parent);
        // hearth's control corners: round(6 pt) (1 pt = 1.125 px).
        if (units)
            units->setcornerRadius(7);
        return units;
    }
};

#include "plugin.moc"
