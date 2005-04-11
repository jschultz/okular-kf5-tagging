/***************************************************************************
 *   Copyright (C) 2004 by Enrico Ros <eros.kde@email.it>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _KPDF_PRESENTATIONWIDGET_H_
#define _KPDF_PRESENTATIONWIDGET_H_

#include <qwidget.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qvaluevector.h>
#include "core/observer.h"
#include "core/pagetransition.h"

class KToolBar;
class QTimer;

class KPDFDocument;
class KPDFPage;
class PresentationFrame;

/**
 * @short A widget that shows pages as fullscreen slides (with transitions fx).
 *
 * This is a fullscreen widget that displays 
 */
class PresentationWidget : public QWidget, public DocumentObserver
{
    Q_OBJECT
    public:
        PresentationWidget( KPDFDocument * doc );
        ~PresentationWidget();

        // inherited from DocumentObserver
        uint observerId() const { return PRESENTATION_ID; }
        void notifySetup( const QValueVector< KPDFPage * > & pages, bool documentChanged );
        void notifyViewportChanged( bool smoothMove );
        void notifyPageChanged( int pageNumber, int changedFlags );
        bool canUnloadPixmap( int pageNumber );

    protected:
        // widget events
        void keyPressEvent( QKeyEvent * e );
        void wheelEvent( QWheelEvent * e );
        void mousePressEvent( QMouseEvent * e );
        void mouseMoveEvent( QMouseEvent * e );
        void paintEvent( QPaintEvent * e );

    private:
        void overlayClick( const QPoint & position );
        void changePage( int newPage );
        void generatePage();
        void generateIntroPage( QPainter & p );
        void generateContentsPage( int page, QPainter & p );
        void generateOverlay();
        void initTransition( const KPDFPageTransition *transition );
        const KPDFPageTransition defaultTransition() const;
        const KPDFPageTransition defaultTransition( int ) const;

        // cache stuff
        int m_width;
        int m_height;
        QPixmap m_lastRenderedPixmap;
        QPixmap m_lastRenderedOverlay;
        QRect m_overlayGeometry;

        // transition related
        QTimer * m_transitionTimer;
        QTimer * m_overlayHideTimer;
        int m_transitionDelay;
        int m_transitionMul;
        QValueList< QRect > m_transitionRects;

        // misc stuff
        KPDFDocument * m_document;
        QValueVector< PresentationFrame * > m_frames;
        int m_frameIndex;
        QStringList m_metaStrings;
        KToolBar * m_topBar;

    private slots:
        void slotNextPage();
        void slotPrevPage();
        void slotFirstPage();
        void slotLastPage();
        void slotHideOverlay();
        void slotTransitionStep();
};

#endif
