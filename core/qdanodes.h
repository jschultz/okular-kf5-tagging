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
        static QList< QDANode * > * QDANodes ;

        static void storeQDANodes( QDomElement & QDAElement, QDomDocument & doc );

        static QDANode * retrieveNode(int id);
};

class OKULARCORE_EXPORT QDANode
{
    friend class QDANodeUtils;

    public:
        QDANode();
        QDANode(const QDomNode& node);
        ~QDANode();

        void store( QDomNode & QDANode, QDomDocument & document ) const;

        unsigned int color() const;
        int id()             const;

        void setName( QString name );
        QString Name();

    protected:
        int m_id;

    private:
        QString m_name;
        QString m_author;
        QString m_uniqueName;
        QDateTime m_modifyDate;
        QDateTime m_creationDate;
};

}

#endif