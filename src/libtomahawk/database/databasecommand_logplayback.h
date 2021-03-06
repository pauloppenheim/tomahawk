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

#ifndef DATABASECOMMAND_LOGPLAYBACK_H
#define DATABASECOMMAND_LOGPLAYBACK_H

#include <QObject>
#include <QVariantMap>

#include "database/databasecommandloggable.h"
#include "sourcelist.h"
#include "typedefs.h"
#include "query.h"
#include "result.h"
#include "artist.h"

#include "dllmacro.h"

class DLLEXPORT DatabaseCommand_LogPlayback : public DatabaseCommandLoggable
{
Q_OBJECT
Q_PROPERTY( QString artist READ artist WRITE setArtist )
Q_PROPERTY( QString track READ track WRITE setTrack )
Q_PROPERTY( unsigned int playtime READ playtime WRITE setPlaytime )
Q_PROPERTY( unsigned int secsPlayed READ secsPlayed WRITE setSecsPlayed )
Q_PROPERTY( int action READ action WRITE setAction )

public:
    enum Action
    {
        Started = 1,
        Finished = 2
    };

    explicit DatabaseCommand_LogPlayback( QObject* parent = 0 )
        : DatabaseCommandLoggable( parent )
    {}

    explicit DatabaseCommand_LogPlayback( const Tomahawk::result_ptr& result, Action action, unsigned int secsPlayed = 0, QObject* parent = 0 )
        : DatabaseCommandLoggable( parent ), m_result( result ), m_secsPlayed( secsPlayed ), m_action( action )
    {
        m_playtime = QDateTime::currentDateTimeUtc().toTime_t();
        setSource( SourceList::instance()->getLocal() );

        setArtist( result->artist()->name() );
        setTrack( result->track() );
    }

    virtual QString commandname() const { return "logplayback"; }

    virtual void exec( DatabaseImpl* );
    virtual void postCommitHook();

    virtual bool doesMutates() const { return true; }
    virtual bool singletonCmd() const { return ( m_action == Started ); }

    QString artist() const { return m_artist; }
    void setArtist( const QString& s ) { m_artist = s; }

    QString track() const { return m_track; }
    void setTrack( const QString& s ) { m_track = s; }

    unsigned int playtime() const { return m_playtime; }
    void setPlaytime( unsigned int i ) { m_playtime = i; }

    unsigned int secsPlayed() const { return m_secsPlayed; }
    void setSecsPlayed( unsigned int i ) { m_secsPlayed = i; }

    int action() const { return m_action; }
    void setAction( int a ) { m_action = (Action)a; }

signals:
    void trackPlaying( const Tomahawk::query_ptr& query );
    void trackPlayed( const Tomahawk::query_ptr& query );

private:
    Tomahawk::result_ptr m_result;

    QString m_artist;
    QString m_track;
    unsigned int m_playtime;
    unsigned int m_secsPlayed;
    Action m_action;
};

#endif // DATABASECOMMAND_LOGPLAYBACK_H
