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

#include "echonestplugin.h"
#include <echonest/Artist.h>
#include <echonest/ArtistTypes.h>

using namespace Tomahawk::InfoSystem;
using namespace Echonest;

// for internal neatness

EchoNestPlugin::EchoNestPlugin()
    : InfoPlugin()
{
    qDebug() << Q_FUNC_INFO;
    m_supportedGetTypes << Tomahawk::InfoSystem::InfoArtistBiography << Tomahawk::InfoSystem::InfoArtistFamiliarity << Tomahawk::InfoSystem::InfoArtistHotttness << Tomahawk::InfoSystem::InfoArtistTerms << Tomahawk::InfoSystem::InfoMiscTopTerms;
}

EchoNestPlugin::~EchoNestPlugin()
{
    qDebug() << Q_FUNC_INFO;
}

void
EchoNestPlugin::namChangedSlot( QNetworkAccessManager *nam )
{
    qDebug() << Q_FUNC_INFO;
    if( !nam )
        return;
    
    m_nam = QWeakPointer< QNetworkAccessManager >( nam );
    Echonest::Config::instance()->setNetworkAccessManager( nam );
}

void
EchoNestPlugin::getInfo(const QString caller, const Tomahawk::InfoSystem::InfoType type, const QVariant input, const Tomahawk::InfoSystem::InfoCustomData customData)
{
    switch (type)
    {
        case Tomahawk::InfoSystem::InfoArtistBiography:
            return getArtistBiography(caller, input, customData);
        case Tomahawk::InfoSystem::InfoArtistFamiliarity:
            return getArtistFamiliarity(caller, input, customData);
        case Tomahawk::InfoSystem::InfoArtistHotttness:
            return getArtistHotttnesss(caller, input, customData);
        case Tomahawk::InfoSystem::InfoArtistTerms:
            return getArtistTerms(caller, input, customData);
        case Tomahawk::InfoSystem::InfoTrackEnergy:
            return getSongProfile(caller, input, customData, "energy");
        case Tomahawk::InfoSystem::InfoMiscTopTerms:
            return getMiscTopTerms(caller, input, customData);
        default:
        {
            emit info(caller, Tomahawk::InfoSystem::InfoNoInfo, QVariant(), QVariant(), customData);
            return;
        }
    }
}

void
EchoNestPlugin::getSongProfile(const QString &caller, const QVariant &input, const InfoCustomData &customData, const QString &item)
{
    //WARNING: Totally not implemented yet
    Q_UNUSED( item );
    
    if( !isValidTrackData( caller, input, customData ) )
        return;

//     Track track( input.toString() );
//     Artist artist( customData.input()->property("artistName").toString() );
//     reply->setProperty("artist", QVariant::fromValue<Artist>(artist));
//     reply->setProperty( "input", input );
//     m_replyMap[reply] = customData;
//     connect(reply, SIGNAL(finished()), SLOT(getArtistBiographySlot()));
}

void
EchoNestPlugin::getArtistBiography(const QString &caller, const QVariant &input, const InfoCustomData &customData)
{
    if( !isValidArtistData( caller, input, customData ) )
        return;

    Echonest::Artist artist( input.toString() );
    QNetworkReply *reply = artist.fetchBiographies();
    reply->setProperty("artist", QVariant::fromValue<Echonest::Artist>(artist));
    reply->setProperty( "input", input );
    m_replyMap[reply] = customData;
    m_callerMap[reply] = caller;
    connect(reply, SIGNAL(finished()), SLOT(getArtistBiographySlot()));
}

void
EchoNestPlugin::getArtistFamiliarity(const QString &caller, const QVariant &input, const InfoCustomData &customData)
{
    if( !isValidArtistData( caller, input, customData ) )
        return;

    qDebug() << "Fetching artist familiarity!" << input;
    Echonest::Artist artist( input.toString() );
    QNetworkReply* reply = artist.fetchFamiliarity();
    reply->setProperty( "artist", QVariant::fromValue<Echonest::Artist>(artist));
    reply->setProperty( "input", input );
    m_replyMap[reply] = customData;
    m_callerMap[reply] = caller;
    connect(reply, SIGNAL(finished()), SLOT(getArtistFamiliaritySlot()));
}

void
EchoNestPlugin::getArtistHotttnesss(const QString &caller, const QVariant &input, const InfoCustomData &customData)
{
    if( !isValidArtistData( caller, input, customData ) )
        return;

    Echonest::Artist artist( input.toString() );
    QNetworkReply* reply = artist.fetchHotttnesss();
    reply->setProperty( "artist", QVariant::fromValue<Echonest::Artist>(artist));
    reply->setProperty( "input", input );
    m_replyMap[reply] = customData;
    m_callerMap[reply] = caller;
    connect(reply, SIGNAL(finished()), SLOT(getArtistHotttnesssSlot()));
}

void
EchoNestPlugin::getArtistTerms(const QString &caller, const QVariant &input, const InfoCustomData &customData)
{
    if( !isValidArtistData( caller, input, customData ) )
        return;

    Echonest::Artist artist( input.toString() );
    QNetworkReply* reply = artist.fetchTerms( Echonest::Artist::Weight );
    reply->setProperty( "artist", QVariant::fromValue<Echonest::Artist>(artist));
    reply->setProperty( "input", input );
    m_replyMap[reply] = customData;
    m_callerMap[reply] = caller;
    connect(reply, SIGNAL(finished()), SLOT(getArtistTermsSlot()));
}

void
EchoNestPlugin::getMiscTopTerms(const QString &caller, const QVariant &input, const InfoCustomData& customData)
{
    Q_UNUSED( input );
    QNetworkReply* reply = Echonest::Artist::topTerms( 20 );
    m_replyMap[reply] = customData;
    m_callerMap[reply] = caller;
    connect( reply,SIGNAL(finished()), SLOT( getMiscTopSlot()));
}


void
EchoNestPlugin::getArtistBiographySlot()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender() );
    Echonest::Artist artist = artistFromReply( reply );
    BiographyList biographies = artist.biographies();
    InfoGenericMap biographyMap;
    Q_FOREACH(const Biography& biography, biographies)
    {
        biographyMap[biography.site()]["site"] = biography.site();
        biographyMap[biography.site()]["url"] = biography.url().toString();
        biographyMap[biography.site()]["text"] = biography.text();
        biographyMap[biography.site()]["attribution"] = biography.license().attribution;
        biographyMap[biography.site()]["licensetype"] = biography.license().type;
        biographyMap[biography.site()]["attribution"] = biography.license().url.toString();

    }
    emit info( m_callerMap[reply], Tomahawk::InfoSystem::InfoArtistBiography, reply->property( "input" ), QVariant::fromValue<Tomahawk::InfoSystem::InfoGenericMap>(biographyMap), m_replyMap[reply] );
    m_replyMap.remove(reply);
    m_callerMap.remove(reply);
    reply->deleteLater();
}

void
EchoNestPlugin::getArtistFamiliaritySlot()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender() );
    Echonest::Artist artist = artistFromReply( reply );
    qreal familiarity = artist.familiarity();
    emit info( m_callerMap[reply], Tomahawk::InfoSystem::InfoArtistFamiliarity, reply->property( "input" ), familiarity, m_replyMap[reply] );
    m_replyMap.remove(reply);
    m_callerMap.remove(reply);
    reply->deleteLater();
}

void
EchoNestPlugin::getArtistHotttnesssSlot()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender() );
    Echonest::Artist artist = artistFromReply( reply );
    qreal hotttnesss = artist.hotttnesss();
    emit info( m_callerMap[reply], Tomahawk::InfoSystem::InfoArtistHotttness, reply->property( "input" ), hotttnesss, m_replyMap[reply] );
    m_replyMap.remove(reply);
    m_callerMap.remove(reply);
    reply->deleteLater();
}

void
EchoNestPlugin::getArtistTermsSlot()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender() );
    Echonest::Artist artist = artistFromReply( reply );
    TermList terms = artist.terms();
    InfoGenericMap termsMap;
    Q_FOREACH( const Echonest::Term& term, terms ) {
        QMap< QString, QString > termMap;
        termMap[ "weight" ] = QString::number(term.weight());
        termMap[ "frequency" ] = QString::number(term.frequency());
        termsMap[ term.name() ] = termMap;
    }
    emit info( m_callerMap[reply], Tomahawk::InfoSystem::InfoArtistTerms, reply->property( "input" ), QVariant::fromValue<Tomahawk::InfoSystem::InfoGenericMap>(termsMap), m_replyMap[reply] );
    m_replyMap.remove(reply);
    m_callerMap.remove(reply);
    reply->deleteLater();
}

void
EchoNestPlugin::getMiscTopSlot()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender() );
    TermList terms = Echonest::Artist::parseTopTerms( reply );
    InfoGenericMap termsMap;
    Q_FOREACH( const Echonest::Term& term, terms ) {
        QMap< QString, QString > termMap;
        termMap[ "weight" ] = QString::number( term.weight() );
        termMap[ "frequency" ] = QString::number( term.frequency() );
        termsMap[ term.name().toLower() ] = termMap;
    }
    emit info( m_callerMap[reply], Tomahawk::InfoSystem::InfoMiscTopTerms, QVariant(), QVariant::fromValue<Tomahawk::InfoSystem::InfoGenericMap>(termsMap), m_replyMap[reply] );
    m_replyMap.remove(reply);
    m_callerMap.remove(reply);
    reply->deleteLater();
}

bool
EchoNestPlugin::isValidArtistData(const QString &caller, const QVariant &input, const InfoCustomData &customData)
{
    if (input.isNull() || !input.isValid() || !input.canConvert<QString>())
    {
        emit info(caller, Tomahawk::InfoSystem::InfoNoInfo, QVariant(), QVariant(), customData);
        return false;
    }
    QString artistName = input.toString();
    if (artistName.isEmpty() )
    {
        emit info(caller, Tomahawk::InfoSystem::InfoNoInfo, QVariant(), QVariant(), customData);
        return false;
    }
    return true;
}

bool
EchoNestPlugin::isValidTrackData(const QString &caller, const QVariant &input, const InfoCustomData &customData)
{
    if (input.isNull() || !input.isValid() || !input.canConvert<QString>())
    {
        emit info(caller, Tomahawk::InfoSystem::InfoNoInfo, QVariant(), QVariant(), customData);
        return false;
    }
    QString trackName = input.toString();
    if (trackName.isEmpty() )
    {
        emit info(caller, Tomahawk::InfoSystem::InfoNoInfo, QVariant(), QVariant(), customData);
        return false;
    }
    if (!customData.contains("artistName") ||
         customData["artistName"].toString().isEmpty())
        return false;
    return true;
}

Artist
EchoNestPlugin::artistFromReply(QNetworkReply* reply)
{
    Echonest::Artist artist = reply->property("artist").value<Echonest::Artist>();
    try {
        artist.parseProfile(reply);
    } catch( const Echonest::ParseError& e ) {
        qWarning() << "Caught parser error from echonest!" << e.what();
    }
    return artist;
}
