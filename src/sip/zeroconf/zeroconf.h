/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ZEROCONF_H
#define ZEROCONF_H

#include "sip/SipPlugin.h"
#include "tomahawkzeroconf.h"

#include "../sipdllmacro.h"

#define MYNAME "Local Network"

class Ui_ZeroconfConfig;

class SIPDLLEXPORT ZeroconfFactory : public SipPluginFactory
{
    Q_OBJECT
    Q_INTERFACES( SipPluginFactory )
public:
    ZeroconfFactory() {}
    virtual ~ZeroconfFactory() {}

    virtual QString factoryId() const { return "sipzeroconf"; }
    virtual QString prettyName() const { return "Local Network"; }
    virtual bool isUnique() const { return true; }
    virtual QIcon icon() const;


    virtual SipPlugin* createPlugin ( const QString& pluginId = QString() );
};

class SIPDLLEXPORT ZeroconfPlugin : public SipPlugin
{
    Q_OBJECT

public:
    ZeroconfPlugin( const QString& pluginId );

    virtual ~ZeroconfPlugin()
    {
        qDebug() << Q_FUNC_INFO;
    }

    virtual const QString name() const;
    virtual const QString friendlyName() const;
    virtual const QString accountName() const;
    virtual ConnectionState connectionState() const;
    virtual bool isValid() const { return true; };
    virtual QIcon icon() const;
    virtual QWidget* configWidget() { return m_configWidget.data(); }
    virtual void saveConfig();

public slots:
    virtual bool connectPlugin( bool startup );
    void disconnectPlugin();
    void checkSettings() {}

    void sendMsg( const QString& to, const QString& msg )
    {
        Q_UNUSED( to );
        Q_UNUSED( msg );
    }

    void broadcastMsg( const QString &msg )
    {
        Q_UNUSED( msg );
    }

    void addContact( const QString &jid, const QString& msg = QString() )
    {
        Q_UNUSED( jid );
        Q_UNUSED( msg );
    }

private slots:
    void lanHostFound( const QString& host, int port, const QString& name, const QString& nodeid );

private:
    bool autoConnect() const;

    TomahawkZeroconf* m_zeroconf;
    ConnectionState m_state;
    QVector<QStringList> m_cachedNodes;

    Ui_ZeroconfConfig* m_ui; // so the google wrapper can change the config dialog a bit
    QWeakPointer< QWidget > m_configWidget;
};

#endif
