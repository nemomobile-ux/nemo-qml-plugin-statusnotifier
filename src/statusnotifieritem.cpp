/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org/
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Paulo Lieuthier <paulolieuthier@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "statusnotifieritem.h"
#include "statusnotifieritemadaptor.h"
#include <QDBusInterface>
#include <QDBusServiceWatcher>

static int m_serviceCounter = 0;

StatusNotifierItem::StatusNotifierItem(const QString &category, bool itemIsMenu, const QString &menuPath, QObject *parent)
    : QObject(parent),
    m_adaptor(new StatusNotifierItemAdaptor(this)),
    m_service(QStringLiteral("org.kde.StatusNotifierItem-%1-%2")
             .arg(QCoreApplication::applicationPid())
             .arg(++m_serviceCounter)),
    m_title(QStringLiteral("Test")),
    m_status(QStringLiteral("Active")),
    m_sessionBus(QDBusConnection::connectToBus(QDBusConnection::SessionBus, m_service))
{
    setProperty("Id", objectName());
    setProperty("AttentionMovieName", QString());
    setProperty("WindowId", m_serviceCounter);
    setProperty("Category", category);
    setProperty("ItemIsMenu", itemIsMenu);
    setProperty("Menu", QVariant::fromValue(QDBusObjectPath(menuPath)));

    // Separate DBus connection to the session bus is created, because QDbus does not provide
    // a way to register different objects for different services with the same paths.
    // For status notifiers we need different /StatusNotifierItem for each service.

    // register service

    m_sessionBus.registerService(m_service);
    m_sessionBus.registerObject(QStringLiteral("/StatusNotifierItem"), this);

    registerToHost();

    // monitor the watcher service in case the host restarts
    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(QStringLiteral("org.kde.StatusNotifierWatcher"),
                                                           m_sessionBus,
                                                           QDBusServiceWatcher::WatchForOwnerChange,
                                                           this);
    connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged,
            this, &StatusNotifierItem::onServiceOwnerChanged);
}

StatusNotifierItem::~StatusNotifierItem()
{
    m_sessionBus.unregisterObject(QStringLiteral("/StatusNotifierItem"));
    m_sessionBus.unregisterService(m_service);
    QDBusConnection::disconnectFromBus(m_service);
}

QString StatusNotifierItem::title() const
{
    return m_title;
}

void StatusNotifierItem::registerToHost()
{
    QDBusInterface interface(QStringLiteral("org.kde.StatusNotifierWatcher"),
                             QStringLiteral("/StatusNotifierWatcher"),
                             QStringLiteral("org.kde.StatusNotifierWatcher"),
                             m_sessionBus);
    interface.asyncCall(QStringLiteral("RegisterStatusNotifierItem"), m_service);
}

void StatusNotifierItem::onServiceOwnerChanged(const QString&, const QString&,
                                               const QString& newOwner)
{
    if (!newOwner.isEmpty())
        registerToHost();
}

void StatusNotifierItem::setTitle(const QString &title)
{
    if (m_title == title)
        return;

    m_title = title;
    emit m_adaptor->NewTitle();
}

QString StatusNotifierItem::status() const
{
    return m_status;
}

void StatusNotifierItem::setStatus(const QString &status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit m_adaptor->NewStatus(m_status);
}

QString StatusNotifierItem::iconName() const
{
    return m_iconName;
}

void StatusNotifierItem::setIconByName(const QString &name)
{
    if (m_iconName == name)
        return;

    m_iconName = name;
    emit m_adaptor->NewIcon();
}

IconPixmapList StatusNotifierItem::iconPixmap() const
{
    return m_icon;
}

void StatusNotifierItem::setIconByPixmap(const QPixmap &icon)
{
    m_iconName.clear();
    m_icon.clear();
    IconPixmap pix;
    pix.width = icon.width();
    pix.height = icon.height();
    QBuffer buf(&pix.bytes);
    icon.save(&buf);
    m_icon.append(pix);
    emit m_adaptor->NewIcon();
}

void StatusNotifierItem::setIconByImage(const QImage &icon)
{
    m_iconName.clear();
    m_icon.clear();
    IconPixmap pix;
    pix.width = icon.width();
    pix.height = icon.height();
    QBuffer buf(&pix.bytes);
    icon.save(&buf);
    m_icon.append(pix);
    emit m_adaptor->NewIcon();
}

QString StatusNotifierItem::overlayIconName() const
{
    return m_overlayIconName;
}

void StatusNotifierItem::setOverlayIconByName(const QString &name)
{
    if (m_overlayIconName == name)
        return;

    m_overlayIconName = name;
    emit m_adaptor->NewOverlayIcon();
}

IconPixmapList StatusNotifierItem::overlayIconPixmap() const
{
    return m_overlayIcon;
}

void StatusNotifierItem::setOverlayIconByPixmap(const QPixmap &icon)
{
    m_overlayIconName.clear();
    m_overlayIcon.clear();
    IconPixmap pix;
    pix.width = icon.width();
    pix.height = icon.height();
    QBuffer buf(&pix.bytes);
    icon.save(&buf);
    m_overlayIcon.append(pix);
    emit m_adaptor->NewOverlayIcon();
}

void StatusNotifierItem::setOverlayIconByImage(const QImage &icon)
{
    m_overlayIconName.clear();
    m_overlayIcon.clear();
    IconPixmap pix;
    pix.width = icon.width();
    pix.height = icon.height();
    QBuffer buf(&pix.bytes);
    icon.save(&buf);
    m_overlayIcon.append(pix);
    emit m_adaptor->NewOverlayIcon();
}

QString StatusNotifierItem::attentionIconName() const
{
    return m_attentionIconName;
}

void StatusNotifierItem::setAttentionIconByName(const QString &name)
{
    if (m_attentionIconName == name)
        return;

    m_attentionIconName = name;
    emit m_adaptor->NewAttentionIcon();
}

IconPixmapList StatusNotifierItem::attentionIconPixmap() const
{
    return m_attentionIcon;
}

void StatusNotifierItem::setAttentionIconByPixmap(const QPixmap &icon)
{
    m_attentionIconName.clear();
    m_attentionIcon.clear();
    IconPixmap pix;
    pix.width = icon.width();
    pix.height = icon.height();
    QBuffer buf(&pix.bytes);
    icon.save(&buf);
    m_attentionIcon.append(pix);
    emit m_adaptor->NewAttentionIcon();
}

void StatusNotifierItem::setAttentionIconByImage(const QImage &icon)
{
    m_attentionIconName.clear();
    m_attentionIcon.clear();
    IconPixmap pix;
    pix.width = icon.width();
    pix.height = icon.height();
    QBuffer buf(&pix.bytes);
    icon.save(&buf);
    m_attentionIcon.append(pix);
    emit m_adaptor->NewAttentionIcon();
}

ToolTip StatusNotifierItem::toolTip() const
{
    ToolTip tt;
    return tt;
}

void StatusNotifierItem::Activate(int x, int y)
{
    if (m_status == QStringLiteral("NeedsAttention"))
        m_status = QStringLiteral("Active");

    emit activateRequested(QPoint(x, y));
}

void StatusNotifierItem::SecondaryActivate(int x, int y)
{
    if (m_status == QStringLiteral("NeedsAttention"))
        m_status = QStringLiteral("Active");

    emit secondaryActivateRequested(QPoint(x, y));
}

void StatusNotifierItem::ContextMenu(int, int)
{
}

void StatusNotifierItem::Scroll(int delta, const QString &orientation)
{
    Qt::Orientation orient = Qt::Vertical;
    if (orientation.toLower() == QStringLiteral("horizontal"))
        orient = Qt::Horizontal;

    emit scrollRequested(delta, orient);
}
