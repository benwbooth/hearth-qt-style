#pragma once

#include <QProxyStyle>

// Hearth: a Qt Widgets style that draws third-party Qt 5 and Qt 6 apps the
// way Ben's desktop shell hearth and his own Qt Quick apps look (macOS 26
// proportions). A proxy over Fusion: Fusion does the layout bookkeeping we
// don't change, everything visible is drawn here. Colours come from the
// application palette (hearth-color-scheme writes hearth's palette and
// accent into qt5ct/qt6ct and kdeglobals).
class HearthStyle : public QProxyStyle
{
    Q_OBJECT
public:
    HearthStyle();

    void polish(QWidget *widget) override;
    void unpolish(QWidget *widget) override;
    using QProxyStyle::polish;
    using QProxyStyle::unpolish;

    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = nullptr) const override;
    void drawControl(ControlElement ce, const QStyleOption *opt, QPainter *p,
                     const QWidget *w = nullptr) const override;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                            const QWidget *w = nullptr) const override;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                         const QWidget *w = nullptr) const override;
    QRect subElementRect(SubElement se, const QStyleOption *opt,
                         const QWidget *w = nullptr) const override;
    int pixelMetric(PixelMetric pm, const QStyleOption *opt = nullptr,
                    const QWidget *w = nullptr) const override;
    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &size,
                           const QWidget *w = nullptr) const override;
    int styleHint(StyleHint sh, const QStyleOption *opt = nullptr, const QWidget *w = nullptr,
                  QStyleHintReturn *ret = nullptr) const override;
};
