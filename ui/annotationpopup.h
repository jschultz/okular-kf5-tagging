/***************************************************************************
 *   Copyright (C) 2006 by Tobias Koenig <tokoe@kde.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ANNOTATIONPOPUP_H
#define ANNOTATIONPOPUP_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QPoint>

namespace Okular {
class Annotation;
class Document;
class Tagging;
}

class AnnotationPopup : public QObject
{
    Q_OBJECT

    public:
        /**
         * Describes the structure of the popup menu.
         */
        enum MenuMode
        {
            SingleAnnotationMode, ///< The menu shows only entries to manipulate a single annotation, or multiple annotations as a group.
            MultiAnnotationMode   ///< The menu shows entries to manipulate multiple annotations.
        };

        AnnotationPopup( Okular::Document *document, MenuMode mode, QWidget *parent = Q_NULLPTR );

        void addAnnotation( Okular::Annotation* annotation, int pageNumber );

        void addTagging( Okular::Tagging* tagging, int pageNumber );

        void exec( const QPoint &point = QPoint() );

    Q_SIGNALS:
        void openAnnotationWindow( Okular::Annotation *annotation, int pageNumber );
        void openTaggingWindow( Okular::Tagging *tagging, int pageNumber );

    public:
        struct AnnotOrTagPagePair {
            AnnotOrTagPagePair() : annotation( 0 ),  tagging( 0 ), pageNumber( -1 )
            { }

            AnnotOrTagPagePair( Okular::Annotation *a, int pn ) : annotation( a ),  tagging( 0 ), pageNumber( pn )
            { }

            AnnotOrTagPagePair( Okular::Tagging *t, int pn ) : annotation( 0 ),  tagging( t ), pageNumber( pn )
            { }

            AnnotOrTagPagePair( const AnnotOrTagPagePair & pair ) : annotation( pair.annotation ), tagging( pair.tagging ),  pageNumber( pair.pageNumber )
            { }

            bool operator==( const AnnotOrTagPagePair & pair ) const
            { return annotation == pair.annotation && tagging == pair.tagging && pageNumber == pair.pageNumber; }

            Okular::Annotation* annotation;
            Okular::Tagging* tagging;
            int pageNumber;
        };

    public:
        struct TaggingPagePair {
            TaggingPagePair() : tagging( 0 ),  pageNumber( -1 )
            { }

            TaggingPagePair( Okular::Tagging *a, int pn ) : tagging( a ),  pageNumber( pn )
            { }

            TaggingPagePair( const TaggingPagePair & pair ) : tagging( pair.tagging ),  pageNumber( pair.pageNumber )
            { }

            bool operator==( const TaggingPagePair & pair ) const
            { return tagging == pair.tagging && pageNumber == pair.pageNumber; }

            Okular::Tagging* tagging;
            int pageNumber;
        };

    private:
        QWidget *mParent;

        QList< AnnotOrTagPagePair > mAnnotsAndTags;
        Okular::Document *mDocument;
        MenuMode mMenuMode;
};


#endif
