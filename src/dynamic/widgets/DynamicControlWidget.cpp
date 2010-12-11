/****************************************************************************************
 * Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>                                    *
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

#include "DynamicControlWidget.h"

#include "tomahawk/tomahawkapp.h"
#include "dynamic/DynamicControl.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QLayout>
#include <QToolButton>

using namespace Tomahawk;

DynamicControlWidget::DynamicControlWidget( const Tomahawk::dyncontrol_ptr& control, bool showPlus, QWidget* parent )
     : QWidget(parent)
     , m_showPlus( showPlus )
     , m_plusButton( 0 )
     , m_control( control )
     , m_typeSelector( 0 )
     , m_layout( 0 )
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    
    m_layout = new QHBoxLayout;
    m_typeSelector = new QComboBox( this );
    
    m_plusButton= new QToolButton( this );
    m_plusButton->setIcon( QIcon( RESPATH "images/list-add.png" ) );
    
    connect( m_typeSelector, SIGNAL( currentIndexChanged( QString ) ), SLOT( typeSelectorChanged( QString ) ) );
    
    if( !control.isNull() ) {
        foreach( const QString& type, control->typeSelectors() )
            m_typeSelector->addItem( type );
        
        m_layout->addWidget( m_typeSelector, 0, Qt::AlignLeft );
        m_layout->addWidget( m_control->matchSelector(), 0, Qt::AlignCenter );
        m_layout->addWidget( m_control->inputField(), 0, Qt::AlignRight );
    }
    
    if( m_showPlus )
        m_layout->addWidget( m_plusButton, 0, Qt::AlignRight );
    
    setLayout( m_layout );
}

DynamicControlWidget::~DynamicControlWidget()
{

}

void 
DynamicControlWidget::typeSelectorChanged( QString type )
{
    Q_ASSERT( m_layout );
    // remove the two widgets, change the control,and re-add the new ones
    if( m_layout->count() == 3 )
    {
        m_layout->takeAt( 1 );
        m_layout->takeAt( 2 );
    } else
        Q_ASSERT( m_layout->count() == 1 );
    
    m_control->setSelectedType( type );
    m_layout->addWidget( m_control->matchSelector(), 0, Qt::AlignCenter );
    m_layout->addWidget( m_control->inputField(), 0, Qt::AlignRight );
}

void 
DynamicControlWidget::setShowPlusButton(bool show)
{
    
    if( m_showPlus != show ) {
        if( show ) {
            m_layout->addWidget( m_plusButton, 0, Qt::AlignRight );
        } else {
            m_layout->removeWidget( m_plusButton );
        }
    }
    
    m_showPlus = show;
}

bool 
DynamicControlWidget::showPlusButton() const
{
    return m_showPlus;
}