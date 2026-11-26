#include "hearthstyle.h"

#include <QAbstractItemView>
#include <QAbstractScrollArea>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QHeaderView>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QStyleFactory>
#include <QStyleOption>
#include <QTabBar>
#include <QTabWidget>
#include <QTableView>
#include <QToolButton>
#include <QTreeView>
#include <QtMath>

namespace {

// ---- metrics -------------------------------------------------------------
// One hearth point is 1.125 logical px (shell/Theme.qml).
constexpr qreal Pt = 1.125;

// Menus: hearth/shared/app-menus/MacMenuStyle.qml.
constexpr int MenuRadius = 14;          // round(12 pt)
constexpr int MenuPadding = 5;          // round(4 pt)
constexpr int MenuRow = 27;             // round(24 pt)
constexpr int MenuSelRadius = 9;        // round(8 pt)
constexpr int MenuTextInset = 11;       // round(10 pt)
constexpr int MenuGutter = 27;          // round(24 pt): checkmark / icon column
constexpr int MenuSeparator = 10;       // round(9 pt)
constexpr int MenuHeader = 25;          // round(22 pt)
constexpr int MenuMinWidth = 203;       // round(180 pt)

// Controls (grove's Theme.qml: 28 pt controls, 6 pt corners).
constexpr int ControlHeight = 28;
constexpr int ButtonHeight = 29;        // round(26 pt), capsule
constexpr int FieldRadius = 7;          // round(6 pt)
constexpr int CardRadius = 10;
constexpr int SmallRadius = 5;

// ---- colours ---------------------------------------------------------------

QColor rgba(qreal r, qreal g, qreal b, qreal a)
{
    return QColor::fromRgbF(float(r), float(g), float(b), float(a));
}

QColor withAlpha(QColor c, qreal a)
{
    c.setAlphaF(float(a));
    return c;
}

QColor mix(const QColor &a, const QColor &b, qreal t)
{
    return QColor::fromRgbF(float(a.redF() + (b.redF() - a.redF()) * t),
                            float(a.greenF() + (b.greenF() - a.greenF()) * t),
                            float(a.blueF() + (b.blueF() - a.blueF()) * t),
                            float(a.alphaF() + (b.alphaF() - a.alphaF()) * t));
}

struct Colors {
    bool dark;
    QColor window, base, fg, fg2, fgDisabled, accent, accentText;
    QColor field, fieldHover, fieldPressed, fieldBorder;
    QColor hover, pressed, selected, separator, track;
};

Colors colors(const QPalette &pal)
{
    Colors c;
    c.window = pal.color(QPalette::Window);
    c.base = pal.color(QPalette::Base);
    c.dark = c.window.lightness() < 128;
    c.fg = pal.color(QPalette::Active, QPalette::WindowText);
    c.fg2 = c.dark ? rgba(1, 1, 1, 0.55) : rgba(0, 0, 0, 0.5);
    c.fgDisabled = c.dark ? rgba(1, 1, 1, 0.28) : rgba(0, 0, 0, 0.26);
    c.accent = pal.color(QPalette::Active, QPalette::Highlight);
    c.accentText = pal.color(QPalette::Active, QPalette::HighlightedText);
    c.field = c.dark ? rgba(1, 1, 1, 0.10) : rgba(0, 0, 0, 0.055);
    c.fieldHover = c.dark ? rgba(1, 1, 1, 0.15) : rgba(0, 0, 0, 0.085);
    c.fieldPressed = c.dark ? rgba(1, 1, 1, 0.21) : rgba(0, 0, 0, 0.13);
    c.fieldBorder = c.dark ? rgba(1, 1, 1, 0.10) : rgba(0, 0, 0, 0.14);
    c.hover = c.dark ? rgba(1, 1, 1, 0.08) : rgba(0, 0, 0, 0.06);
    c.pressed = c.dark ? rgba(1, 1, 1, 0.14) : rgba(0, 0, 0, 0.11);
    c.selected = c.dark ? rgba(1, 1, 1, 0.16) : rgba(0, 0, 0, 0.10);
    c.separator = c.dark ? rgba(1, 1, 1, 0.12) : rgba(0, 0, 0, 0.10);
    // Opaque, so a segmented tab bar hides the tab pane's edge under it.
    c.track = mix(c.window, c.fg, c.dark ? 0.10 : 0.075);
    return c;
}

// The menu glass (MacMenuStyle.qml). Translucent where the compositor
// blurs behind popups (arbor blurs every app's popups); on X11 without
// that, nearly opaque. HEARTH_STYLE_MENU_GLASS=1/0 overrides.
bool menuGlass()
{
    static const bool glass = [] {
        const QByteArray env = qgetenv("HEARTH_STYLE_MENU_GLASS");
        if (!env.isEmpty())
            return env != "0";
        return QGuiApplication::platformName().startsWith(QLatin1String("wayland"));
    }();
    return glass;
}

QColor menuTint(bool dark)
{
    if (menuGlass())
        return dark ? rgba(0.15, 0.155, 0.17, 0.74) : rgba(0.975, 0.975, 0.98, 0.74);
    return dark ? rgba(0.17, 0.175, 0.19, 0.985) : rgba(0.975, 0.975, 0.98, 0.985);
}

// Menu text (MacMenuStyle.qml): its own greys, not the palette's.
QColor menuFg(bool dark) { return dark ? QColor("#f5f5f7") : QColor("#1d1d1f"); }
QColor menuFg2(bool dark) { return dark ? rgba(1, 1, 1, 0.55) : rgba(0, 0, 0, 0.5); }
QColor menuFgDisabled(bool dark) { return dark ? rgba(1, 1, 1, 0.28) : rgba(0, 0, 0, 0.26); }
QColor menuSeparator(bool dark) { return dark ? rgba(1, 1, 1, 0.13) : rgba(0, 0, 0, 0.11); }

// SF Pro when installed, else Inter (hearth's font), at 13 pt (Inter runs
// ~4% larger than SF Pro at the same size).
QFont menuFont(const QFont &appFont)
{
    static const QString family = [] {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QStringList all = QFontDatabase::families();
#else
        const QStringList all = QFontDatabase().families();
#endif
        for (const char *f : {"SF Pro", "SF Pro Text", "Inter", "Inter Variable"})
            if (all.contains(QLatin1String(f)))
                return QString::fromLatin1(f);
        return QString();
    }();
    QFont f = appFont;
    if (family.isEmpty())
        return f;
    f.setFamily(family);
    const qreal px = qRound(13 * Pt * (family.startsWith(QLatin1String("Inter")) ? 0.962 : 1.0) * 10) / 10.0;
    f.setPointSizeF(px * 72.0 / 96.0);
    f.setWeight(QFont::Normal);
    f.setLetterSpacing(QFont::AbsoluteSpacing, -0.09);
    return f;
}

// ---- painting helpers ------------------------------------------------------

struct Saver {
    QPainter *p;
    explicit Saver(QPainter *painter) : p(painter)
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);
    }
    ~Saver() { p->restore(); }
};

void fillRounded(QPainter *p, const QRectF &r, qreal radius, const QBrush &brush)
{
    Saver s(p);
    p->setPen(Qt::NoPen);
    p->setBrush(brush);
    radius = qMin(radius, qMin(r.width(), r.height()) / 2);
    p->drawRoundedRect(r, radius, radius);
}

// A stroke inside `r` (the line's outer edge on r's edge).
void strokeRounded(QPainter *p, const QRectF &r, qreal radius, const QColor &color, qreal width = 1)
{
    Saver s(p);
    p->setPen(QPen(color, width));
    p->setBrush(Qt::NoBrush);
    const QRectF in = r.adjusted(width / 2, width / 2, -width / 2, -width / 2);
    radius = qMax<qreal>(0, qMin(radius - width / 2, qMin(in.width(), in.height()) / 2));
    p->drawRoundedRect(in, radius, radius);
}

void chevron(QPainter *p, const QPointF &c, qreal w, qreal h, Qt::ArrowType dir,
             const QColor &color, qreal width = 1.5)
{
    Saver s(p);
    QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);
    QPainterPath path;
    switch (dir) {
    case Qt::DownArrow:
        path.moveTo(c.x() - w / 2, c.y() - h / 2);
        path.lineTo(c.x(), c.y() + h / 2);
        path.lineTo(c.x() + w / 2, c.y() - h / 2);
        break;
    case Qt::UpArrow:
        path.moveTo(c.x() - w / 2, c.y() + h / 2);
        path.lineTo(c.x(), c.y() - h / 2);
        path.lineTo(c.x() + w / 2, c.y() + h / 2);
        break;
    case Qt::LeftArrow:
        path.moveTo(c.x() + h / 2, c.y() - w / 2);
        path.lineTo(c.x() - h / 2, c.y());
        path.lineTo(c.x() + h / 2, c.y() + w / 2);
        break;
    default:
        path.moveTo(c.x() - h / 2, c.y() - w / 2);
        path.lineTo(c.x() + h / 2, c.y());
        path.lineTo(c.x() - h / 2, c.y() + w / 2);
        break;
    }
    p->drawPath(path);
}

// Up and down chevrons stacked (pop-up buttons, spin boxes).
void updown(QPainter *p, const QPointF &c, const QColor &color)
{
    chevron(p, QPointF(c.x(), c.y() - 3.5), 6.5, 3.5, Qt::UpArrow, color, 1.4);
    chevron(p, QPointF(c.x(), c.y() + 3.5), 6.5, 3.5, Qt::DownArrow, color, 1.4);
}

void checkMark(QPainter *p, const QRectF &box, const QColor &color, qreal width = 1.8)
{
    Saver s(p);
    p->setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p->setBrush(Qt::NoBrush);
    const qreal x = box.x(), y = box.y(), w = box.width(), h = box.height();
    QPainterPath path;
    path.moveTo(x + w * 0.22, y + h * 0.53);
    path.lineTo(x + w * 0.42, y + h * 0.73);
    path.lineTo(x + w * 0.78, y + h * 0.29);
    p->drawPath(path);
}

bool keyboardFocus(const QStyleOption *opt)
{
    return (opt->state & QStyle::State_HasFocus) && (opt->state & QStyle::State_KeyboardFocusChange);
}

// The accent focus ring around a control shape (grove: 2 px, 60%).
void focusRing(QPainter *p, const QRectF &r, qreal radius, const Colors &c)
{
    strokeRounded(p, r, radius, withAlpha(c.accent, 0.6), 2);
}

// A filled, borderless control (push buttons, pop-up buttons, tool buttons).
void drawButtonShape(QPainter *p, const QStyleOption *opt, qreal radius, bool accent, bool flat,
                     const Colors &c)
{
    const bool enabled = opt->state & QStyle::State_Enabled;
    const bool sunken = opt->state & (QStyle::State_Sunken);
    const bool on = opt->state & QStyle::State_On;
    const bool hover = enabled && (opt->state & QStyle::State_MouseOver);
    QRectF r = QRectF(opt->rect).adjusted(1, 1, -1, -1);
    QColor fill;
    if (accent && enabled)
        fill = sunken ? c.accent.darker(115) : c.accent;
    else if (sunken)
        fill = c.fieldPressed;
    else if (on)
        fill = c.selected;
    else if (hover)
        fill = c.fieldHover;
    else
        fill = flat ? QColor(Qt::transparent) : c.field;
    if (!enabled)
        fill = withAlpha(fill, fill.alphaF() * 0.6);
    if (fill.alpha())
        fillRounded(p, r, radius, fill);
    if (keyboardFocus(opt))
        focusRing(p, QRectF(opt->rect), radius + 1, c);
}

// A text field: a light well with a hairline, an accent ring when focused.
void drawField(QPainter *p, const QRect &rect, const QStyleOption *opt, const Colors &c,
               qreal radius = FieldRadius)
{
    const bool enabled = opt->state & QStyle::State_Enabled;
    const bool focus = enabled && (opt->state & QStyle::State_HasFocus);
    const QRectF r = QRectF(rect).adjusted(1, 1, -1, -1);
    QColor fill = c.dark ? rgba(1, 1, 1, 0.07) : c.base;
    if (!enabled)
        fill = withAlpha(fill, fill.alphaF() * 0.5);
    fillRounded(p, r, radius, fill);
    if (focus) {
        strokeRounded(p, QRectF(rect), radius + 1, withAlpha(c.accent, 0.45), 3);
        strokeRounded(p, r, radius, c.accent, 1);
    } else {
        strokeRounded(p, r, radius, enabled ? c.fieldBorder : withAlpha(c.fieldBorder, 0.5), 1);
    }
}

// The selection fill of item views: the accent while the view's window is
// active, grey otherwise (macOS).
QColor selectionFill(const QStyleOption *opt, const Colors &c)
{
    if (opt->state & QStyle::State_Active)
        return opt->palette.color(QPalette::Active, QPalette::Highlight);
    return c.selected;
}

bool isVertical(QTabBar::Shape s)
{
    return s == QTabBar::RoundedWest || s == QTabBar::RoundedEast
        || s == QTabBar::TriangularWest || s == QTabBar::TriangularEast;
}

} // namespace

HearthStyle::HearthStyle()
    : QProxyStyle(QStyleFactory::create(QStringLiteral("Fusion")))
{
}

// ---- polish -------------------------------------------------------------------

static bool isComboPopup(const QWidget *w)
{
    return w && w->inherits("QComboBoxPrivateContainer");
}

void HearthStyle::polish(QWidget *w)
{
    QProxyStyle::polish(w);
    if (!w)
        return;
    if (qobject_cast<QMenu *>(w) || isComboPopup(w) || w->inherits("QTipLabel")) {
        // Rounded (and, on Wayland, see-through) popups.
        w->setAttribute(Qt::WA_TranslucentBackground, true);
        if (qobject_cast<QMenu *>(w))
            w->setFont(menuFont(QApplication::font("QMenu")));
    }
    if (isComboPopup(w)) {
        for (QAbstractItemView *v : w->findChildren<QAbstractItemView *>()) {
            v->setAutoFillBackground(false);
            v->viewport()->setAutoFillBackground(false);
            v->setFont(menuFont(QApplication::font("QMenu")));
        }
    }
    if (auto v = qobject_cast<QAbstractItemView *>(w); v && isComboPopup(w->parentWidget())) {
        v->setAutoFillBackground(false);
        v->viewport()->setAutoFillBackground(false);
        v->setFont(menuFont(QApplication::font("QMenu")));
    }
    if (qobject_cast<QAbstractButton *>(w) || qobject_cast<QComboBox *>(w)
        || qobject_cast<QAbstractSpinBox *>(w) || qobject_cast<QTabBar *>(w)
        || qobject_cast<QScrollBar *>(w) || qobject_cast<QSlider *>(w)
        || qobject_cast<QHeaderView *>(w) || qobject_cast<QLineEdit *>(w)) {
        w->setAttribute(Qt::WA_Hover, true);
    }
}

void HearthStyle::unpolish(QWidget *w)
{
    if (w && (qobject_cast<QMenu *>(w) || isComboPopup(w) || w->inherits("QTipLabel")))
        w->setAttribute(Qt::WA_TranslucentBackground, false);
    QProxyStyle::unpolish(w);
}

// ---- primitives -------------------------------------------------------------

void HearthStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                                const QWidget *w) const
{
    const Colors c = colors(opt->palette);
    const bool enabled = opt->state & State_Enabled;

    switch (pe) {
    case PE_PanelButtonCommand: {
        bool accent = false, flat = false;
        if (auto b = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            accent = b->features & QStyleOptionButton::DefaultButton;
            flat = b->features & QStyleOptionButton::Flat;
        }
        const qreal h = opt->rect.height() - 2;
        drawButtonShape(p, opt, h <= 34 ? h / 2 : CardRadius, accent || (opt->state & State_On), flat, c);
        return;
    }
    case PE_PanelButtonBevel:
        drawButtonShape(p, opt, FieldRadius, false, false, c);
        return;
    case PE_FrameDefaultButton:
    case PE_FrameButtonBevel:
    case PE_FrameButtonTool:
    case PE_FrameLineEdit:
    case PE_FrameMenu:
    case PE_FrameTabBarBase:
    case PE_FrameStatusBarItem:
    case PE_PanelMenuBar:
    case PE_PanelToolBar:
    case PE_FrameFocusRect:
        return;
    case PE_PanelButtonTool: {
        const bool autoRaise = opt->state & State_AutoRaise;
        const bool active = opt->state & (State_Sunken | State_On | State_MouseOver);
        if (autoRaise && !active)
            return;
        const QRectF r = QRectF(opt->rect).adjusted(1, 1, -1, -1);
        QColor fill = (opt->state & State_Sunken) ? c.pressed
                    : (opt->state & State_On) ? c.selected
                    : (opt->state & State_MouseOver) ? (autoRaise ? c.hover : c.fieldHover)
                    : c.field;
        if (!enabled)
            fill = withAlpha(fill, fill.alphaF() * 0.6);
        fillRounded(p, r, FieldRadius, fill);
        if (keyboardFocus(opt))
            focusRing(p, QRectF(opt->rect), FieldRadius + 1, c);
        return;
    }
    case PE_PanelLineEdit: {
        auto f = qstyleoption_cast<const QStyleOptionFrame *>(opt);
        const QWidget *parent = w ? w->parentWidget() : nullptr;
        if (parent && (qobject_cast<const QComboBox *>(parent) || qobject_cast<const QAbstractSpinBox *>(parent)))
            return;
        if (f && f->lineWidth <= 0) {
            p->fillRect(opt->rect, opt->palette.brush(QPalette::Base));
            return;
        }
        drawField(p, opt->rect, opt, c);
        return;
    }
    case PE_IndicatorCheckBox:
    case PE_IndicatorItemViewItemCheck: {
        const int s = qMin(16, qMin(opt->rect.width(), opt->rect.height()));
        QRectF box(0, 0, s, s);
        box.moveCenter(QRectF(opt->rect).center());
        const bool on = opt->state & (State_On | State_NoChange);
        QColor fill = on ? c.accent : (c.dark ? rgba(1, 1, 1, 0.10) : c.base);
        if (opt->state & State_Sunken)
            fill = on ? c.accent.darker(115) : c.fieldPressed;
        if (!enabled)
            fill = on ? withAlpha(c.accent, 0.4) : withAlpha(fill, fill.alphaF() * 0.5);
        fillRounded(p, box.adjusted(0.5, 0.5, -0.5, -0.5), 4, fill);
        if (!on)
            strokeRounded(p, box.adjusted(0.5, 0.5, -0.5, -0.5), 4,
                          c.dark ? rgba(1, 1, 1, 0.16) : rgba(0, 0, 0, 0.24), 1);
        const QColor mark = enabled ? QColor(Qt::white) : withAlpha(Qt::white, 0.7);
        if (opt->state & State_NoChange) {
            Saver sv(p);
            p->setPen(QPen(mark, 1.8, Qt::SolidLine, Qt::RoundCap));
            p->drawLine(QPointF(box.left() + 4.5, box.center().y()), QPointF(box.right() - 4.5, box.center().y()));
        } else if (opt->state & State_On) {
            checkMark(p, box, mark);
        }
        if (keyboardFocus(opt))
            focusRing(p, box.adjusted(-2, -2, 2, 2), 6, c);
        return;
    }
    case PE_IndicatorRadioButton: {
        const int s = qMin(16, qMin(opt->rect.width(), opt->rect.height()));
        QRectF box(0, 0, s, s);
        box.moveCenter(QRectF(opt->rect).center());
        const bool on = opt->state & State_On;
        QColor fill = on ? c.accent : (c.dark ? rgba(1, 1, 1, 0.10) : c.base);
        if (opt->state & State_Sunken)
            fill = on ? c.accent.darker(115) : c.fieldPressed;
        if (!enabled)
            fill = on ? withAlpha(c.accent, 0.4) : withAlpha(fill, fill.alphaF() * 0.5);
        Saver sv(p);
        p->setPen(on ? Qt::NoPen : QPen(c.dark ? rgba(1, 1, 1, 0.16) : rgba(0, 0, 0, 0.24), 1));
        p->setBrush(fill);
        p->drawEllipse(box.adjusted(0.5, 0.5, -0.5, -0.5));
        if (on) {
            p->setPen(Qt::NoPen);
            p->setBrush(Qt::white);
            p->drawEllipse(box.center(), 3, 3);
        }
        if (keyboardFocus(opt)) {
            p->setPen(QPen(withAlpha(c.accent, 0.6), 2));
            p->setBrush(Qt::NoBrush);
            p->drawEllipse(box.adjusted(-2, -2, 2, 2).adjusted(1, 1, -1, -1));
        }
        return;
    }
    case PE_PanelMenu: {
        // Glass: tint, a darker hairline outside, a faint rim inside and a
        // specular edge brightest along the top (MacMenuBackground.qml).
        const bool dark = c.dark;
        const QRectF r = QRectF(opt->rect);
        Saver sv(p);
        p->setCompositionMode(QPainter::CompositionMode_Source);
        p->fillRect(opt->rect, Qt::transparent);
        p->setCompositionMode(QPainter::CompositionMode_SourceOver);
        fillRounded(p, r, MenuRadius, menuTint(dark));
        strokeRounded(p, r, MenuRadius, dark ? rgba(0, 0, 0, 0.28) : rgba(0, 0, 0, 0.09), 1);
        const QRectF in = r.adjusted(1, 1, -1, -1);
        strokeRounded(p, in, MenuRadius - 1, dark ? rgba(1, 1, 1, 0.05) : rgba(1, 1, 1, 0.40), 1);
        QLinearGradient spec(in.topLeft(), QPointF(in.left(), in.top() + qMin<qreal>(in.height(), 60)));
        spec.setColorAt(0, dark ? rgba(1, 1, 1, 0.20) : rgba(1, 1, 1, 0.85));
        spec.setColorAt(1, rgba(1, 1, 1, 0));
        p->setPen(QPen(QBrush(spec), 1));
        p->setBrush(Qt::NoBrush);
        p->drawRoundedRect(in.adjusted(0.5, 0.5, -0.5, -0.5), MenuRadius - 1.5, MenuRadius - 1.5);
        return;
    }
    case PE_PanelTipLabel: {
        const QRectF r = QRectF(opt->rect);
        Saver sv(p);
        p->setCompositionMode(QPainter::CompositionMode_Source);
        p->fillRect(opt->rect, Qt::transparent);
        p->setCompositionMode(QPainter::CompositionMode_SourceOver);
        const qreal radius = qRound(7 * Pt);
        fillRounded(p, r, radius, c.dark ? rgba(0.16, 0.165, 0.18, 0.97) : rgba(0.98, 0.98, 0.985, 0.97));
        strokeRounded(p, r, radius, c.dark ? rgba(0, 0, 0, 0.35) : rgba(0, 0, 0, 0.12), 1);
        strokeRounded(p, r.adjusted(1, 1, -1, -1), radius - 1, c.dark ? rgba(1, 1, 1, 0.08) : rgba(1, 1, 1, 0.6), 1);
        return;
    }
    case PE_FrameTabWidget: {
        const QRectF r = QRectF(opt->rect).adjusted(0.5, 0.5, -0.5, -0.5);
        fillRounded(p, r, CardRadius, c.dark ? rgba(1, 1, 1, 0.035) : rgba(0, 0, 0, 0.025));
        strokeRounded(p, QRectF(opt->rect), CardRadius, c.separator, 1);
        return;
    }
    case PE_FrameGroupBox: {
        const QRectF r = QRectF(opt->rect);
        fillRounded(p, r, CardRadius, c.dark ? rgba(1, 1, 1, 0.035) : rgba(0, 0, 0, 0.025));
        strokeRounded(p, r, CardRadius, c.separator, 1);
        return;
    }
    case PE_Frame: {
        auto f = qstyleoption_cast<const QStyleOptionFrame *>(opt);
        if (f && f->lineWidth <= 0)
            return;
        strokeRounded(p, QRectF(opt->rect), 3, c.separator, 1);
        return;
    }
    case PE_FrameDockWidget:
    case PE_FrameWindow:
        strokeRounded(p, QRectF(opt->rect), 0, c.separator, 1);
        return;
    case PE_PanelItemViewItem: {
        auto vi = qstyleoption_cast<const QStyleOptionViewItem *>(opt);
        if (!vi) break;
        if (vi->backgroundBrush.style() != Qt::NoBrush)
            p->fillRect(opt->rect, vi->backgroundBrush);
        if (!(opt->state & State_Selected))
            return;
        const QColor fill = selectionFill(opt, c);
        // Rounded rows in lists; square cells in tables and trees, where a
        // row is several cells.
        const bool list = !w || (qobject_cast<const QListView *>(w) && !w->inherits("QComboBoxListView"));
        if (list && vi->viewItemPosition != QStyleOptionViewItem::Middle) {
            fillRounded(p, QRectF(opt->rect).adjusted(0, 0.5, 0, -0.5), SmallRadius + 1, fill);
        } else {
            p->fillRect(opt->rect, fill);
        }
        return;
    }
    case PE_PanelItemViewRow: {
        auto vi = qstyleoption_cast<const QStyleOptionViewItem *>(opt);
        if (vi && (vi->features & QStyleOptionViewItem::Alternate))
            p->fillRect(opt->rect, opt->palette.brush(QPalette::AlternateBase));
        if (opt->state & State_Selected)
            p->fillRect(opt->rect, selectionFill(opt, c));
        return;
    }
    case PE_IndicatorBranch: {
        if (!(opt->state & State_Children))
            return;
        const QColor col = (opt->state & State_Selected) ? c.accentText : c.fg2;
        chevron(p, QRectF(opt->rect).center(), 9, 4.5,
                (opt->state & State_Open) ? Qt::DownArrow
                : (opt->direction == Qt::RightToLeft ? Qt::LeftArrow : Qt::RightArrow), col, 1.5);
        return;
    }
    case PE_IndicatorArrowUp:
    case PE_IndicatorArrowDown:
    case PE_IndicatorArrowLeft:
    case PE_IndicatorArrowRight: {
        const Qt::ArrowType dir = pe == PE_IndicatorArrowUp ? Qt::UpArrow
                                : pe == PE_IndicatorArrowDown ? Qt::DownArrow
                                : pe == PE_IndicatorArrowLeft ? Qt::LeftArrow : Qt::RightArrow;
        const qreal s = qMin<qreal>(9, qMin(opt->rect.width(), opt->rect.height()) * 0.6);
        chevron(p, QRectF(opt->rect).center(), s, s / 2, dir,
                enabled ? opt->palette.color(QPalette::ButtonText) : c.fgDisabled, 1.5);
        return;
    }
    case PE_IndicatorButtonDropDown:
        return;
    case PE_IndicatorHeaderArrow: {
        auto h = qstyleoption_cast<const QStyleOptionHeader *>(opt);
        if (!h) break;
        const Qt::ArrowType dir = h->sortIndicator == QStyleOptionHeader::SortUp ? Qt::UpArrow : Qt::DownArrow;
        chevron(p, QRectF(opt->rect).center(), 8, 4, dir, c.fg2, 1.4);
        return;
    }
    case PE_IndicatorTabClose: {
        const bool hover = enabled && (opt->state & (State_MouseOver | State_Raised));
        QRectF r(0, 0, 16, 16);
        r.moveCenter(QRectF(opt->rect).center());
        if (hover || (opt->state & State_Sunken))
            fillRounded(p, r, 8, (opt->state & State_Sunken) ? c.pressed : c.hover);
        Saver sv(p);
        p->setPen(QPen(hover ? c.fg : c.fg2, 1.3, Qt::SolidLine, Qt::RoundCap));
        const QPointF m = r.center();
        p->drawLine(m + QPointF(-3.2, -3.2), m + QPointF(3.2, 3.2));
        p->drawLine(m + QPointF(-3.2, 3.2), m + QPointF(3.2, -3.2));
        return;
    }
    case PE_IndicatorToolBarSeparator: {
        Saver sv(p);
        p->setPen(QPen(c.separator, 1));
        const QRectF r = QRectF(opt->rect);
        if (opt->state & State_Horizontal)
            p->drawLine(QPointF(r.center().x(), r.top() + 4), QPointF(r.center().x(), r.bottom() - 4));
        else
            p->drawLine(QPointF(r.left() + 4, r.center().y()), QPointF(r.right() - 4, r.center().y()));
        return;
    }
    case PE_IndicatorToolBarHandle:
        return;
    case PE_IndicatorDockWidgetResizeHandle:
        return;
    default:
        break;
    }
    QProxyStyle::drawPrimitive(pe, opt, p, w);
}

// ---- controls -------------------------------------------------------------------

static void drawMenuItem(const QStyle *style, const QStyleOptionMenuItem *mi, QPainter *p, const QWidget *w)
{
    const bool dark = colors(mi->palette).dark;
    QRect r = mi->rect;
    // A combo box's popup lays its rows edge to edge: inset them like a menu's.
    if (qobject_cast<const QComboBox *>(w))
        r.adjust(MenuPadding, 0, -MenuPadding, 0);
    const bool enabled = mi->state & QStyle::State_Enabled;
    const bool rtl = mi->direction == Qt::RightToLeft;
    p->save();
    p->setFont(mi->font);

    if (mi->menuItemType == QStyleOptionMenuItem::Separator) {
        if (!mi->text.isEmpty()) {
            // A section header: secondary, a little smaller, semibold.
            QFont f = mi->font;
            f.setPointSizeF(f.pointSizeF() * 0.85);
            f.setWeight(QFont::DemiBold);
            p->setFont(f);
            p->setPen(menuFg2(dark));
            const QRect tr = r.adjusted(MenuTextInset, 0, -MenuTextInset, 0);
            p->drawText(tr, Qt::AlignVCenter | (rtl ? Qt::AlignRight : Qt::AlignLeft) | Qt::TextSingleLine, mi->text);
        } else {
            p->setRenderHint(QPainter::Antialiasing, false);
            p->fillRect(QRect(r.left() + MenuTextInset, r.top() + r.height() / 2, r.width() - 2 * MenuTextInset, 1),
                        menuSeparator(dark));
        }
        p->restore();
        return;
    }

    const bool lit = enabled && (mi->state & QStyle::State_Selected);
    const QColor accent = mi->palette.color(QPalette::Active, QPalette::Highlight);
    if (lit)
        fillRounded(p, QRectF(r), MenuSelRadius, accent);
    const QColor textColor = !enabled ? menuFgDisabled(dark) : lit ? QColor(Qt::white) : menuFg(dark);
    const QColor keyColor = !enabled ? menuFgDisabled(dark) : lit ? QColor(Qt::white) : menuFg2(dark);

    const bool gutter = mi->menuHasCheckableItems || mi->maxIconWidth > 0
                        || mi->checkType != QStyleOptionMenuItem::NotCheckable;
    const int textLeft = gutter ? MenuGutter : MenuTextInset;
    auto vis = [&](const QRect &lr) { return QStyle::visualRect(mi->direction, r, lr); };

    // Checkmark / icon column: x 5 pt, centred 2 px left of its middle.
    const QRect column(r.left() + qRound(5 * Pt), r.top(), MenuGutter - qRound(5 * Pt), r.height());
    const QPointF cc(column.center().x() + 0.5 - 2, column.center().y() + 0.5);
    const bool checked = mi->checkType != QStyleOptionMenuItem::NotCheckable && mi->checked;
    const bool hasIcon = !mi->icon.isNull();
    if (checked && hasIcon) {
        QRectF sq(0, 0, 22, 22);
        sq.moveCenter(QPointF(vis(column.adjusted(0, 0, 0, 0)).center().x() + (rtl ? 2 : -2) + 0.5, cc.y()));
        fillRounded(p, sq, 5, lit ? rgba(1, 1, 1, 0.25) : withAlpha(accent, dark ? 0.35 : 0.25));
    }
    if (hasIcon) {
        const int is = 16;
        const QPixmap pm = mi->icon.pixmap(QSize(is, is), enabled ? QIcon::Normal : QIcon::Disabled,
                                           checked ? QIcon::On : QIcon::Off);
        QRect ir(0, 0, is, is);
        const QRect cv = vis(column);
        ir.moveCenter(QPoint(cv.center().x() + (rtl ? 2 : -2), cv.center().y()));
        p->drawPixmap(ir, pm);
    } else if (checked) {
        const QRect cv = vis(column);
        const QPointF m(cv.center().x() + (rtl ? 2 : -2) + 0.5, cv.center().y() + 0.5);
        if (mi->checkType == QStyleOptionMenuItem::Exclusive) {
            Saver sv(p);
            p->setPen(Qt::NoPen);
            p->setBrush(textColor);
            p->drawEllipse(m, 3.2, 3.2);
        } else {
            checkMark(p, QRectF(m.x() - 7, m.y() - 7, 14, 14), textColor, 1.7);
        }
    }

    // Label and shortcut ("Open\tCtrl+O").
    QString text = mi->text;
    QString key;
    const int tab = text.indexOf(QLatin1Char('\t'));
    if (tab >= 0) {
        key = text.mid(tab + 1);
        text = text.left(tab);
    }
    const bool sub = mi->menuItemType == QStyleOptionMenuItem::SubMenu;
    int textFlags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
    if (!style->styleHint(QStyle::SH_UnderlineShortcut, mi, w))
        textFlags |= Qt::TextHideMnemonic;
    int right = r.right() - MenuTextInset;
    QFont keyFont = mi->font;
    keyFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.6);
    if (!key.isEmpty()) {
        p->setFont(keyFont);
        p->setPen(keyColor);
        const QRect kr(r.left(), r.top(), right - r.left() + 1, r.height());
        p->drawText(vis(kr), Qt::AlignVCenter | (rtl ? Qt::AlignLeft : Qt::AlignRight) | Qt::TextSingleLine, key);
        right -= QFontMetrics(keyFont).horizontalAdvance(key) + 16;
    }
    if (sub) {
        const QRect ar(r.right() - MenuTextInset - 6, r.top(), 6, r.height());
        const QRect av = vis(ar);
        chevron(p, QPointF(av.center().x() + 0.5, av.center().y() + 0.5), 9.4, 4.4,
                rtl ? Qt::LeftArrow : Qt::RightArrow, textColor, 1.6);
        right -= 20;
    }
    p->setFont(mi->font);
    p->setPen(textColor);
    const QRect tr(r.left() + textLeft, r.top(), qMax(0, right - (r.left() + textLeft) + 1), r.height());
    const QString shown = QFontMetrics(mi->font).elidedText(text, Qt::ElideRight, tr.width(), Qt::TextShowMnemonic);
    // (A 0.001 px nudge: Qt's native text drops a pixel row at exact half
    // pixels at fractional scales.)
    p->translate(0.001, 0.001);
    p->drawText(vis(tr), textFlags | (rtl ? Qt::AlignRight : Qt::AlignLeft), shown);
    p->restore();
}

static void drawTabShape(const QStyleOptionTab *tab, QPainter *p, const Colors &c)
{
    const bool vertical = isVertical(tab->shape);
    const bool rtl = !vertical && tab->direction == Qt::RightToLeft;
    const QRectF r = QRectF(tab->rect);
    const qreal radius = FieldRadius;
    bool roundStart = false, roundEnd = false;
    switch (tab->position) {
    case QStyleOptionTab::Beginning: roundStart = true; break;
    case QStyleOptionTab::End: roundEnd = true; break;
    case QStyleOptionTab::OnlyOneTab: roundStart = roundEnd = true; break;
    default: break;
    }
    if (rtl)
        std::swap(roundStart, roundEnd);

    // The track: this tab's piece of one rounded bar.
    {
        Saver sv(p);
        p->setClipRect(r);
        QRectF tr = r;
        if (vertical) {
            if (!roundStart) tr.setTop(tr.top() - radius * 2);
            if (!roundEnd) tr.setBottom(tr.bottom() + radius * 2);
        } else {
            if (!roundStart) tr.setLeft(tr.left() - radius * 2);
            if (!roundEnd) tr.setRight(tr.right() + radius * 2);
        }
        p->setPen(Qt::NoPen);
        p->setBrush(c.track);
        p->drawRoundedRect(tr, radius, radius);
    }

    const bool selected = tab->state & QStyle::State_Selected;
    const bool enabled = tab->state & QStyle::State_Enabled;
    const QRectF seg = r.adjusted(2, 2, -2, -2);
    if (selected) {
        const QColor fill = c.dark ? mix(c.track, Qt::white, 0.20) : QColor(Qt::white);
        if (!c.dark)
            fillRounded(p, seg.translated(0, 0.5), radius - 2, rgba(0, 0, 0, 0.10));
        fillRounded(p, seg, radius - 2, enabled ? fill : withAlpha(fill, 0.6));
    } else if (enabled && (tab->state & QStyle::State_MouseOver)) {
        fillRounded(p, seg, radius - 2, c.hover);
    }

    // A divider at this tab's end, unless it or its neighbour is selected.
    const bool last = tab->position == QStyleOptionTab::End || tab->position == QStyleOptionTab::OnlyOneTab;
    if (!selected && !last && tab->selectedPosition != QStyleOptionTab::NextIsSelected) {
        Saver sv(p);
        p->setRenderHint(QPainter::Antialiasing, false);
        if (vertical)
            p->fillRect(QRectF(r.left() + 6, r.bottom(), r.width() - 12, 1), c.separator);
        else
            p->fillRect(QRectF(rtl ? r.left() : r.right(), r.top() + 6, 1, r.height() - 12), c.separator);
    }
}

void HearthStyle::drawControl(ControlElement ce, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    const Colors c = colors(opt->palette);
    const bool enabled = opt->state & State_Enabled;

    switch (ce) {
    case CE_PushButtonBevel: {
        auto b = qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (!b) break;
        proxy()->drawPrimitive(PE_PanelButtonCommand, b, p, w);
        if (b->features & QStyleOptionButton::HasMenu) {
            const int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, b, w);
            QRect ir(b->rect.right() - mbi - 6, b->rect.top(), mbi, b->rect.height());
            ir = visualRect(b->direction, b->rect, ir);
            const bool accent = b->features & QStyleOptionButton::DefaultButton;
            chevron(p, QRectF(ir).center(), 8, 4, Qt::DownArrow, accent ? c.accentText : c.fg2, 1.5);
        }
        return;
    }
    case CE_PushButtonLabel: {
        auto b = qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (!b) break;
        QStyleOptionButton copy = *b;
        if ((b->features & QStyleOptionButton::DefaultButton) || (b->state & State_On)) {
            if (enabled)
                copy.palette.setColor(QPalette::ButtonText, c.accentText);
        } else {
            copy.palette.setColor(QPalette::ButtonText, opt->palette.color(QPalette::WindowText));
        }
        copy.state &= ~State_HasFocus;
        QProxyStyle::drawControl(ce, &copy, p, w);
        return;
    }
    case CE_MenuItem:
        if (auto mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            drawMenuItem(this, mi, p, w);
            return;
        }
        break;
    case CE_MenuEmptyArea:
        return;
    case CE_MenuBarEmptyArea:
        p->fillRect(opt->rect, opt->palette.window());
        return;
    case CE_MenuBarItem: {
        auto mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        if (!mi) break;
        const bool active = enabled && (opt->state & (State_Selected | State_Sunken));
        if (active)
            fillRounded(p, QRectF(opt->rect).adjusted(0, 2, 0, -2), SmallRadius + 1,
                        (opt->state & State_Sunken) ? c.pressed : c.hover);
        int flags = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
        if (!styleHint(SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;
        p->save();
        p->setPen(enabled ? c.fg : c.fgDisabled);
        p->drawText(opt->rect, flags, mi->text);
        p->restore();
        return;
    }
    case CE_TabBarTabShape:
        if (auto tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            drawTabShape(tab, p, c);
            return;
        }
        break;
    case CE_TabBarTabLabel:
        if (auto tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            QStyleOptionTab copy = *tab;
            copy.palette.setColor(QPalette::WindowText, enabled ? c.fg : c.fgDisabled);
            copy.palette.setColor(QPalette::ButtonText, enabled ? c.fg : c.fgDisabled);
            copy.state &= ~State_HasFocus;
            QProxyStyle::drawControl(ce, &copy, p, w);
            return;
        }
        break;
    case CE_HeaderSection: {
        auto h = qstyleoption_cast<const QStyleOptionHeader *>(opt);
        p->fillRect(opt->rect, opt->palette.base());
        if (enabled && (opt->state & State_Sunken))
            p->fillRect(opt->rect, c.pressed);
        else if (enabled && (opt->state & State_MouseOver))
            p->fillRect(opt->rect, c.hover);
        Saver sv(p);
        p->setRenderHint(QPainter::Antialiasing, false);
        const QRect r = opt->rect;
        if (h && h->orientation == Qt::Horizontal) {
            p->fillRect(QRect(r.left(), r.bottom(), r.width(), 1), c.separator);
            if (h->position != QStyleOptionHeader::End && h->position != QStyleOptionHeader::OnlyOneSection) {
                const int x = opt->direction == Qt::RightToLeft ? r.left() : r.right();
                p->fillRect(QRect(x, r.top() + 5, 1, r.height() - 10), c.separator);
            }
        } else {
            const int x = opt->direction == Qt::RightToLeft ? r.left() : r.right();
            p->fillRect(QRect(x, r.top(), 1, r.height()), c.separator);
        }
        return;
    }
    case CE_HeaderEmptyArea:
        p->fillRect(opt->rect, opt->palette.base());
        {
            Saver sv(p);
            p->setRenderHint(QPainter::Antialiasing, false);
            p->fillRect(QRect(opt->rect.left(), opt->rect.bottom(), opt->rect.width(), 1), c.separator);
        }
        return;
    case CE_HeaderLabel:
        if (auto h = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            QStyleOptionHeader copy = *h;
            copy.palette.setColor(QPalette::ButtonText, enabled ? c.fg2 : c.fgDisabled);
            QProxyStyle::drawControl(ce, &copy, p, w);
            return;
        }
        break;
    case CE_ProgressBar:
        if (auto pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            QStyleOptionProgressBar sub = *pb;
            sub.rect = proxy()->subElementRect(SE_ProgressBarGroove, pb, w);
            proxy()->drawControl(CE_ProgressBarGroove, &sub, p, w);
            sub.rect = proxy()->subElementRect(SE_ProgressBarContents, pb, w);
            proxy()->drawControl(CE_ProgressBarContents, &sub, p, w);
            if (pb->textVisible) {
                sub.rect = proxy()->subElementRect(SE_ProgressBarLabel, pb, w);
                proxy()->drawControl(CE_ProgressBarLabel, &sub, p, w);
            }
            return;
        }
        break;
    case CE_ProgressBarGroove: {
        const QRectF r = QRectF(opt->rect);
        const bool horiz = opt->state & State_Horizontal;
        QRectF g = r;
        if (horiz) { g.setHeight(6); g.moveCenter(r.center()); }
        else { g.setWidth(6); g.moveCenter(r.center()); }
        fillRounded(p, g, 3, c.track);
        return;
    }
    case CE_ProgressBarContents: {
        auto pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        if (!pb) break;
        const QRectF r = QRectF(opt->rect);
        const bool horiz = opt->state & State_Horizontal;
        QRectF g = r;
        if (horiz) { g.setHeight(6); g.moveCenter(r.center()); }
        else { g.setWidth(6); g.moveCenter(r.center()); }
        const QColor accent = enabled ? c.accent : withAlpha(c.accent, 0.4);
        if (pb->minimum == 0 && pb->maximum == 0) {
            fillRounded(p, g, 3, withAlpha(accent, 0.45));
            return;
        }
        const qreal span = qreal(pb->maximum) - pb->minimum;
        const qreal frac = span > 0 ? qBound<qreal>(0, (qreal(pb->progress) - pb->minimum) / span, 1) : 0;
        if (frac <= 0)
            return;
        QRectF f = g;
        bool reverse = pb->invertedAppearance;
        if (horiz) {
            if (opt->direction == Qt::RightToLeft) reverse = !reverse;
            const qreal len = qMax<qreal>(6, g.width() * frac);
            f.setWidth(len);
            if (reverse) f.moveRight(g.right());
        } else {
            const qreal len = qMax<qreal>(6, g.height() * frac);
            if (pb->bottomToTop != reverse) { f.setTop(g.bottom() - len); }
            else f.setHeight(len);
        }
        fillRounded(p, f, 3, accent);
        return;
    }
    case CE_ProgressBarLabel: {
        auto pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        if (!pb || !pb->textVisible || !(opt->state & State_Horizontal)) return;
        p->save();
        p->setPen(enabled ? c.fg2 : c.fgDisabled);
        p->drawText(opt->rect, Qt::AlignVCenter | (opt->direction == Qt::RightToLeft ? Qt::AlignLeft : Qt::AlignRight), pb->text);
        p->restore();
        return;
    }
    case CE_Splitter: {
        Saver sv(p);
        p->setRenderHint(QPainter::Antialiasing, false);
        const QRect r = opt->rect;
        if (opt->state & State_Horizontal)
            p->fillRect(QRect(r.center().x(), r.top(), 1, r.height()), c.separator);
        else
            p->fillRect(QRect(r.left(), r.center().y(), r.width(), 1), c.separator);
        return;
    }
    case CE_ToolBar:
        return;
    case CE_RubberBand: {
        const QRectF r = QRectF(opt->rect);
        fillRounded(p, r, 4, withAlpha(c.accent, 0.18));
        strokeRounded(p, r, 4, withAlpha(c.accent, 0.6), 1);
        return;
    }
    case CE_ComboBoxLabel:
        if (auto cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            QStyleOptionComboBox copy = *cb;
            copy.palette.setColor(QPalette::ButtonText, enabled ? c.fg : c.fgDisabled);
            copy.palette.setColor(QPalette::Text, enabled ? c.fg : c.fgDisabled);
            QProxyStyle::drawControl(ce, &copy, p, w);
            return;
        }
        break;
    case CE_ItemViewItem:
        if (auto vi = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            if ((vi->state & State_Selected) && !(vi->state & State_Active)) {
                // Grey selection: the normal text colour on it.
                QStyleOptionViewItem copy = *vi;
                for (auto g : {QPalette::Active, QPalette::Inactive})
                    copy.palette.setColor(g, QPalette::HighlightedText, vi->palette.color(g, QPalette::Text));
                QProxyStyle::drawControl(ce, &copy, p, w);
                return;
            }
        }
        break;
    case CE_ScrollBarAddLine:
    case CE_ScrollBarSubLine:
    case CE_ScrollBarFirst:
    case CE_ScrollBarLast:
    case CE_ScrollBarAddPage:
    case CE_ScrollBarSubPage:
        return;
    default:
        break;
    }
    QProxyStyle::drawControl(ce, opt, p, w);
}

// ---- complex controls ---------------------------------------------------------

void HearthStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                     const QWidget *w) const
{
    const Colors c = colors(opt->palette);
    const bool enabled = opt->state & State_Enabled;

    switch (cc) {
    case CC_ScrollBar: {
        auto sb = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!sb) break;
        const bool horiz = sb->orientation == Qt::Horizontal;
        const bool hover = enabled && (opt->state & State_MouseOver);
        const bool pressed = (opt->activeSubControls & SC_ScrollBarSlider) && (opt->state & State_Sunken);
        if (hover) {
            Saver sv(p);
            p->setRenderHint(QPainter::Antialiasing, false);
            p->fillRect(opt->rect, withAlpha(c.fg, 0.04));
            p->fillRect(horiz ? QRect(opt->rect.left(), opt->rect.top(), opt->rect.width(), 1)
                              : QRect(opt->direction == Qt::RightToLeft ? opt->rect.right() : opt->rect.left(),
                                      opt->rect.top(), 1, opt->rect.height()),
                        withAlpha(c.fg, 0.07));
        }
        if (sb->maximum == sb->minimum)
            return;
        const QRect slider = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, w);
        if (!slider.isValid())
            return;
        const qreal thick = (hover || pressed) ? 8 : 6;
        QRectF t = QRectF(slider);
        if (horiz) {
            t.setTop(t.bottom() + 1 - 2 - thick);
            t.setHeight(thick);
            t.adjust(2, 0, -2, 0);
        } else {
            if (opt->direction == Qt::RightToLeft)
                t.setLeft(t.left() + 2);
            else
                t.setLeft(t.right() + 1 - 2 - thick);
            t.setWidth(thick);
            t.adjust(0, 2, 0, -2);
        }
        const qreal a = pressed ? 0.6 : hover ? 0.48 : 0.34;
        fillRounded(p, t, thick / 2, withAlpha(c.fg, enabled ? a : a / 2));
        return;
    }
    case CC_ComboBox: {
        auto cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        if (!cb) break;
        const QRect arrow = subControlRect(CC_ComboBox, cb, SC_ComboBoxArrow, w);
        if (cb->editable) {
            drawField(p, opt->rect, opt, c);
        } else if (cb->frame) {
            QStyleOption o = *opt;
            o.state &= ~State_On;
            if (opt->activeSubControls == SC_None)
                o.state &= ~State_Sunken;
            drawButtonShape(p, &o, FieldRadius, false, false, c);
        } else if (enabled && (opt->state & State_MouseOver)) {
            fillRounded(p, QRectF(opt->rect).adjusted(1, 1, -1, -1), FieldRadius, c.hover);
        }
        updown(p, QPointF(arrow.center().x() + 0.5, arrow.center().y() + 0.5), enabled ? c.fg2 : c.fgDisabled);
        return;
    }
    case CC_SpinBox: {
        auto sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt);
        if (!sb) break;
        if (sb->frame)
            drawField(p, opt->rect, opt, c);
        if (sb->buttonSymbols != QAbstractSpinBox::NoButtons) {
            const QRect up = subControlRect(CC_SpinBox, sb, SC_SpinBoxUp, w);
            const QRect down = subControlRect(CC_SpinBox, sb, SC_SpinBoxDown, w);
            auto col = [&](bool on, SubControl sc) {
                if (!enabled || !on) return c.fgDisabled;
                if ((opt->activeSubControls & sc) && (opt->state & State_Sunken)) return c.fg;
                return c.fg2;
            };
            const QColor cu = col(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled, SC_SpinBoxUp);
            const QColor cd = col(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled, SC_SpinBoxDown);
            if (sb->buttonSymbols == QAbstractSpinBox::PlusMinus) {
                Saver sv(p);
                p->setPen(QPen(cu, 1.4, Qt::SolidLine, Qt::RoundCap));
                QPointF m = QRectF(up).center();
                p->drawLine(m + QPointF(-3, 0), m + QPointF(3, 0));
                p->drawLine(m + QPointF(0, -3), m + QPointF(0, 3));
                p->setPen(QPen(cd, 1.4, Qt::SolidLine, Qt::RoundCap));
                m = QRectF(down).center();
                p->drawLine(m + QPointF(-3, 0), m + QPointF(3, 0));
            } else {
                chevron(p, QPointF(QRectF(up).center().x(), up.bottom() - 2.5), 6.5, 3.5, Qt::UpArrow, cu, 1.4);
                chevron(p, QPointF(QRectF(down).center().x(), down.top() + 3), 6.5, 3.5, Qt::DownArrow, cd, 1.4);
            }
        }
        return;
    }
    case CC_Slider: {
        auto s = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!s) break;
        const bool horiz = s->orientation == Qt::Horizontal;
        const QRect groove = subControlRect(CC_Slider, s, SC_SliderGroove, w);
        const QRect handle = subControlRect(CC_Slider, s, SC_SliderHandle, w);
        if (s->subControls & SC_SliderTickmarks && s->tickPosition != QSlider::NoTicks) {
            QStyleOptionSlider t = *s;
            t.subControls = SC_SliderTickmarks;
            QProxyStyle::drawComplexControl(cc, &t, p, w);
        }
        if (s->subControls & SC_SliderGroove) {
            QRectF g = QRectF(groove);
            const QPointF hc = QRectF(handle).center();
            if (horiz) { g.setHeight(4); g.moveCenter(QPointF(groove.center().x() + 0.5, hc.y())); }
            else { g.setWidth(4); g.moveCenter(QPointF(hc.x(), groove.center().y() + 0.5)); }
            fillRounded(p, g, 2, c.track);
            QRectF f = g;
            const QColor accent = enabled ? c.accent : withAlpha(c.accent, 0.4);
            if (horiz) {
                const bool rev = (s->upsideDown) != (s->direction == Qt::RightToLeft);
                if (rev) f.setLeft(hc.x()); else f.setRight(hc.x());
            } else {
                if (s->upsideDown) f.setBottom(hc.y()); else f.setTop(hc.y());
            }
            fillRounded(p, f, 2, accent);
        }
        if (s->subControls & SC_SliderHandle) {
            const qreal d = qMin(20, qMin(handle.width(), handle.height()));
            QRectF k(0, 0, d - 1, d - 1);
            k.moveCenter(QRectF(handle).center());
            Saver sv(p);
            p->setPen(Qt::NoPen);
            p->setBrush(rgba(0, 0, 0, c.dark ? 0.35 : 0.14));
            p->drawEllipse(k.translated(0, 0.75).adjusted(-0.5, -0.5, 0.5, 0.5));
            QColor knob = c.dark ? QColor("#ececee") : QColor(Qt::white);
            if ((opt->activeSubControls & SC_SliderHandle) && (opt->state & State_Sunken))
                knob = knob.darker(108);
            if (!enabled)
                knob = mix(c.window, knob, 0.6);
            p->setBrush(knob);
            p->drawEllipse(k);
            if (keyboardFocus(opt)) {
                p->setPen(QPen(withAlpha(c.accent, 0.6), 2));
                p->setBrush(Qt::NoBrush);
                p->drawEllipse(k.adjusted(-1.5, -1.5, 1.5, 1.5));
            }
        }
        return;
    }
    default:
        break;
    }
    QProxyStyle::drawComplexControl(cc, opt, p, w);
}

QRect HearthStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                                  const QWidget *w) const
{
    switch (cc) {
    case CC_ScrollBar: {
        auto sb = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!sb) break;
        const QRect r = sb->rect;
        const bool horiz = sb->orientation == Qt::Horizontal;
        const int len = horiz ? r.width() : r.height();
        int sliderLen = len;
        if (sb->maximum != sb->minimum) {
            const qint64 range = qint64(sb->maximum) - sb->minimum;
            sliderLen = int((qint64(sb->pageStep) * len) / (range + sb->pageStep));
            const int minLen = proxy()->pixelMetric(PM_ScrollBarSliderMin, sb, w);
            if (sliderLen < minLen || range > INT_MAX / 2)
                sliderLen = minLen;
            sliderLen = qMin(sliderLen, len);
        }
        const int pos = sliderPositionFromValue(sb->minimum, sb->maximum, sb->sliderPosition,
                                                len - sliderLen, sb->upsideDown);
        QRect out;
        switch (sc) {
        case SC_ScrollBarGroove:
            out = r;
            break;
        case SC_ScrollBarSlider:
            out = horiz ? QRect(r.x() + pos, r.y(), sliderLen, r.height())
                        : QRect(r.x(), r.y() + pos, r.width(), sliderLen);
            break;
        case SC_ScrollBarSubPage:
            out = horiz ? QRect(r.x(), r.y(), pos, r.height()) : QRect(r.x(), r.y(), r.width(), pos);
            break;
        case SC_ScrollBarAddPage:
            out = horiz ? QRect(r.x() + pos + sliderLen, r.y(), len - pos - sliderLen, r.height())
                        : QRect(r.x(), r.y() + pos + sliderLen, r.width(), len - pos - sliderLen);
            break;
        default:
            return QRect();
        }
        return horiz ? visualRect(sb->direction, r, out) : out;
    }
    case CC_ComboBox: {
        auto cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        if (!cb) break;
        const QRect r = cb->rect;
        QRect out;
        switch (sc) {
        case SC_ComboBoxFrame:
        case SC_ComboBoxListBoxPopup:
            return r;
        case SC_ComboBoxArrow:
            out = QRect(r.right() - 22, r.top(), 22, r.height());
            break;
        case SC_ComboBoxEditField:
            out = cb->editable ? r.adjusted(7, 2, -24, -2) : r.adjusted(11, 1, -24, -1);
            break;
        default:
            break;
        }
        if (out.isValid())
            return visualRect(cb->direction, r, out);
        break;
    }
    case CC_SpinBox: {
        auto sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt);
        if (!sb) break;
        const QRect r = sb->rect;
        const int bw = sb->buttonSymbols == QAbstractSpinBox::NoButtons ? 0 : 20;
        QRect out;
        switch (sc) {
        case SC_SpinBoxFrame:
            return r;
        case SC_SpinBoxEditField:
            out = bw ? QRect(r.left() + 7, r.top() + 2, r.width() - 7 - bw, r.height() - 4)
                     : r.adjusted(7, 2, -7, -2);
            break;
        case SC_SpinBoxUp:
            if (!bw) return QRect();
            out = QRect(r.right() - bw + 1, r.top() + 2, bw - 3, r.height() / 2 - 2);
            break;
        case SC_SpinBoxDown:
            if (!bw) return QRect();
            out = QRect(r.right() - bw + 1, r.top() + r.height() / 2, bw - 3, r.height() - r.height() / 2 - 2);
            break;
        default:
            break;
        }
        if (out.isValid())
            return visualRect(sb->direction, r, out);
        break;
    }
    default:
        break;
    }
    return QProxyStyle::subControlRect(cc, opt, sc, w);
}

QRect HearthStyle::subElementRect(SubElement se, const QStyleOption *opt, const QWidget *w) const
{
    switch (se) {
    case SE_ProgressBarGroove:
    case SE_ProgressBarContents:
    case SE_ProgressBarLabel: {
        auto pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        if (!pb || !(opt->state & State_Horizontal))
            break;
        QRect r = opt->rect;
        if (!pb->textVisible)
            return se == SE_ProgressBarLabel ? QRect() : r;
        const int labelW = opt->fontMetrics.horizontalAdvance(QStringLiteral("100%")) + 10;
        QRect bar(r.left(), r.top(), r.width() - labelW, r.height());
        QRect label(r.right() - labelW + 1, r.top(), labelW, r.height());
        return visualRect(opt->direction, r, se == SE_ProgressBarLabel ? label : bar);
    }
    case SE_ShapedFrameContents:
        // A combo box's popup: the menu's padding above and below its rows.
        if (isComboPopup(w))
            return opt->rect.adjusted(0, MenuPadding, 0, -MenuPadding);
        break;
    case SE_LineEditContents: {
        QRect r = QProxyStyle::subElementRect(se, opt, w);
        auto f = qstyleoption_cast<const QStyleOptionFrame *>(opt);
        if (f && f->lineWidth > 0)
            r.adjust(4, 0, -4, 0);
        return r;
    }
    case SE_TabWidgetTabPane: {
        auto tw = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
        QRect r = QProxyStyle::subElementRect(se, opt, w);
        auto tabw = qobject_cast<const QTabWidget *>(w);
        if (tw && (!tabw || !tabw->documentMode()) && tw->tabBarSize.height() > 0
            && (tw->shape == QTabBar::RoundedNorth || tw->shape == QTabBar::TriangularNorth)) {
            // macOS: the tab pane's box starts behind the middle of the tabs.
            r.setTop(r.top() - tw->tabBarSize.height() / 2);
        }
        return r;
    }
    case SE_TabWidgetTabContents: {
        QRect r = QProxyStyle::subElementRect(se, opt, w);
        auto tw = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
        if (tw && baseStyle()) {
            const QRect pane = baseStyle()->subElementRect(SE_TabWidgetTabPane, opt, w);
            if (tw->shape == QTabBar::RoundedNorth || tw->shape == QTabBar::TriangularNorth)
                r.setTop(qMax(r.top(), pane.top() + 4));
            r.adjust(4, 2, -4, -4);
        }
        return r;
    }
    default:
        break;
    }
    return QProxyStyle::subElementRect(se, opt, w);
}

// ---- metrics -------------------------------------------------------------------

int HearthStyle::pixelMetric(PixelMetric pm, const QStyleOption *opt, const QWidget *w) const
{
    switch (pm) {
    case PM_ScrollBarExtent: return 12;
    case PM_ScrollBarSliderMin: return 32;
    case PM_ScrollView_ScrollBarSpacing: return 0;
    case PM_ScrollView_ScrollBarOverlap: return 0;
    case PM_MenuPanelWidth: return 0;
    case PM_MenuHMargin: return MenuPadding;
    case PM_MenuVMargin: return MenuPadding;
    case PM_MenuDesktopFrameWidth: return 0;
    case PM_SubMenuOverlap: return 1;
    case PM_MenuBarPanelWidth: return 0;
    case PM_MenuBarItemSpacing: return 2;
    case PM_MenuBarVMargin: return 2;
    case PM_MenuBarHMargin: return 4;
    case PM_SmallIconSize: return 16;
    case PM_ButtonIconSize: return 16;
    case PM_ButtonMargin: return 14;
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical: return 0;
    case PM_MenuButtonIndicator: return 12;
    case PM_DefaultFrameWidth: return 1;
    case PM_ComboBoxFrameWidth: return 2;
    case PM_SpinBoxFrameWidth: return 2;
    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight: return 16;
    case PM_CheckBoxLabelSpacing:
    case PM_RadioButtonLabelSpacing: return 7;
    case PM_SliderThickness: return 22;
    case PM_SliderLength: return 20;
    case PM_SliderControlThickness: return 20;
    case PM_TabBarTabHSpace: return 26;
    case PM_TabBarTabVSpace: return 10;
    case PM_TabBarTabShiftHorizontal:
    case PM_TabBarTabShiftVertical: return 0;
    case PM_TabBarBaseOverlap: return 0;
    case PM_TabBarBaseHeight: return 0;
    case PM_TabBarTabOverlap: return 0;
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight: return 16;
    case PM_ToolTipLabelFrameWidth: return 6;
    case PM_ToolBarFrameWidth: return 0;
    case PM_ToolBarItemSpacing: return 4;
    case PM_ToolBarItemMargin: return 2;
    case PM_ToolBarSeparatorExtent: return 9;
    case PM_ToolBarHandleExtent: return 6;
    case PM_SplitterWidth: return 5;
    case PM_HeaderMargin: return 6;
    case PM_DockWidgetTitleBarButtonMargin: return 4;
    case PM_FocusFrameHMargin:
    case PM_FocusFrameVMargin: return 2;
    default:
        break;
    }
    return QProxyStyle::pixelMetric(pm, opt, w);
}

QSize HearthStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &size,
                                    const QWidget *w) const
{
    switch (ct) {
    case CT_PushButton: {
        auto b = qstyleoption_cast<const QStyleOptionButton *>(opt);
        QSize s(size.width() + 2 * 14 + 2, qMax(size.height() + 10, ButtonHeight));
        if (b && !b->text.isEmpty() && !(b->features & QStyleOptionButton::Flat))
            s.setWidth(qMax(s.width(), qRound(76 * Pt)));
        return s;
    }
    case CT_LineEdit: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        s.setHeight(qMax(s.height() + 2, ControlHeight));
        s.rwidth() += 8;
        return s;
    }
    case CT_ComboBox: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        s.setHeight(qMax(s.height(), ControlHeight));
        s.rwidth() += 12;
        return s;
    }
    case CT_SpinBox: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        s.setHeight(qMax(s.height(), ControlHeight));
        s.rwidth() += 8;
        return s;
    }
    case CT_MenuItem: {
        auto mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
        if (!mi) break;
        if (mi->menuItemType == QStyleOptionMenuItem::Separator)
            return QSize(size.width(), mi->text.isEmpty() ? MenuSeparator : MenuHeader);
        const bool gutter = mi->menuHasCheckableItems || mi->maxIconWidth > 0
                            || mi->checkType != QStyleOptionMenuItem::NotCheckable;
        // (A combo box's popup passes the whole view's size: measure the text.)
        const bool combo = qobject_cast<const QComboBox *>(w);
        int textWidth = size.width();
        int height = size.height();
        if (combo) {
            textWidth = mi->fontMetrics.horizontalAdvance(mi->text);
            height = qMax(mi->fontMetrics.height(), mi->icon.isNull() ? 0 : 16) + 8;
        }
        int width = (gutter ? MenuGutter : MenuTextInset) + textWidth + MenuTextInset;
        if (mi->text.contains(QLatin1Char('\t')))
            width += 16 + 5;    // the gap before the shortcut, and its looser spacing
        if (mi->menuItemType == QStyleOptionMenuItem::SubMenu)
            width += 20;
        // A combo box's popup: its rows are inset like a menu's.
        if (combo)
            width += 2 * MenuPadding;
        else
            width = qMax(width, MenuMinWidth - 2 * MenuPadding);
        return QSize(width, qMax(MenuRow, height));
    }
    case CT_MenuBarItem: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        return QSize(size.width() + 20, qMax(s.height(), 24));
    }
    case CT_TabBarTab: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        auto tab = qstyleoption_cast<const QStyleOptionTab *>(opt);
        if (tab && isVertical(tab->shape))
            s.setWidth(qMax(s.width(), 26));
        else
            s.setHeight(qMax(s.height(), 26));
        return s;
    }
    case CT_ToolButton: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        return QSize(qMax(s.width(), 28), qMax(s.height(), 28));
    }
    case CT_HeaderSection: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        s.setHeight(qMax(s.height(), 24));
        return s;
    }
    case CT_ProgressBar: {
        QSize s = QProxyStyle::sizeFromContents(ct, opt, size, w);
        auto pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        if (pb && (opt->state & State_Horizontal))
            s.setHeight(qMax(16, opt->fontMetrics.height()));
        return s;
    }
    default:
        break;
    }
    return QProxyStyle::sizeFromContents(ct, opt, size, w);
}

int HearthStyle::styleHint(StyleHint sh, const QStyleOption *opt, const QWidget *w, QStyleHintReturn *ret) const
{
    switch (sh) {
    case SH_ComboBox_Popup:
        return 1;
    case SH_ComboBox_PopupFrameStyle:
        return QFrame::NoFrame | QFrame::Plain;
    case SH_ComboBox_ListMouseTracking:
    case SH_Menu_MouseTracking:
    case SH_MenuBar_MouseTracking:
        return 1;
    case SH_Menu_FlashTriggeredItem:
        return 1;
    case SH_TabBar_Alignment: {
        auto tw = qobject_cast<const QTabWidget *>(w);
        if (tw && tw->documentMode())
            return Qt::AlignLeft;
        auto tb = qobject_cast<const QTabBar *>(w);
        if (tb && tb->documentMode())
            return Qt::AlignLeft;
        return Qt::AlignCenter;
    }
    case SH_ItemView_ShowDecorationSelected:
        return 1;
    case SH_ScrollView_FrameOnlyAroundContents:
        return 0;
    case SH_DialogButtonBox_ButtonsHaveIcons:
        return 0;
    case SH_Widget_Animate:
        return 0;
    case SH_Slider_AbsoluteSetButtons:
        return Qt::LeftButton;
    case SH_Slider_PageSetButtons:
        return Qt::MiddleButton;
    case SH_Table_GridLineColor:
        if (opt)
            return int(colors(opt->palette).separator.rgba());
        break;
    case SH_ToolTip_Mask:
    case SH_Menu_Mask:
        return 0;
    case SH_ScrollBar_Transient:
        return 0;
    case SH_FormLayoutLabelAlignment:
        return Qt::AlignRight | Qt::AlignVCenter;
    default:
        break;
    }
    return QProxyStyle::styleHint(sh, opt, w, ret);
}
