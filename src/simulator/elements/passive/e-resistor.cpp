/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 *   ( see copyright.txt file at root folder )                             *
 ***************************************************************************/

#include "e-resistor.h"
#include "simulator.h"
#include "e-pin.h"

eResistor::eResistor( QString id )
         : eElement( id )
{
    m_nominalResistance = 100.0;
    m_admit  = 1.0/100.0;
    m_current = 0;
    m_powerRating = 0.25; // 1/4 Watt standard
    m_isBurnt = false;
    m_ePin.resize(2);
}
eResistor::~eResistor(){}

void eResistor::initialize()
{
    setIsBurnt( false );
}

void eResistor::stamp()
{
    m_ePin[0]->setEnodeComp( m_ePin[1]->getEnode() );
    m_ePin[1]->setEnodeComp( m_ePin[0]->getEnode() );
    stampAdmit();
}

void eResistor::stampAdmit()
{
    if( !m_ePin[0] || !m_ePin[1] ) return;
    if( !m_ePin[0]->isConnected() || !m_ePin[1]->isConnected() ) return;

    m_ePin[0]->stampAdmitance( m_admit );
    m_ePin[1]->stampAdmitance( m_admit );
}

void eResistor::setResistance( double resist )
{
    if( resist < 1e-12 ) resist = 1e-12;
    m_nominalResistance = resist;
    if( !m_isBurnt )
    {
        setAdmit( 1/resist );
    }
}

void eResistor::setIsBurnt( bool b )
{
    m_isBurnt = b;
    if( m_isBurnt ) setAdmit( 0 ); // open circuit
    else setAdmit( 1.0 / m_nominalResistance );
}

void eResistor::burnOut()
{
    if( m_isBurnt ) return;
    m_isBurnt = true;
    setAdmit( 0 ); // open circuit
    onBurned();
}

void eResistor::setAdmit( double admit )   // Admit can be 0
{
    m_admit = admit;
    stampAdmit();
}

void eResistor::setResSafe( double resist )
{
    Simulator::self()->pauseSim();
    eResistor::setResistance( resist );
    Simulator::self()->resumeSim();
}

double eResistor::current()
{
    updateVI();
    return m_current;
}

void eResistor::updateVI()
{
    if( m_ePin[0]->isConnected() && m_ePin[1]->isConnected() )
    {
        double volt = m_ePin[0]->getVoltage()-m_ePin[1]->getVoltage();
        m_current = volt*m_admit;
    }
    else m_current = 0;
}
