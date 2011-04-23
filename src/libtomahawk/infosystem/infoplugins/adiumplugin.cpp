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

#include <Carbon/Carbon.h>
#include <string.h>

#include "artist.h"
#include "result.h"

#include "adiumplugin.h"

/* NOTE: LowRunAppleScript comes from 
http://developer.apple.com/library/mac/#qa/qa2001/qa1026.html */

    /* LowRunAppleScript compiles and runs an AppleScript
    provided as text in the buffer pointed to by text.  textLength
    bytes will be compiled from this buffer and run as an AppleScript
    using all of the default environment and execution settings.  If
    resultData is not NULL, then the result returned by the execution
    command will be returned as typeChar in this descriptor record
    (or typeNull if there is no result information).  If the function
    returns errOSAScriptError, then resultData will be set to a
    descriptive error message describing the error (if one is
    available).  */
static OSStatus LowRunAppleScript(const void* text, long textLength,
                                    AEDesc *resultData) {
    ComponentInstance theComponent;
    AEDesc scriptTextDesc;
    OSStatus err;
    OSAID scriptID, resultID;

        /* set up locals to a known state */
    theComponent = NULL;
    AECreateDesc(typeNull, NULL, 0, &scriptTextDesc);
    scriptID = kOSANullScript;
    resultID = kOSANullScript;

        /* open the scripting component */
    theComponent = OpenDefaultComponent(kOSAComponentType,
                    typeAppleScript);
    if (theComponent == NULL) { err = paramErr; goto bail; }

        /* put the script text into an aedesc */
    err = AECreateDesc(typeChar, text, textLength, &scriptTextDesc);
    if (err != noErr) goto bail;

        /* compile the script */
    err = OSACompile(theComponent, &scriptTextDesc,
                    kOSAModeNull, &scriptID);
    if (err != noErr) goto bail;

        /* run the script */
    err = OSAExecute(theComponent, scriptID, kOSANullScript,
                    kOSAModeNull, &resultID);

        /* collect the results - if any */
    if (resultData != NULL) {
        AECreateDesc(typeNull, NULL, 0, resultData);
        if (err == errOSAScriptError) {
            OSAScriptError(theComponent, kOSAErrorMessage,
                        typeChar, resultData);
        } else if (err == noErr && resultID != kOSANullScript) {
            OSADisplay(theComponent, resultID, typeChar,
                        kOSAModeNull, resultData);
        }
    }
bail:
    AEDisposeDesc(&scriptTextDesc);
    if (scriptID != kOSANullScript) OSADispose(theComponent, scriptID);
    if (resultID != kOSANullScript) OSADispose(theComponent, resultID);
    if (theComponent != NULL) CloseComponent(theComponent);
    return err;
}


    /* SimpleRunAppleScript compiles and runs the AppleScript in
    the c-style string provided as a parameter.  The result returned
    indicates the success of the operation. */
static OSStatus SimpleRunAppleScript(const char* theScript) {
    return LowRunAppleScript(theScript, strlen(theScript), NULL);
}

static void setStatus(const QString &status)
{
  QString adiumStatus = "tell application \"Adium\"\n";
  adiumStatus.append("set the status message of every account to \"Tomahawk: ");
  adiumStatus.append(status);
  adiumStatus.append("\"\nend tell");
  qDebug() << "status: " << status;
  qDebug() << "Adium Status: " << adiumStatus;
  const char* script = adiumStatus.toUtf8();
  SimpleRunAppleScript( script );

}


using namespace Tomahawk::InfoSystem;

AdiumPlugin::AdiumPlugin(QObject *parent)
    : InfoPlugin(parent)
{
    /** No supported types since the plugin pushes info, doesn't get any */
    qDebug() << Q_FUNC_INFO;
    QSet< InfoType > supportedTypes;
    InfoSystem *system = qobject_cast< InfoSystem* >(parent);
    system->registerInfoTypes(this, supportedTypes);

    /** Connect to audio state signals.
	TODO: Move this into InfoSystem? There could end up being many plugins
	connected to audio state signals. */

    connect( system, SIGNAL( audioStarted( const Tomahawk::result_ptr& ) ),
	     SLOT( audioStarted( const Tomahawk::result_ptr& ) ) );
    connect( system, SIGNAL( audioFinished( const Tomahawk::result_ptr& ) ),
	     SLOT( audioFinished( const Tomahawk::result_ptr& ) ) );
    connect( system, SIGNAL( audioStopped() ),
	     SLOT( audioStopped() ) );
    connect( system, SIGNAL( audioPaused() ),
	     SLOT( audioPaused() ) );
    connect( system, SIGNAL( audioResumed() ),
	     SLOT( audioResumed() ) );
    
}

AdiumPlugin::~AdiumPlugin()
{
    qDebug() << Q_FUNC_INFO;
}

void AdiumPlugin::getInfo(const QString &caller, const InfoType type, const QVariant& data, InfoCustomData customData)
{
    switch (type)
    {
        default:
        {
            emit info(caller, Tomahawk::InfoSystem::InfoNoInfo, QVariant(), QVariant(), customData);
            return;
        }
    }
}

/** Audio state slots */

void AdiumPlugin::audioStarted( const Tomahawk::result_ptr& track )
{
    qDebug() << Q_FUNC_INFO;
    // TODO: audio started, so push update status to Adium with playing track
    QString nowPlaying = "";
    nowPlaying.append( track->track() );
    nowPlaying.append(" - ");
    nowPlaying.append(track->artist()->name());
    setStatus( nowPlaying );
}

void AdiumPlugin::audioFinished( const Tomahawk::result_ptr& track )
{
    qDebug() << Q_FUNC_INFO;
}

void AdiumPlugin::audioStopped()
{
    qDebug() << Q_FUNC_INFO;
    // TODO: audio stopped, so push update status to Adium that says "stopped"
}

void AdiumPlugin::audioPaused()
{
    qDebug() << Q_FUNC_INFO;
    // TODO: audio paused, so push update status to Adium that says "paused"
}

void AdiumPlugin::audioResumed()
{
    qDebug() << Q_FUNC_INFO;
    // TODO: audio resumed, so push update status to Adium with playing track
}

