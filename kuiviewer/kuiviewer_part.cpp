/*
    SPDX-FileCopyrightText: 2003 Richard Moore <rich@kde.org>
    SPDX-FileCopyrightText: 2003 Ian Reinhart Geiser <geiseri@kde.org>
    SPDX-FileCopyrightText: 2017 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#define TRANSLATION_DOMAIN "kuiviewer"

#include "kuiviewer_part.h"

// part
#include <kuiviewer_part_debug.h>
#include "kuiviewer_version.h"
// KF
#include <KActionCollection>
#include <KSelectAction>
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KPluginMetaData>
#include <KPluginFactory>

// Qt
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFormBuilder>
#include <QStyle>
#include <QStyleFactory>
#include <QScrollArea>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMimeDatabase>
#include <QBuffer>


K_PLUGIN_FACTORY_WITH_JSON(KUIViewerPartFactory, "kuiviewer_part.json",
                           registerPlugin<KUIViewerPart>();)

KUIViewerPart::KUIViewerPart(QWidget* parentWidget,
                             QObject* parent,
                             const KPluginMetaData& metaData,
                             const QVariantList& /*args*/)
    : KParts::ReadOnlyPart(parent)
    , m_subWindow(nullptr)
    , m_view(nullptr)
{
    setMetaData(metaData);

    // this should be your custom internal widget
    m_widget = new QMdiArea(parentWidget);
    m_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // notify the part that this is our internal widget
    setWidget(m_widget);

    // set our XML-UI resource file
    setXMLFile(QStringLiteral("kuiviewer_part.rc"));

    m_style = actionCollection()->add<KSelectAction>(QStringLiteral("change_style"));
    m_style->setText(i18n("Style"));
    connect(m_style, &KSelectAction::indexTriggered,
            this, &KUIViewerPart::slotStyle);
    //m_style->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    m_style->setEditable(false);

    m_styleFromConfig = KConfigGroup(KSharedConfig::openConfig(), "General").readEntry("currentWidgetStyle", QString());

    const QStringList styles = QStyleFactory::keys();
    m_style->setItems(QStringList(i18nc("Default style", "Default")) + styles);
    m_style->setCurrentItem(0);

    // empty or incorrect value  means the default value of currentWidgetStyle,
    // which leads to the Default option.
    if (!m_styleFromConfig.isEmpty()) {
        QStringList::ConstIterator it = styles.begin();
        QStringList::ConstIterator end = styles.end();

        // Skip the default item
        int idx = 1;
        for (; it != end; ++it, ++idx) {
            if ((*it).toLower() == m_styleFromConfig.toLower()) {
                m_style->setCurrentItem(idx);
                break;
            }
        }
    }

    m_style->setToolTip(i18n("Set the style used for the view."));
    m_style->setMenuAccelsEnabled(true);

    m_copy = KStandardAction::copy(this, &KUIViewerPart::slotGrab, actionCollection());
    m_copy->setText(i18n("Copy as Image"));

    updateActions();

// Commented out to fix warning (rich)
// slot should probably be called saveAs() for consistency with
// KParts::ReadWritePart BTW.
//    KStandardAction::saveAs(this, SLOT(slotSave()), actionCollection());
}

KUIViewerPart::~KUIViewerPart()
{
}

static QStringList designerPluginPaths()
{
    QStringList paths;
    const QStringList& libraryPaths = QApplication::libraryPaths();
    for (const auto& path : libraryPaths) {
        paths.append(path + QLatin1String("/designer"));
    }
    return paths;
}

bool KUIViewerPart::openFile()
{
    // m_file is always local so we can use QFile on it
    QFile file(localFilePath());

    return loadUiFile(&file);
}

bool KUIViewerPart::doOpenStream(const QString& mimeType)
{
    auto mime = QMimeDatabase().mimeTypeForName(mimeType);
    if (!mime.inherits(QStringLiteral("application/x-designer"))) {
        return false;
    }

    m_streamedData.clear();

    return true;
}

bool KUIViewerPart::doWriteStream(const QByteArray& data)
{
    m_streamedData.append(data);
    return true;
}

bool KUIViewerPart::doCloseStream()
{
    QBuffer buffer(&m_streamedData);

    const auto success = loadUiFile(&buffer);
    m_streamedData.clear();

    return success;
}

bool KUIViewerPart::loadUiFile(QIODevice* device)
{
    if (!device->open(QIODevice::ReadOnly|QIODevice::Text)) {
        qCDebug(KUIVIEWERPART) << "Could not open UI file: " << device->errorString();
        if (m_previousUrl != url()) {
            // drop previous view state
            m_previousScrollPosition = QPoint();
            m_previousSize = QSize();
        }
        return false;
    }

    if (m_subWindow) {
        m_widget->removeSubWindow(m_subWindow);
        delete m_view;
        delete m_subWindow;
        m_subWindow = nullptr;
    }

    QFormBuilder builder;
    builder.setPluginPath(designerPluginPaths());
    m_view = builder.load(device, nullptr);

    updateActions();

    if (!m_view) {
        qCDebug(KUIVIEWERPART) << "Could not load UI file: " << builder.errorString();
        if (m_previousUrl != url()) {
            // drop previous view state
            m_previousScrollPosition = QPoint();
            m_previousSize = QSize();
        }
        return false;
    }

    // hack ahead:
    // UI files have a size set for the widget they define. The QMdiSubWindow relies on sizeHint()
    // during the show event though it seems, to calculate the initial window size, and then discards
    // the widget size initially set from the builder in the following layout-ruled geometry update.
    // Enforcing the initial size by manually setting it afterwards to the widget itself seems not possible,
    // due to the layout government based on window size.
    // To inject the initial widget size into the initial window geometry, as hack the min and max sizes are
    // temporarily set to the wanted size and, once the window is shown, reset to their initial values.
    const QSize widgetSize = m_view->size();
    const QSize origWidgetMinimumSize = m_view->minimumSize();
    const QSize origWidgetMaximumSize = m_view->maximumSize();
    restyleView(m_style->currentText());
    m_view->setMinimumSize(widgetSize);
    m_view->setMaximumSize(widgetSize);

    const Qt::WindowFlags windowFlags(Qt::SubWindow|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    m_subWindow = m_widget->addSubWindow(m_view, windowFlags);
    // prevent focus stealing by adding the window in disabled state
    m_subWindow->setEnabled(false);
    m_subWindow->show();
    // and restore minimum size
    m_view->setMinimumSize(origWidgetMinimumSize);
    m_view->setMaximumSize(origWidgetMaximumSize);

    m_widget->setActiveSubWindow(m_subWindow);
    m_subWindow->setEnabled(true);

    // restore view state if reload
    if (url() == m_previousUrl) {
        qCDebug(KUIVIEWERPART) << "Restoring previous view state";
        m_subWindow->move(m_previousScrollPosition);
        if (m_previousSize.isValid()) {
            m_subWindow->resize(m_previousSize);
        }
    }

    return true;
}

bool KUIViewerPart::closeUrl()
{
    // store view state if file could be loaded
    // otherwise keep old in case same url will get reloaded again and then successfully
    if (m_subWindow) {
        m_previousScrollPosition = m_subWindow->pos();
        m_previousSize = m_subWindow->size();
    }
    // store last used url
    const auto activeUrl = url();
    if (activeUrl.isValid()) {
        m_previousUrl = activeUrl;
    }

    m_streamedData.clear();

    return ReadOnlyPart::closeUrl();
}

void KUIViewerPart::updateActions()
{
    const bool hasView = !m_view.isNull();

    m_style->setEnabled(hasView);
    m_copy->setEnabled(hasView);
}

void KUIViewerPart::restyleView(const QString& styleName)
{
    QStyle* style = QStyleFactory::create(styleName);

    m_view->setStyle(style);

    const QList<QWidget*> childWidgets = m_view->findChildren<QWidget*>();
    for (auto child : childWidgets) {
        child->setStyle(style);
    }
}

void KUIViewerPart::setWidgetSize(const QSize& size)
{
    if (m_view.isNull()) {
        return;
    }

    // hack: enforce widget size by setting min/max sizes to wanted size
    // and then have layout update the complete window
    const QSize origWidgetMinimumSize = m_view->minimumSize();
    const QSize origWidgetMaximumSize = m_view->maximumSize();
    m_view->setMinimumSize(size);
    m_view->setMaximumSize(size);
    m_subWindow->updateGeometry();
    // restore
    m_view->setMinimumSize(origWidgetMinimumSize);
    m_view->setMaximumSize(origWidgetMaximumSize);
}

QPixmap KUIViewerPart::renderWidgetAsPixmap() const
{
    if (m_view.isNull()) {
        return QPixmap();
    }

    return m_view->grab();
}

void KUIViewerPart::slotStyle(int)
{
    if (m_view.isNull()) {
        updateActions();
        return;
    }

    m_view->hide();

    const QString styleName = m_style->currentText();
    qCDebug(KUIVIEWERPART) << "Style selected:" << styleName;
    restyleView(styleName);

    m_view->show();

    /* the style changed, update the configuration */
    if (m_styleFromConfig != styleName) {
        KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
        KConfigGroup cg(cfg, "General");
        if (m_style->currentItem() > 0) {
            /* A style different from the default */
            cg.writeEntry("currentWidgetStyle", styleName);
        } else {
            /* default style: remove the entry */
            cg.deleteEntry("currentWidgetStyle");
        }
        cfg->sync();
    }
}

void KUIViewerPart::slotGrab()
{
    if (m_view.isNull()) {
        updateActions();
        return;
    }

    const QPixmap pixmap = m_view->grab();
    QApplication::clipboard()->setPixmap(pixmap);
}

#include "kuiviewer_part.moc"
