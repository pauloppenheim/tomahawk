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

#include "elidedlabel.h"

#include <QEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QApplication>
#include <QRect>


ElidedLabel::ElidedLabel( QWidget* parent, Qt::WindowFlags flags )
    : QFrame( parent, flags )
{
    init();
}


ElidedLabel::ElidedLabel( const QString& text, QWidget* parent, Qt::WindowFlags flags )
    : QFrame( parent, flags )
{
    init(text);
}


ElidedLabel::~ElidedLabel()
{
}


QString
ElidedLabel::text() const
{
    return m_text;
}


void
ElidedLabel::setText( const QString& text )
{
    if ( m_text != text )
    {
        m_text = text;
        updateLabel();
        emit textChanged( text );
    }
}


Qt::Alignment
ElidedLabel::alignment() const
{
    return m_align;
}


void
ElidedLabel::setAlignment( Qt::Alignment alignment )
{
    if ( m_align != alignment )
    {
        m_align = alignment;
        update(); // no geometry change, repaint is sufficient
    }
}


Qt::TextElideMode
ElidedLabel::elideMode() const
{
    return m_mode;
}


void
ElidedLabel::setElideMode( Qt::TextElideMode mode )
{
    if ( m_mode != mode )
    {
        m_mode = mode;
        updateLabel();
    }
}

void 
ElidedLabel::setMargin( int margin )
{
    m_margin = margin;
}

int 
ElidedLabel::margin() const
{
    return m_margin;
}


void
ElidedLabel::init( const QString& txt )
{
    m_text = txt;
    m_align = Qt::AlignLeft;
    m_mode = Qt::ElideMiddle;
    m_margin = 0;
    setContentsMargins( 0, 0, 0, 0 );
}


void
ElidedLabel::updateLabel()
{
    updateGeometry();
    update();
}


QSize
ElidedLabel::sizeHint() const
{
    const QFontMetrics& fm = fontMetrics();
    QSize size( fm.width( m_text ), fm.height() );
    return size;
}


QSize
ElidedLabel::minimumSizeHint() const
{
    switch ( m_mode )
    {
        case Qt::ElideNone:
            return sizeHint();

        default:
        {
            const QFontMetrics& fm = fontMetrics();
            QSize size( fm.width( "..." ), fm.height() );
            return size;
        }
    }
}


void
ElidedLabel::paintEvent( QPaintEvent* event )
{
    QFrame::paintEvent( event );
    QPainter p( this );
    QRect r = contentsRect();
    r.adjust( m_margin, m_margin, -m_margin, -m_margin );
    
    const QString elidedText = fontMetrics().elidedText( m_text, m_mode, r.width() );
    p.drawText( r, m_align, elidedText );
}


void
ElidedLabel::changeEvent( QEvent* event )
{
    QFrame::changeEvent( event );
    switch ( event->type() )
    {
        case QEvent::FontChange:
        case QEvent::ApplicationFontChange:
            updateLabel();
            break;

        default:
            // nothing to do
            break;
    }
}


void
ElidedLabel::mousePressEvent( QMouseEvent* event )
{
    QFrame::mousePressEvent( event );
    m_time.start();
}


void
ElidedLabel::mouseReleaseEvent( QMouseEvent* event )
{
    QFrame::mouseReleaseEvent( event );
    if ( m_time.elapsed() < qApp->doubleClickInterval() )
        emit clicked();
}
