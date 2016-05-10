/***************************************************************************
 *   Copyright (C) 2016 by Jonathan Schultz <jonathan@imatix.com?          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _OKULAR_QDANODES_H_
#define _OKULAR_QDANODES_H_

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QLinkedList>
#include <QtGui/QColor>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "okularcore_export.h"

namespace Okular {

class QDANode;
class QDANodeUtils;

/**
 * @short Helper class for node retrieval/storage.
 */
class OKULARCORE_EXPORT QDANodeUtils
{
    public:
        static QRgb tagColors [];

        static QList< QDANode * > * QDANodes ;

        static QDANode * retrieve( QString m_uniqueName );

        static void storeQDANodes( QDomElement & QDAElement, QDomDocument & doc );
        static void load(const QDomNode& node);
};

class OKULARCORE_EXPORT QDANode
{
    friend class QDANodeUtils;

    public:
        QDANode();
        QDANode( QString uniqueName );
        ~QDANode();

        void store( QDomNode & QDANode, QDomDocument & document ) const;

        QString uniqueName() const;

        void setName( QString name );
        QString name() const;

        void setColor( QRgb color );
        QRgb color () const;

        void setAuthor( QString author );
        QString author() const;

        void setCreationDate( QDateTime creationDate );
        QDateTime creationDate() const;

        void setModificationDate( QDateTime modificationDate );
        QDateTime modificationDate() const;

    protected:
        QString m_uniqueName;
        QString m_name;
        QRgb    m_color;
        QString m_author;
        QDateTime m_creationDate;
        QDateTime m_modifyDate;
};

}

#endif