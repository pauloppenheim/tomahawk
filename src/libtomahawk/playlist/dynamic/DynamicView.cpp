/****************************************************************************************
 * Copyright (c) 2011 Leo Franchi <lfranchi@kde.org>                                    *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#include "DynamicView.h"

#include "widgets/overlaywidget.h"
#include "playlistmodel.h"
#include "trackproxymodel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QtGui/qpaintengine.h>
using namespace Tomahawk;

#define FADE_LENGTH 800
#define SLIDE_LENGTH 300
#define SLIDE_OFFSET 500
#define LONG_MULT 0 // to avoid superfast slides when the length is long, make it longer incrementally

DynamicView::DynamicView( QWidget* parent )
        : PlaylistView( parent )
        , m_onDemand( false )
{
    m_fadeOutAnim.setDuration( FADE_LENGTH );
    m_fadeOutAnim.setCurveShape( QTimeLine::LinearCurve );
    m_fadeOutAnim.setFrameRange( 100, 0 );
    m_fadeOutAnim.setUpdateInterval( 5 );
    
    QEasingCurve curve( QEasingCurve::OutBounce );
    curve.setAmplitude( .25 );
    m_slideAnim.setEasingCurve( curve );
    m_slideAnim.setDirection( QTimeLine::Forward );
    m_fadeOutAnim.setUpdateInterval( 5 );
    
    
    connect( &m_fadeOutAnim, SIGNAL( frameChanged( int ) ), viewport(), SLOT( update() ) );
}

DynamicView::~DynamicView()
{

}

void 
DynamicView::setModel( PlaylistModel* model)
{
    PlaylistView::setModel( model );
    
    connect( model, SIGNAL( trackCountChanged( unsigned int ) ), SLOT( onTrackCountChanged( unsigned int ) ) );
}

void
DynamicView::setOnDemand( bool onDemand )
{
    m_onDemand = onDemand;
}

void 
DynamicView::showMessageTimeout( const QString& title, const QString& body )
{
    m_title = title;
    m_body = body;
    
    overlay()->setText( QString( "%1:\n\n%2" ).arg( m_title, m_body ) );
    overlay()->show( 5 );
}

void 
DynamicView::onTrackCountChanged( unsigned int tracks )
{
    if ( tracks == 0 )
    {
        if( m_onDemand )
            overlay()->setText( tr( "Add some filters above, and press Start to begin listening to this custom station!" ) );
        else
            overlay()->setText( tr( "Add some filters above, and press Generate to get started!" ) );
        if( !overlay()->shown() )
            overlay()->show();
    }
    else
        overlay()->hide();
}

void 
DynamicView::collapseEntries( int startRow, int num )
{
    if( m_fadeOutAnim.state() == QTimeLine::Running )
        qDebug() << "COLLAPSING TWICE!";
    // we capture the image of the rows we're going to collapse
    // then we capture the image of the target row we're going to animate downwards
    // then we fade the first image out while sliding the second image up.
    QModelIndex topLeft = proxyModel()->index( startRow, 0, QModelIndex() );
    QModelIndex bottomRight = proxyModel()->index( startRow + num - 1, proxyModel()->columnCount( QModelIndex() ) - 1, QModelIndex() );
    QItemSelection sel( topLeft, bottomRight );
    QRect fadingRect = visualRegionForSelection( sel ).boundingRect();
    
    m_fadingIndexes = QPixmap::grabWidget( viewport(), fadingRect );
    m_fadingPointAnchor = fadingRect.topLeft();
    
    qDebug() << "Grabbed fading indexes from rect:" << fadingRect << m_fadingIndexes.size();
    
    topLeft = proxyModel()->index( startRow + num, 0, QModelIndex() );
    bottomRight = proxyModel()->index( startRow + num, proxyModel()->columnCount( QModelIndex() ) - 1, QModelIndex() );
    QRect slidingRect = visualRegionForSelection( QItemSelection( topLeft, bottomRight ) ).boundingRect();
    
    m_slidingIndex = QPixmap::grabWidget( viewport(), slidingRect );
    m_bottomAnchor = slidingRect.topLeft();
    m_bottomOfAnim = slidingRect.bottomLeft();
    qDebug() << "Grabbed sliding index from rect:" << slidingRect << m_slidingIndex.size();
    
    // slide from the current position to the new one
    int frameRange = fadingRect.topLeft().y() - slidingRect.topLeft().y();
    m_slideAnim.setDuration( SLIDE_LENGTH + frameRange * LONG_MULT );
    m_slideAnim.setFrameRange( slidingRect.topLeft().y(), fadingRect.topLeft().y() );
    
    m_fadeOutAnim.start();
    QTimer::singleShot( SLIDE_OFFSET, &m_slideAnim, SLOT( start() ) );
    
    QModelIndexList todel;
    for( int i = 0; i < num; i++ ) {
        for( int k = 0; k < proxyModel()->columnCount( QModelIndex() ); k++ ) {
            todel << proxyModel()->index( startRow + i, k );
        }
    }
    proxyModel()->removeIndexes( todel );
}

void 
DynamicView::paintEvent( QPaintEvent* event )
{
    TrackView::paintEvent(event);
    
    QPainter p( viewport() );
    if( m_fadeOutAnim.state() == QTimeLine::Running ) { // both run together
        p.save();
        QRect bg = m_fadingIndexes.rect();
        bg.moveTo( m_fadingPointAnchor ); // cover up the background
        p.fillRect( bg, Qt::white );
        
//         qDebug() << "FAST SETOPACITY:" << p.paintEngine()->hasFeature(QPaintEngine::ConstantOpacity);
        p.setOpacity( 1 - m_fadeOutAnim.currentValue() );
        p.drawPixmap( m_fadingPointAnchor, m_fadingIndexes );
        
        p.restore();   
        
        if( m_slideAnim.state() == QTimeLine::Running ) {
            // draw the collapsing entry
            QRect bg = m_slidingIndex.rect();
            bg.moveTo( m_bottomAnchor );
            bg.setBottom( m_bottomOfAnim.y() );
            p.fillRect( bg, Qt::white );
            p.drawPixmap( 0, m_slideAnim.currentFrame(), m_slidingIndex );
        } else if( m_fadeOutAnim.state() == QTimeLine::Running ) {
            p.drawPixmap( m_bottomAnchor, m_slidingIndex );
        }
    }
}
