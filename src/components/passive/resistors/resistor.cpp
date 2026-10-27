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
#include <QFont>
#include <QFontMetrics>

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
        tr("Resistor"),
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

    if( Simulator::self() ) Simulator::self()->addToUpdateList( this );
}

Resistor::~Resistor()
{
    if( Simulator::self() ) Simulator::self()->remFromUpdateList( this );
}

void Resistor::slotProperties()
{
    Component::slotProperties();
    if( m_propDialog )
    {
        m_propDialog->showProp( "TCR", m_bands == 6 );
        m_propDialog->adjustWidgets();
    }
}

void Resistor::updateStep()
{
    if( m_isBurnt ) return;
    if( !Simulator::self() || !Simulator::self()->isRunning() ) return;
    if( !m_pin[0] || !m_pin[1] ) return;
    if( !m_pin[0]->isConnected() || !m_pin[1]->isConnected() ) return;

    double volt = std::abs( m_pin[0]->getVoltage() - m_pin[1]->getVoltage() );
    double power = volt * volt * m_admit;

    if( m_powerRating > 0.0 && power > m_powerRating )
    {
        qDebug() << "Resistor" << m_id << "burned out! Power:" << power << "W > Rated:" << m_powerRating << "W";
        burnOut();
    }
}

void Resistor::setResistance( double resist )
{
    eResistor::setResistance( resist );
    if( m_isBurnt ) setIsBurnt( false );
    updateBands();
    update();
}

void Resistor::setPowerRating( double p )
{
    if( p <= 0.0 ) p = 0.125;
    eResistor::setPowerRating( p );
    if( m_isBurnt ) setIsBurnt( false );
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

void Resistor::setIsBurnt( bool b )
{
    prepareGeometryChange();
    eResistor::setIsBurnt( b );
    updateDimensions();
    update();
}

void Resistor::onBurned()
{
    prepareGeometryChange();
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
    if( std::abs( tol - 10.0 ) < 1.5 )  return QColor( 192, 192, 192 ); // Silver (10%)
    if( std::abs( tol - 20.0 ) < 2.0 )  return QColor( 0, 0, 0, 0 );    // No band (20%)
    return QColor( 212, 175, 55 ); // Default Gold (5%)
}

QColor Resistor::colorForTcr( int tcr )
{
    switch( tcr ) {
        case 100: return QColor( 139, 69, 19 );  // Brown (100 ppm)
        case 50:  return QColor( 210, 25, 25 );  // Red (50 ppm)
        case 15:  return QColor( 255, 120, 0 );  // Orange (15 ppm)
        case 25:  return QColor( 245, 210, 0 );  // Yellow (25 ppm)
        case 10:  return QColor( 30, 130, 235 ); // Blue (10 ppm)
        case 5:   return QColor( 145, 25, 195 ); // Violet (5 ppm)
        default:  return QColor( 139, 69, 19 );
    }
}

void Resistor::updateBands()
{
    double R = m_nominalResistance;
    if( R <= 0.0 ) R = 1.0;

    m_bandColors.clear();

    if( m_bands >= 5 ) // 5 or 6 bands (3 significant digits)
    {
        double exp = std::floor( std::log10( R ) );
        double mantissa = R / std::pow( 10.0, exp );
        double val = std::round( mantissa * 100.0 );
        int exp_adj = (int)exp - 2;
        if( val >= 1000.0 ) { val /= 10.0; exp_adj += 1; }
        int ival = (int)val;
        int b1 = ival / 100;
        int b2 = (ival / 10) % 10;
        int b3 = ival % 10;
        int mult = exp_adj;

        m_bandColors.push_back( colorForDigit( b1 ) );
        m_bandColors.push_back( colorForDigit( b2 ) );
        m_bandColors.push_back( colorForDigit( b3 ) );
        m_bandColors.push_back( colorForMultiplier( mult ) );
        m_bandColors.push_back( colorForTolerance( m_tolerance ) );

        if( m_bands == 6 )
        {
            m_bandColors.push_back( colorForTcr( m_tcr ) );
        }
    }
    else // 4 bands (2 significant digits)
    {
        double exp = std::floor( std::log10( R ) );
        double mantissa = R / std::pow( 10.0, exp );
        double val = std::round( mantissa * 10.0 );
        int exp_adj = (int)exp - 1;
        if( val >= 100.0 ) { val /= 10.0; exp_adj += 1; }
        int ival = (int)val;
        int b1 = ival / 10;
        int b2 = ival % 10;
        int mult = exp_adj;

        m_bandColors.push_back( colorForDigit( b1 ) );
        m_bandColors.push_back( colorForDigit( b2 ) );
        m_bandColors.push_back( colorForMultiplier( mult ) );
        m_bandColors.push_back( colorForTolerance( m_tolerance ) );
    }
}

void Resistor::updateDimensions()
{
    if( !m_realistic )
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

    if( m_isBurnt )
    {
        // Expand bounding area to encompass the warning banner above
        m_area = QRectF( -125.0, -38.0, 250.0, 42.0 + rCap * 2.0 );
    }
    else
    {
        m_area = QRectF( -halfL - 2, -rCap - 2, m_bodyLength + 4, rCap * 2 + 4 );
    }

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
    double x0 = -halfL;
    double x1 =  halfL;

    bodyPath.moveTo( x0 + capW, -rCap );
    bodyPath.quadTo( x0, -rCap, x0, 0.0 );
    bodyPath.quadTo( x0,  rCap, x0 + capW,  rCap );
    bodyPath.lineTo( x0 + capW * 1.35,  R );
    bodyPath.lineTo( x1 - capW * 1.35,  R );
    bodyPath.lineTo( x1 - capW,  rCap );
    bodyPath.quadTo( x1,  rCap, x1, 0.0 );
    bodyPath.quadTo( x1, -rCap, x1 - capW, -rCap );
    bodyPath.lineTo( x1 - capW * 1.35, -R );
    bodyPath.lineTo( x0 + capW * 1.35, -R );
    bodyPath.closeSubpath();

    // 3. 3D Cylindrical lighting gradient for the body
    QLinearGradient bodyGrad( 0.0, -rCap, 0.0, rCap );
    if( m_isBurnt )
    {
        bodyGrad.setColorAt( 0.0,  QColor( 55, 45, 40 ) );
        bodyGrad.setColorAt( 0.25, QColor( 30, 25, 22 ) );
        bodyGrad.setColorAt( 0.65, QColor( 18, 15, 13 ) );
        bodyGrad.setColorAt( 1.0,  QColor( 35, 28, 24 ) );
    }
    else
    {
        // 4 Bands: Tan/cream (Carbon film)
        // 5 Bands: Sky blue (Metal film)
        // 6 Bands: Indigo-blue (High precision film)
        QColor base;
        if( m_bands == 6 )      base = QColor( 120, 140, 215 );
        else if( m_bands == 5 ) base = QColor( 135, 195, 245 );
        else                    base = QColor( 225, 202, 160 );

        bodyGrad.setColorAt( 0.0,  base.lighter( 145 ) );
        bodyGrad.setColorAt( 0.25, base.lighter( 115 ) );
        bodyGrad.setColorAt( 0.65, base );
        bodyGrad.setColorAt( 1.0,  base.darker( 150 ) );
    }

    QColor outlineColor = m_isBurnt ? QColor( 20, 15, 12 ) : QColor( 120, 100, 75 );
    p->setBrush( bodyGrad );
    p->setPen( QPen( outlineColor, 0.8 ) );
    p->drawPath( bodyPath );

    // 4. Draw color bands clipped inside the contoured body
    p->save();
    p->setClipPath( bodyPath );

    double bandW = (m_bands == 6) ? (L * 0.070) : (L * 0.085);
    if( bandW < 1.4 ) bandW = 1.4;

    std::vector<double> bandX;
    if( m_bands == 6 )
    {
        bandX.push_back( x0 + L * 0.17 );
        bandX.push_back( x0 + L * 0.30 );
        bandX.push_back( x0 + L * 0.43 );
        bandX.push_back( x0 + L * 0.56 );
        bandX.push_back( x1 - L * 0.26 ); // Tolerance
        bandX.push_back( x1 - L * 0.13 ); // TCR (6th band)
    }
    else if( m_bands == 5 )
    {
        bandX.push_back( x0 + L * 0.20 );
        bandX.push_back( x0 + L * 0.35 );
        bandX.push_back( x0 + L * 0.50 );
        bandX.push_back( x0 + L * 0.65 );
        bandX.push_back( x1 - L * 0.20 ); // Tolerance band
    }
    else // 4 bands
    {
        bandX.push_back( x0 + L * 0.22 );
        bandX.push_back( x0 + L * 0.40 );
        bandX.push_back( x0 + L * 0.58 );
        bandX.push_back( x1 - L * 0.22 ); // Tolerance band
    }

    size_t count = std::min( m_bandColors.size(), bandX.size() );
    for( size_t i = 0; i < count; ++i )
    {
        QColor c = m_bandColors[i];
        if( c.alpha() == 0 ) continue; // e.g. 20% no band

        if( m_isBurnt ) c = c.darker( 300 ); // charred band color

        QLinearGradient bandGrad( 0.0, -rCap, 0.0, rCap );
        bandGrad.setColorAt( 0.0,  c.lighter( 145 ) );
        bandGrad.setColorAt( 0.25, c.lighter( 110 ) );
        bandGrad.setColorAt( 0.65, c );
        bandGrad.setColorAt( 1.0,  c.darker( 155 ) );

        p->setBrush( bandGrad );
        p->setPen( Qt::NoPen );
        p->drawRect( QRectF( bandX[i] - bandW/2.0, -rCap - 1, bandW, (rCap + 1) * 2 ) );
    }

    // 5. If burnt: render central scorch mark and fissure crack
    if( m_isBurnt )
    {
        QRadialGradient scorch( 0.0, 0.0, L * 0.35 );
        scorch.setColorAt( 0.0,  QColor( 10, 10, 10, 240 ) );
        scorch.setColorAt( 0.45, QColor( 35, 20, 10, 190 ) );
        scorch.setColorAt( 1.0,  QColor( 0, 0, 0, 0 ) );

        p->setBrush( scorch );
        p->setPen( Qt::NoPen );
        p->drawEllipse( QPointF( 0.0, 0.0 ), L * 0.38, rCap * 1.15 );

        // Charred fissure crack across center
        QPen crackPen( QColor( 5, 5, 5, 230 ), 0.9 );
        p->setPen( crackPen );
        p->drawLine( QPointF( -L*0.12, -rCap*0.55 ), QPointF( -L*0.02, 0.0 ) );
        p->drawLine( QPointF( -L*0.02, 0.0 ),        QPointF(  L*0.10, rCap*0.60 ) );

        p->restore(); // restore clip from bodyPath

        // 6. Draw educational warning banner above the resistor:
        QString msg = "La resistencia se ha quemado por exceso de corriente";
        QFont font = p->font();
        font.setPointSizeF( 6.5 );
        font.setBold( true );
        p->setFont( font );

        QFontMetrics fm( font );
        int textW = fm.horizontalAdvance( msg );
        double bannerW = textW + 12.0;
        double bannerH = 15.0;
        double bannerX = -bannerW / 2.0;
        double bannerY = -rCap - 19.0;

        // Warning banner box
        p->setBrush( QColor( 195, 25, 25, 235 ) ); // Intense red warning
        p->setPen( QPen( QColor( 255, 255, 255, 220 ), 0.8 ) );
        p->drawRoundedRect( QRectF( bannerX, bannerY, bannerW, bannerH ), 3.0, 3.0 );

        // Small indicator triangle pointing to the burned body
        QPolygonF arrow;
        arrow << QPointF( -3.0, bannerY + bannerH )
              << QPointF(  3.0, bannerY + bannerH )
              << QPointF(  0.0, bannerY + bannerH + 3.0 );
        p->drawPolygon( arrow );

        // Warning message text
        p->setPen( QColor( 255, 255, 255 ) );
        p->drawText( QRectF( bannerX, bannerY, bannerW, bannerH ), Qt::AlignCenter, msg );
    }
    else
    {
        p->restore(); // restore clip
    }

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
