#include "hearthstyle.h"

#include <QStylePlugin>

class HearthStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE "hearth.json")
public:
    QStyle *create(const QString &key) override
    {
        if (key.compare(QLatin1String("hearth"), Qt::CaseInsensitive) == 0)
            return new HearthStyle;
        return nullptr;
    }
};

#include "plugin.moc"
