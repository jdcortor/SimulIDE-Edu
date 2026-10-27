/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 *   ( see copyright.txt file at root folder )                             *
 ***************************************************************************/

#include <cmath>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>

#include "resistor.h"
#include "itemlibrary.h"
#include "pin.h"
#include "simulator.h"
#include "propdialog.h"

#include "doubleprop.h"
#include "stringprop.h"
#include "boolprop.h"

#define tr(str) simulideTr("Resistor",str)

Component* Resistor::construct( QString type, QString id )
{ return new Resistor( type, id ); }

LibraryItem* Resistor::libraryItem()
{
    return new LibraryItem(
        tr("Carbon Resistor"),
        "Resistors",
        "resistor.png",
        "Resistor",
        Resistor::construct);
}

Resistor::Resistor( QString type, QString id )
        : Comp2Pin( type, id )
        , eResistor( id )
{
    m_ePin[0] = m_pin[0];
    m_ePin[1] = m_pin[1];

    m_powerRating = 0.25; // 0.25 W (1/4 W standard)
    m_tolerance   = 5.0;  // 5% (Gold)
    m_bands       = 4;    // 4 bands by default
    m_tcr         = 100;  // 100 ppm/K (Brown)
    m_realistic   = true; // Realistic photorealistic 3D view
    m_bodyLength  = 19.0;
    m_bodyRadius  = 4.0;

    addPropGroup( { tr("Main"), {
        new DoubProp<Resistor>("Resistance", tr("Resistance"), "Ω"
                              , this, &Resistor::resistance, &Resistor::setResistance ),
        new DoubProp<Resistor>("PowerRating", tr("Power Rating"), "W"
                              , this, &Resistor::powerRating, &Resistor::setPowerRating ),
        new StrProp<Resistor>( "Bands", tr("Color Bands"), "4 Bands,5 Bands,6 Bands"
                              , this, &Resistor::bandsStr, &Resistor::setBandsStr, 0, "enum" ),
        new StrProp<Resistor>( "Tolerance", tr("Tolerance"), "0.1 %,0.25 %,0.5 %,1 %,2 %,5 %,10 %,20 %"
                              , this, &Resistor::toleranceStr, &Resistor::setToleranceStr, 0, "enum" ),
        new StrProp<Resistor>( "TCR", tr("Temp. Coeff. (6th)"), "100 ppm/K,50 ppm/K,25 ppm/K,15 ppm/K,10 ppm/K,5 ppm/K"
                              , this, &Resistor::tcrStr, &Resistor::setTcrStr, 0, "enum" ),
        new BoolProp<Resistor>("Realistic", tr("Realistic View"), ""
                              , this, &Resistor::realistic, &Resistor::setRealistic )
    }, 0 } );

    setShowProp("Resistance");
    setPropStr("Resistance", "100");

    updateDimensions();
    updateBands();
}

Resistor::~Resistor(){}

void Resistor::slotProperties()
{
    Component::slotProperties();
    if( m_propDialog )
    {
        m_propDialog->showProp( "TCR", m_bands == 6 );
        m_propDialog->adjustWidgets();
    }
}

void Resistor::setResistance( double resist )
{
    eResistor::setResistance( resist );
    updateBands();
    update();
}

void Resistor::setPowerRating( double p )
{
    if( p <= 0.0 ) p = 0.125;
    eResistor::setPowerRating( p );
    updateDimensions();
    update();
}

void Resistor::setTolerance( double t )
{
    if( t <= 0.0 ) t = 5.0;
    m_tolerance = t;
    updateBands();
    update();
}

QString Resistor::toleranceStr()
{
    return QString::number( m_tolerance ) + " %";
}

void Resistor::setToleranceStr( QString str )
{
    str.remove( "%" ).remove( " " );
    bool ok = false;
    double val = str.toDouble( &ok );
    if( ok && val > 0.0 )
    {
        m_tolerance = val;
        updateBands();
        update();
    }
}

void Resistor::setBands( int b )
{
    if( b >= 6 )      m_bands = 6;
    else if( b == 5 ) m_bands = 5;
    else              m_bands = 4;
    updateBands();
    if( m_propDialog )
    {
        m_propDialog->showProp( "TCR", m_bands == 6 );
        m_propDialog->adjustWidgets();
    }
    update();
}

QString Resistor::bandsStr()
{
    return QString::number( m_bands ) + " Bands";
}

void Resistor::setBandsStr( QString str )
{
    if( str.contains( "6" ) )      m_bands = 6;
    else if( str.contains( "5" ) ) m_bands = 5;
    else                           m_bands = 4;
    updateBands();
    if( m_propDialog )
    {
        m_propDialog->showProp( "TCR", m_bands == 6 );
        m_propDialog->adjustWidgets();
    }
    update();
}

void Resistor::setTcr( int t )
{
    m_tcr = t;
    updateBands();
    update();
}

QString Resistor::tcrStr()
{
    return QString::number( m_tcr ) + " ppm/K";
}

void Resistor::setTcrStr( QString str )
{
    str.remove( "ppm/K" ).remove( " " );
    bool ok = false;
    int val = str.toInt( &ok );
    if( ok && val > 0 )
    {
        m_tcr = val;
        updateBands();
        update();
    }
}

void Resistor::setRealistic( bool r )
{
    m_realistic = r;
    updateDimensions();
    update();
}

QColor Resistor::colorForDigit( int d )
{
    switch( d ) {
        case 0: return QColor( 25, 25, 25 );      // Black
        case 1: return QColor( 139, 69, 19 );     // Brown
        case 2: return QColor( 210, 25, 25 );     // Red
        case 3: return QColor( 255, 120, 0 );     // Orange
        case 4: return QColor( 245, 210, 0 );     // Yellow
        case 5: return QColor( 35, 145, 35 );     // Green
        case 6: return QColor( 30, 130, 235 );    // Blue
        case 7: return QColor( 145, 25, 195 );    // Violet
        case 8: return QColor( 128, 128, 128 );   // Gray
        case 9: return QColor( 240, 240, 240 );   // White
        default: return QColor( 25, 25, 25 );
    }
}

QColor Resistor::colorForMultiplier( int m )
{
    if( m == -1 ) return QColor( 212, 175, 55 );  // Gold
    if( m == -2 ) return QColor( 192, 192, 192 ); // Silver
    if( m >= 0 && m <= 9 ) return colorForDigit( m );
    return QColor( 25, 25, 25 );
}

QColor Resistor::colorForTolerance( double tol )
{
    if( std::abs( tol - 0.1 ) < 0.05 )  return QColor( 145, 25, 195 ); // Violet (0.1%)
    if( std::abs( tol - 0.25 ) < 0.1 )  return QColor( 30, 130, 235 ); // Blue (0.25%)
    if( std::abs( tol - 0.5 ) < 0.15 )  return QColor( 35, 145, 35 );  // Green (0.5%)
    if( std::abs( tol - 1.0 ) < 0.25 )  return QColor( 139, 69, 19 );  // Brown (1%)
    if( std::abs( tol - 2.0 ) < 0.25 )  return QColor( 210, 25, 25 );  // Red (2%)
    if( std::abs( tol - 5.0 ) < 1.0 )   return QColor( 212, 175, 55 );  // Gold (5%)
    if( std::abs( tol - 10.0 ) < 2.0 )  return QColor( 192, 192, 192 ); // Silver (10%)
    return QColor( 0, 0, 0, 0 );                                       // 20%: None
}

QColor Resistor::colorForTcr( int tcr )
{
    switch( tcr ) {
        case 100: return QColor( 139, 69, 19 );  // Brown (100 ppm/K)
        case 50:  return QColor( 210, 25, 25 );  // Red (50 ppm/K)
        case 25:  return QColor( 245, 210, 0 );  // Yellow (25 ppm/K)
        case 15:  return QColor( 255, 120, 0 );  // Orange (15 ppm/K)
        case 10:  return QColor( 30, 130, 235 ); // Blue (10 ppm/K)
        case 5:   return QColor( 145, 25, 195 ); // Violet (5 ppm/K)
        default:  return QColor( 139, 69, 19 );
    }
}

void Resistor::updateBands()
{
    m_bandColors.clear();
    double R = m_nominalResistance;
    if( R <= 0.0 ) R = 1e-3;

    int exponent = static_cast<int>( std::floor( std::log10( R ) ) );

    if( m_bands == 4 )
    {
        int multiplierExp = exponent - 1;
        double normalized = R / std::pow( 10.0, multiplierExp );
        int rounded = static_cast<int>( std::round( normalized ) );
        if( rounded >= 100 ) { rounded /= 10; multiplierExp++; }
        if( rounded < 10 ) rounded = 10;

        int d1 = (rounded / 10) % 10;
        int d2 = rounded % 10;

        m_bandColors.push_back( colorForDigit( d1 ) );
        m_bandColors.push_back( colorForDigit( d2 ) );
        m_bandColors.push_back( colorForMultiplier( multiplierExp ) );
        m_bandColors.push_back( colorForTolerance( m_tolerance ) );
    }
    else // 5 or 6 bands (EIA-96 standard: 3 significant digits)
    {
        int multiplierExp = exponent - 2;
        double normalized = R / std::pow( 10.0, multiplierExp );
        int rounded = static_cast<int>( std::round( normalized ) );
        if( rounded >= 1000 ) { rounded /= 10; multiplierExp++; }
        if( rounded < 100 ) rounded = 100;

        int d1 = (rounded / 100) % 10;
        int d2 = (rounded / 10) % 10;
        int d3 = rounded % 10;

        m_bandColors.push_back( colorForDigit( d1 ) );
        m_bandColors.push_back( colorForDigit( d2 ) );
        m_bandColors.push_back( colorForDigit( d3 ) );
        m_bandColors.push_back( colorForMultiplier( multiplierExp ) );
        m_bandColors.push_back( colorForTolerance( m_tolerance ) );

        if( m_bands == 6 )
        {
            m_bandColors.push_back( colorForTcr( m_tcr ) );
        }
    }
}

void Resistor::updateDimensions()
{
    prepareGeometryChange();

    if( !m_realistic || m_ansiSymbol )
    {
        m_area = QRectF( -11, -4.5, 22, 9 );
        if( m_pin[0] && m_pin[1] )
        {
            m_pin[0]->setLength( 4.4 );
            m_pin[1]->setLength( 4.4 );
        }
        return;
    }

    // Scale physical body dimensions according to rated power
    if( m_powerRating <= 0.125 )      { m_bodyLength = 15.0; m_bodyRadius = 3.0; } // 1/8 W
    else if( m_powerRating <= 0.25 )  { m_bodyLength = 19.0; m_bodyRadius = 4.0; } // 1/4 W
    else if( m_powerRating <= 0.5 )   { m_bodyLength = 22.0; m_bodyRadius = 4.8; } // 1/2 W
    else if( m_powerRating <= 1.0 )   { m_bodyLength = 25.0; m_bodyRadius = 5.8; } // 1 W
    else                              { m_bodyLength = 28.0; m_bodyRadius = 7.0; } // 2 W

    double halfL = m_bodyLength / 2.0;
    double rCap  = m_bodyRadius * 1.25;

    m_area = QRectF( -halfL - 2, -rCap - 2, m_bodyLength + 4, rCap * 2 + 4 );

    // Adjust lead pins to meet the body ends seamlessly
    if( m_pin[0] && m_pin[1] )
    {
        double pinLen = 16.0 - halfL;
        if( pinLen < 2.0 ) pinLen = 2.0;
        if( std::abs( m_pin[0]->length() - pinLen ) > 0.01 )
        {
            m_pin[0]->setLength( pinLen );
            m_pin[1]->setLength( pinLen );
        }
    }
}

void Resistor::drawRealistic( QPainter* p )
{
    p->save();
    p->setRenderHint( QPainter::Antialiasing, true );

    double L = m_bodyLength;
    double R = m_bodyRadius;
    double halfL = L / 2.0;
    double rCap  = R * 1.20;
    double capW  = L * 0.22;

    // 1. Draw metallic silver lead wires connecting body to grid pins
    QPen leadPen( QColor( 180, 185, 190 ), 1.3, Qt::SolidLine, Qt::RoundCap );
    p->setPen( leadPen );
    p->drawLine( QPointF( -16.0, 0.0 ), QPointF( -halfL, 0.0 ) );
    p->drawLine( QPointF(  halfL, 0.0 ), QPointF(  16.0, 0.0 ) );

    // 2. Build contoured axial body path (curved dogbone shoulders)
    QPainterPath bodyPath;
    bodyPath.moveTo( -halfL, -R );
    bodyPath.quadTo( -halfL + capW*0.5, -rCap, -halfL + capW, -rCap );
    bodyPath.quadTo( -halfL + capW*1.4, -R,    0.0,            -R );
    bodyPath.quadTo(  halfL - capW*1.4, -R,     halfL - capW, -rCap );
    bodyPath.quadTo(  halfL - capW*0.5, -rCap,  halfL,        -R );
    bodyPath.lineTo(  halfL,  R );
    bodyPath.quadTo(  halfL - capW*0.5,  rCap,  halfL - capW,  rCap );
    bodyPath.quadTo(  halfL - capW*1.4,  R,     0.0,            R );
    bodyPath.quadTo( -halfL + capW*1.4,  R,    -halfL + capW,  rCap );
    bodyPath.quadTo( -halfL + capW*0.5,  rCap, -halfL,         R );
    bodyPath.closeSubpath();

    // 3. Realistic cylindrical shading gradient
    QColor baseColor( 218, 188, 142 ); // Warm ceramic beige for standard 4-band carbon film
    if( m_bands >= 5 )
    {
        baseColor = QColor( 120, 140, 215 ); // Precision metal film light blue
    }

    QLinearGradient bodyGrad( 0.0, -rCap, 0.0, rCap );
    bodyGrad.setColorAt( 0.0,  baseColor.lighter( 155 ) ); // Upper specular highlight
    bodyGrad.setColorAt( 0.22, baseColor.lighter( 115 ) );
    bodyGrad.setColorAt( 0.55, baseColor );
    bodyGrad.setColorAt( 0.85, baseColor.darker( 135 ) );
    bodyGrad.setColorAt( 1.0,  baseColor.darker( 170 ) ); // Lower shadow

    p->setBrush( bodyGrad );
    p->setPen( QPen( baseColor.darker( 180 ), 0.6 ) );
    p->drawPath( bodyPath );

    // 4. Color Bands (clipped inside body path)
    p->save();
    p->setClipPath( bodyPath );

    size_t count = m_bandColors.size();
    double bandW = (L < 18.0) ? 1.5 : 2.1;
    std::vector<double> bandX;

    if( count == 4 )
    {
        bandX = { -halfL + capW * 0.75, -L * 0.12, L * 0.08, halfL - capW * 0.75 };
    }
    else if( count == 5 )
    {
        bandX = { -halfL + capW * 0.75, -L * 0.18, -L * 0.02, L * 0.14, halfL - capW * 0.75 };
    }
    else if( count == 6 )
    {
        bandX = { -halfL + capW * 0.75, -L * 0.22, -L * 0.08, L * 0.06, L * 0.20, halfL - capW * 0.75 };
    }

    for( size_t i = 0; i < count; ++i )
    {
        QColor c = m_bandColors[i];
        if( c.alpha() == 0 ) continue; // e.g. 20% no band

        QLinearGradient bandGrad( 0.0, -rCap, 0.0, rCap );
        bandGrad.setColorAt( 0.0,  c.lighter( 145 ) );
        bandGrad.setColorAt( 0.25, c.lighter( 110 ) );
        bandGrad.setColorAt( 0.65, c );
        bandGrad.setColorAt( 1.0,  c.darker( 155 ) );

        p->setBrush( bandGrad );
        p->setPen( Qt::NoPen );
        p->drawRect( QRectF( bandX[i] - bandW/2.0, -rCap - 1, bandW, (rCap + 1) * 2 ) );
    }

    p->restore(); // restore clip
    p->restore(); // restore painter state
}

void Resistor::drawAnsi( QPainter* p, int x, int y, double sX, double sY )
{
    QPen pen = p->pen();
    pen.setWidth(2);
    p->setPen( pen );
    p->drawLine( QPointF( sX*(-12+x),sY*( 0)+y ), QPointF( sX*(-10+x),sY*(-4)+y ) );
    p->drawLine( QPointF( sX*(-10+x),sY*(-4)+y ), QPointF( sX*( -6+x),sY*( 4)+y ) );
    p->drawLine( QPointF( sX*( -6+x),sY*( 4)+y ), QPointF( sX*( -2+x),sY*(-4)+y ) );
    p->drawLine( QPointF( sX*( -2+x),sY*(-4)+y ), QPointF( sX*(  2+x),sY*( 4)+y ) );
    p->drawLine( QPointF( sX*(  2+x),sY*( 4)+y ), QPointF( sX*(  6+x),sY*(-4)+y ) );
    p->drawLine( QPointF( sX*(  6+x),sY*(-4)+y ), QPointF( sX*( 10+x),sY*( 4)+y ) );
    p->drawLine( QPointF( sX*( 10+x),sY*( 4)+y ), QPointF( sX*( 12+x),sY*( 0)+y ) );
}

void Resistor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    if( m_realistic && !m_ansiSymbol )
    {
        drawRealistic( p );
    }
    else if( m_ansiSymbol )
    {
        Resistor::drawAnsi( p, 0, 0 );
    }
    else
    {
        p->drawRect( m_area );
    }

    Component::paintSelected( p );
}
