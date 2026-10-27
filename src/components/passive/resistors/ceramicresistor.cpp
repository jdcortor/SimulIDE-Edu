/***************************************************************************
 *   Copyright (C) 2026 by SimulIDE-Edu                                    *
 *                                                                         *
 *   ( see copyright.txt file at root folder )                             *
 ***************************************************************************/

#include <cmath>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QFont>
#include <QFontMetrics>

#include "ceramicresistor.h"
#include "itemlibrary.h"
#include "pin.h"

#include "doubleprop.h"
#include "stringprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("CeramicResistor",str)

Component* CeramicResistor::construct( QString type, QString id )
{ return new CeramicResistor( type, id ); }

LibraryItem* CeramicResistor::libraryItem()
{
    return new LibraryItem(
        tr("Ceramic Resistor"),
        "Resistors",
        "resistor.png",
        "CeramicResistor",
        CeramicResistor::construct);
}

CeramicResistor::CeramicResistor( QString type, QString id )
        : Comp2Pin( type, id )
        , eResistor( id )
{
    m_ePin[0] = m_pin[0];
    m_ePin[1] = m_pin[1];

    m_powerRating = 5.0; // 5 Watts standard ceramic block
    m_tolerance   = 5.0; // ±5 % (J) standard
    m_realistic   = true;
    m_bodyLength  = 34.0;
    m_bodyHeight  = 16.0;
    m_pinDistance = 24.0; // 48 px pin-to-pin = 6 grid cells

    addPropGroup( { tr("Main"), {
        new DoubProp<CeramicResistor>("Resistance", tr("Resistance"), "Ω"
                                     , this, &CeramicResistor::resistance, &CeramicResistor::setResistance ),
        new StrProp<CeramicResistor>( "PowerRating", tr("Power Rating"), "5 W,7 W,10 W,15 W,20 W,25 W"
                                     , this, &CeramicResistor::powerRatingStr, &CeramicResistor::setPowerRatingStr, 0, "enum" ),
        new StrProp<CeramicResistor>( "Tolerance", tr("Tolerance"), "±1 % (F),±2 % (G),±5 % (J),±10 % (K),±20 % (M)"
                                     , this, &CeramicResistor::toleranceStr, &CeramicResistor::setToleranceStr, 0, "enum" ),
        new BoolProp<CeramicResistor>("Realistic", tr("Realistic View"), ""
                                     , this, &CeramicResistor::realistic, &CeramicResistor::setRealistic )
    }, 0 } );

    setShowProp("Resistance");
    setPropStr("Resistance", "10"); // 10 Ω standard power resistor default

    updateDimensions();
}

CeramicResistor::~CeramicResistor(){}

void CeramicResistor::setResistance( double resist )
{
    eResistor::setResistance( resist );
    update();
}

void CeramicResistor::setPowerRating( double p )
{
    if( p <= 0.0 ) p = 5.0;
    eResistor::setPowerRating( p );
    updateDimensions();
    update();
}

QString CeramicResistor::powerRatingStr()
{
    int w = static_cast<int>( std::round( m_powerRating ) );
    return QString::number( w ) + " W";
}

void CeramicResistor::setPowerRatingStr( QString str )
{
    str.remove( "W" ).remove( " " );
    bool ok = false;
    double val = str.toDouble( &ok );
    if( ok && val > 0.0 )
    {
        setPowerRating( val );
    }
}

void CeramicResistor::setTolerance( double t )
{
    if( t <= 0.0 ) t = 5.0;
    m_tolerance = t;
    update();
}

QString CeramicResistor::toleranceStr()
{
    QString letter = toleranceLetter( m_tolerance );
    return QString( "±%1 % (%2)" ).arg( m_tolerance ).arg( letter );
}

void CeramicResistor::setToleranceStr( QString str )
{
    if     ( str.contains( "1 %" ) || str.contains( "(F)" ) )  m_tolerance = 1.0;
    else if( str.contains( "2 %" ) || str.contains( "(G)" ) )  m_tolerance = 2.0;
    else if( str.contains( "5 %" ) || str.contains( "(J)" ) )  m_tolerance = 5.0;
    else if( str.contains( "10 %" ) || str.contains( "(K)" ) ) m_tolerance = 10.0;
    else if( str.contains( "20 %" ) || str.contains( "(M)" ) ) m_tolerance = 20.0;
    update();
}

void CeramicResistor::setRealistic( bool r )
{
    m_realistic = r;
    updateDimensions();
    update();
}

QString CeramicResistor::toleranceLetter( double tol )
{
    if( std::abs( tol - 1.0 ) < 0.25 )  return "F";
    if( std::abs( tol - 2.0 ) < 0.25 )  return "G";
    if( std::abs( tol - 5.0 ) < 1.0 )   return "J";
    if( std::abs( tol - 10.0 ) < 2.0 )  return "K";
    if( std::abs( tol - 20.0 ) < 3.0 )  return "M";
    return "J";
}

QString CeramicResistor::formatValueCode( double R )
{
    if( R <= 0.0 ) return "0R";

    if( R < 1.0 )
    {
        int cents = static_cast<int>( std::round( R * 100.0 ) );
        return QString( "0R%1" ).arg( cents, 2, 10, QChar('0') );
    }
    else if( R < 10.0 )
    {
        int whole = static_cast<int>( std::floor( R ) );
        int frac  = static_cast<int>( std::round( (R - whole) * 10.0 ) );
        if( frac == 0 ) return QString( "%1R" ).arg( whole );
        return QString( "%1R%2" ).arg( whole ).arg( frac );
    }
    else if( R < 1000.0 )
    {
        double rounded = std::round( R * 10.0 ) / 10.0;
        if( std::abs( rounded - std::round( rounded ) ) < 0.01 )
            return QString::number( static_cast<int>( rounded ) ) + "Ω";
        return QString::number( rounded ) + "Ω";
    }
    else if( R < 1e6 )
    {
        double k = R / 1000.0;
        int whole = static_cast<int>( std::floor( k ) );
        int frac  = static_cast<int>( std::round( (k - whole) * 10.0 ) );
        if( frac == 0 ) return QString( "%1K" ).arg( whole );
        return QString( "%1K%2" ).arg( whole ).arg( frac );
    }
    else
    {
        double m = R / 1e6;
        int whole = static_cast<int>( std::floor( m ) );
        int frac  = static_cast<int>( std::round( (m - whole) * 10.0 ) );
        if( frac == 0 ) return QString( "%1M" ).arg( whole );
        return QString( "%1M%2" ).arg( whole ).arg( frac );
    }
}

void CeramicResistor::updateDimensions()
{
    prepareGeometryChange();

    // Scale ceramic block and lead terminal spacing according to power rating
    // Note: m_pinDistance is kept as an exact multiple of 8 to ensure terminal pins snap cleanly to circuit grid
    if( m_powerRating <= 5.0 )
    {
        m_bodyLength  = 34.0;
        m_bodyHeight  = 16.0;
        m_pinDistance = 24.0; // 48 px pin-to-pin = 6 grid cells
    }
    else if( m_powerRating <= 7.0 )
    {
        m_bodyLength  = 44.0;
        m_bodyHeight  = 17.0;
        m_pinDistance = 32.0; // 64 px pin-to-pin = 8 grid cells
    }
    else if( m_powerRating <= 10.0 )
    {
        m_bodyLength  = 52.0;
        m_bodyHeight  = 18.0;
        m_pinDistance = 40.0; // 80 px pin-to-pin = 10 grid cells
    }
    else if( m_powerRating <= 15.0 )
    {
        m_bodyLength  = 62.0;
        m_bodyHeight  = 20.0;
        m_pinDistance = 48.0; // 96 px pin-to-pin = 12 grid cells
    }
    else if( m_powerRating <= 20.0 )
    {
        m_bodyLength  = 72.0;
        m_bodyHeight  = 22.0;
        m_pinDistance = 56.0; // 112 px pin-to-pin = 14 grid cells
    }
    else // 25 W+
    {
        m_bodyLength  = 84.0;
        m_bodyHeight  = 24.0;
        m_pinDistance = 64.0; // 128 px pin-to-pin = 16 grid cells
    }

    if( m_pin[0] && m_pin[1] )
    {
        m_pin[0]->setX( -m_pinDistance );
        m_pin[1]->setX(  m_pinDistance );

        double leadLen = m_pinDistance - (m_bodyLength / 2.0);
        if( leadLen < 2.0 ) leadLen = 2.0;
        if( std::abs( m_pin[0]->length() - leadLen ) > 0.01 )
        {
            m_pin[0]->setLength( leadLen );
            m_pin[1]->setLength( leadLen );
        }
    }

    m_area = QRectF( -m_pinDistance - 4.0, -m_bodyHeight/2.0 - 4.0
                   , m_pinDistance * 2.0 + 8.0, m_bodyHeight + 8.0 );
}

void CeramicResistor::drawRealistic( QPainter* p )
{
    p->save();
    p->setRenderHint( QPainter::Antialiasing, true );

    double L = m_bodyLength;
    double H = m_bodyHeight;
    double halfL = L / 2.0;
    double halfH = H / 2.0;
    double pinX  = m_pinDistance;

    // 1. Draw shiny metallic axial lead wires
    QPen wirePen( QColor( 185, 190, 195 ), 1.6, Qt::SolidLine, Qt::RoundCap );
    p->setPen( wirePen );
    p->drawLine( QPointF( -pinX, 0.0 ), QPointF( -halfL, 0.0 ) );
    p->drawLine( QPointF(  halfL, 0.0 ), QPointF(  pinX, 0.0 ) );

    // 2. Ceramic refractory cement body (rectangular block with beveled/rounded edges)
    QRectF bodyRect( -halfL, -halfH, L, H );

    // Subtle 3D ceramic block shading (light comes from top-left)
    QLinearGradient blockGrad( 0.0, -halfH, 0.0, halfH );
    blockGrad.setColorAt( 0.0,  QColor( 255, 255, 252 ) ); // Top white highlight
    blockGrad.setColorAt( 0.12, QColor( 248, 246, 240 ) ); // Main ceramic ivory
    blockGrad.setColorAt( 0.75, QColor( 236, 232, 222 ) );
    blockGrad.setColorAt( 1.0,  QColor( 212, 206, 194 ) ); // Bottom ambient shadow

    p->setBrush( blockGrad );
    p->setPen( QPen( QColor( 188, 182, 170 ), 0.9 ) );
    p->drawRoundedRect( bodyRect, 2.5, 2.5 );

    // Top edge specular bevel highlight
    p->setPen( QPen( QColor( 255, 255, 255, 220 ), 0.8 ) );
    p->drawLine( QPointF( -halfL + 2.0, -halfH + 1.0 ), QPointF( halfL - 2.0, -halfH + 1.0 ) );

    // Bottom edge shadow line
    p->setPen( QPen( QColor( 170, 165, 150, 160 ), 0.8 ) );
    p->drawLine( QPointF( -halfL + 2.0, halfH - 1.0 ), QPointF( halfL - 2.0, halfH - 1.0 ) );

    // 3. Cement end seals (where the wire enters the ceramic tube/casing)
    p->setBrush( QColor( 80, 75, 70 ) );
    p->setPen( Qt::NoPen );
    p->drawRoundedRect( QRectF( -halfL, -halfH * 0.45, 2.2, H * 0.9 ), 1.0, 1.0 );
    p->drawRoundedRect( QRectF(  halfL - 2.2, -halfH * 0.45, 2.2, H * 0.9 ), 1.0, 1.0 );

    // 4. Industrial stamped factory legend (Printed markings)
    // Line 1: Type / Power rating (e.g., "SQP 5W" or "SQP 10W")
    // Line 2: Resistance value and tolerance code (e.g., "10Ω J", "4R7 J", "100Ω K")
    int ratedW = static_cast<int>( std::round( m_powerRating ) );
    QString line1 = QString( "SQP %1W" ).arg( ratedW );
    QString line2 = QString( "%1 %2" ).arg( formatValueCode( m_nominalResistance ) )
                                       .arg( toleranceLetter( m_tolerance ) );

    QFont font = p->font();
    font.setFamily( "Monospace" );
    font.setStyleHint( QFont::Monospace );
    font.setBold( true );
    font.setPointSizeF( (H <= 16.0) ? 6.2 : 7.2 );
    p->setFont( font );

    QFontMetrics fm( font );
    int lineH = fm.height();
    double textY1 = -lineH * 0.9;
    double textY2 =  lineH * 0.1;

    p->setPen( QColor( 35, 35, 35 ) ); // Dark industrial stamped ink
    p->drawText( QRectF( -halfL + 3.0, textY1, L - 6.0, lineH ), Qt::AlignCenter, line1 );
    p->drawText( QRectF( -halfL + 3.0, textY2, L - 6.0, lineH ), Qt::AlignCenter, line2 );

    p->restore();
}

void CeramicResistor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    if( m_realistic && !m_ansiSymbol )
    {
        drawRealistic( p );
    }
    else
    {
        p->drawRect( m_area );
    }

    Component::paintSelected( p );
}
