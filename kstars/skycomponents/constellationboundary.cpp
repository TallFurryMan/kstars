/***************************************************************************
                 constellationboundary.cpp -  K Desktop Planetarium
                             -------------------
    begin                : 2007-07-10
    copyright            : (C) 2007 by James B. Bowlin
    email                : bowlin@mindspring.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "constellationboundary.h"

#include <stdio.h>

#include <kdebug.h>
#include <klocale.h>
#include <QPolygonF>

#include "Options.h"
#include "ksfilereader.h"
#include "skyobjects/skyobject.h"
#include "skycomponents/skymesh.h"

#include "polylist.h"

#include "skymesh.h"

ConstellationBoundary::ConstellationBoundary( SkyComposite *parent ) :
    SkyComponent( parent )
{
    m_skyMesh = SkyMesh::Instance();
    m_polyIndexCnt = 0;
    for(int i = 0; i < m_skyMesh->size(); i++) {
        m_polyIndex.append( new PolyListList() );
    }
}

void ConstellationBoundary::appendPoly( PolyList* polyList, KSFileReader* file, int debug)
{
    if ( ! file || debug == -1)
        return appendPoly( polyList, debug );

    m_nameHash.insert( polyList->name(), polyList );
    if( !polyList->localizedName().isEmpty() )
        m_nameHash.insert( polyList->localizedName(), polyList );

    while ( file->hasMoreLines() ) {
        QString line = file->readLine();
        if ( line.at( 0 ) == ':' ) return;
        Trixel trixel = line.toInt();
        m_polyIndex[ trixel ]->append( polyList );
    }
}

void ConstellationBoundary::appendPoly( PolyList* polyList, int debug)
{
    m_nameHash.insert( polyList->name(), polyList );
    if( !polyList->localizedName().isEmpty() )
        m_nameHash.insert( polyList->localizedName(), polyList );

    if ( debug >= 0 && debug < m_skyMesh->debug() ) debug = m_skyMesh->debug();

    const IndexHash& indexHash = m_skyMesh->indexPoly( polyList->poly() );
    IndexHash::const_iterator iter = indexHash.constBegin();
    while ( iter != indexHash.constEnd() ) {
        Trixel trixel = iter.key();
        iter++;

        if ( debug == -1 ) printf("%d\n", trixel );

        m_polyIndex[ trixel ]->append( polyList );
    }

    if ( debug > 9 )
        printf("PolyList: %3d: %d\n", ++m_polyIndexCnt, indexHash.size() );
}


void ConstellationBoundary::summary()
{
    if ( m_skyMesh->debug() < 2 ) return;

    int total = m_skyMesh->size();
    int polySize = m_polyIndex.size();

    /**
    for ( int i = 0; i < polySize; i++ ) {
    	PolyListList* listList = m_polyIndex.at( i );
    	printf("%4d: %d\n", i, listList->size() );
    }
    **/

    if ( polySize > 0 )
        printf("%4d out of %4d trixels in poly index %3d%%\n",
               polySize, total, 100 * polySize / total );

}

PolyList* ConstellationBoundary::ContainingPoly( SkyPoint *p )
{
    //printf("called ContainingPoly(p)\n");

    // we save the pointers in a hash because most often there is only one
    // constellation and we can avoid doing the expensive boundary calculations
    // and just return it if we know it is unique.  We can avoid this minor
    // complication entirely if we use index(p) instead of aperture(p, r)
    // because index(p) always returns a single trixel index.

    QHash<PolyList*, bool> polyHash;
    QHash<PolyList*, bool>::const_iterator iter;

    //printf("\n");

    // the boundaries don't precess so we use index() not aperture()
    m_skyMesh->index( p, 1.0, IN_CONSTELL_BUF );
    MeshIterator region( m_skyMesh, IN_CONSTELL_BUF );
    while ( region.hasNext() ) {

        Trixel trixel = region.next();
        //printf("Trixel: %4d %s\n", trixel, m_skyMesh->indexToName( trixel ) );

        PolyListList *polyListList = m_polyIndex[ trixel ];

        //printf("    size: %d\n", polyListList->size() );

        for (int i = 0; i < polyListList->size(); i++) {
            PolyList* polyList = polyListList->at( i );
            polyHash.insert( polyList, true );
        }
    }

    iter = polyHash.constBegin();

    // Don't bother with boundaries if there is only one
    if ( polyHash.size() == 1 ) return iter.key();

    QPointF point( p->ra()->Hours(), p->dec()->Degrees() );
    QPointF wrapPoint( p->ra()->Hours() - 24.0, p->dec()->Degrees() );
    bool wrapRA = p->ra()->Hours() > 12.0;

    while ( iter != polyHash.constEnd() ) {

        PolyList* polyList = iter.key();
        iter++;

        //kDebug() << QString("checking %1 boundary\n").arg( polyList->name() );

        const QPolygonF* poly = polyList->poly();
        if ( wrapRA && polyList->wrapRA() ) {
            if ( poly->containsPoint( wrapPoint, Qt::OddEvenFill ) )
                return polyList;
        }
        else {
            if ( poly->containsPoint( point, Qt::OddEvenFill ) )
                return polyList;
        }
    }

    return 0;
}


//-------------------------------------------------------------------
// The routines for providing public access to the boundary index
// start here.  (Some of them may not be needed (or working)).
//-------------------------------------------------------------------

QString ConstellationBoundary::constellationName( SkyPoint *p )
{
    PolyList *polyList = ContainingPoly( p );
    if ( polyList ) {
        return ( Options::useLocalConstellNames() ?
                 i18nc( "Constellation name (optional)", polyList->name().toUpper().toLocal8Bit().data() ) :
                 polyList->name() );
    }
    return i18n("Unknown");
}

const QPolygonF* ConstellationBoundary::constellationPoly( const QString &name )
{
    if ( m_nameHash.contains( name ) )
        return m_nameHash.value( name )->poly();
    return 0;
}

const QPolygonF* ConstellationBoundary::constellationPoly( SkyPoint *p )
{
    PolyList *polyList = ContainingPoly( p );
    if ( polyList )
        return polyList->poly();
    return 0;
}

bool ConstellationBoundary::inConstellation( const QString &name, SkyPoint *p )
{
    PolyList* polyList = m_nameHash.value( name );
    if ( !polyList )
        return false;
    const QPolygonF* poly = polyList->poly();
    return poly->containsPoint( QPointF( p->ra()->Hours(),
                                         p->dec()->Degrees() ), Qt::OddEvenFill );
}



void ConstellationBoundary::init() {}
void ConstellationBoundary::draw(QPainter&) {}
