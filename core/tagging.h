/***************************************************************************
 *   Copyright (C) 2016 by Jonathan Schultz <jonathan@imatix.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _OKULAR_TAGGING_H_
#define _OKULAR_TAGGING_H_

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QLinkedList>
#include <QtCore/QRect>
#include <QtGui/QFont>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "okularcore_export.h"
#include "area.h"

namespace Okular {

class Page;
class Tagging;
class TaggingObjectRect;
class TaggingPrivate;
class TextTaggingPrivate;
class BoxTaggingPrivate;

class Node;

/**
 * @short Helper class for tagging retrieval/storage.
 */
class OKULARCORE_EXPORT TaggingUtils
{
    public:
        /**
         *
         * Returns a pointer to the complete tagging or 0 if element is invalid.
         */
        static Tagging * createTagging( const QDomElement & tagElement );

        /**
         */
        static void storeTagging( const Tagging * tag,
                                  QDomElement & element, QDomDocument & document );

        /**
         * Returns the child element with the given @p name from the direct
         * children of @p parentNode or a null element if not found.
         */
        static QDomElement findChildElement( const QDomNode & parentNode,
                                             const QString & name );

        /**
         * Returns the geometry of the given @p tagging scaled by
         * @p scaleX and @p scaleY.
         */
        static QRect taggingGeometry( const Tagging * tagging,
                                         double scaleX, double scaleY );
};

/**
 * @short Tagging struct holds properties shared by all taggings.
 *
 * An Tagging is an object (text note, highlight, sound, popup window, ..)
 * contained by a Page in the document.
 */
class OKULARCORE_EXPORT Tagging
{
    /// @cond PRIVATE
    friend class TaggingObjectRect;
    friend class Document;
    friend class DocumentPrivate;
    friend class ObjectRect;
    friend class Page;
    friend class PagePrivate;
    /// @endcond

    public:
        /**
         * Describes the type of tagging.
         */
        enum SubType
        {
            TText = 1,      ///< A textual tagging
            TBox = 2,       ///< A box tagging
            T_BASE = 0      ///< The tagging base class
        };

        /**
         * Describes the type of additional actions.
         *
         * @since 0.16 (KDE 4.10)
         */
        enum AdditionalActionType
        {
            PageOpening, ///< Performed when the page containing the tagging is opened.
            PageClosing  ///< Performed when the page containing the tagging is closed.
        };

        /**
         * A function to be called when the tagging is destroyed.
         *
         * @warning the function must *not* call any virtual function,
         *          nor subcast.
         *
         * @since 0.7 (KDE 4.1)
         */
        typedef void ( * DisposeDataFunction )( const Okular::Tagging * );

        /**
         * Destroys the tagging.
         */
        virtual ~Tagging();

		Tagging( );
		Tagging( NormalizedRect *rect );

        void setNode ( Node *node );

        Node *node() const;

        /**
         * Sets the @p author of the tagging.
         */
        void setAuthor( const QString &author );

        /**
         * Returns the author of the tagging.
         */
        QString author() const;

        /**
         * Sets the unique @p name of the annotation.
         */
        void setUniqueName( const QString &name );

        /**
         * Returns the unique name of the annotation.
         */
        QString uniqueName() const;

        /**
         * Sets the last modification @p date of the tagging.
         *
         * The date must be before or equal to QDateTime::currentDateTime()
         */
        void setModificationDate( const QDateTime &date );

        /**
         * Returns the last modification date of the tagging.
         */
        QDateTime modificationDate() const;

        /**
         * Sets the creation @p date of the tagging.
         *
         * The date must be before or equal to @see modificationDate()
         */
        void setCreationDate( const QDateTime &date );

        /**
         * Returns the creation date of the tagging.
         */
        QDateTime creationDate() const;

        /**
         * Sets the @p flags of the tagging.
         * @see @ref Flag
         */
        void setFlags( int flags );

        /**
         * Returns the flags of the tagging.
         * @see @ref Flag
         */
        int flags() const;

		/**
         * Sets the bounding @p rectangle of the tagging.
         */
        void setBoundingRectangle( const NormalizedRect &rectangle );

        /**
         * Returns the bounding rectangle of the tagging.
         */
        NormalizedRect boundingRectangle() const;

        /**
         * Returns the transformed bounding rectangle of the tagging.
         *
         * This rectangle must be used when showing taggings on screen
         * to have them rotated correctly.
         */
        NormalizedRect transformedBoundingRectangle() const;

        /**
         * Move the tagging by the specified coordinates.
         *
         * @see canBeMoved()
         */
        void translate( const NormalizedPoint &coord );

        /**
         * Sets a function to be called when the tagging is destroyed.
         *
         * @warning the function must *not* call any virtual function,
         *          nor subcast.
         *
         * @since 0.7 (KDE 4.1)
         */
        void setDisposeDataFunction( DisposeDataFunction func );

        /**
         * Returns the sub type of the tagging.
         */
        virtual SubType subType() const = 0;

        /**
         * Retrieve the QDomNode representing this annotation's properties

         * @since 0.17 (KDE 4.11)
         */
        QDomNode getTaggingPropertiesDomNode() const;

        /**
         * Stores the tagging as xml in @p document under the given parent @p node.
         */
        virtual void store( QDomNode & node, QDomDocument & document ) const;

        /**
         * Sets annotations internal properties according to the contents of @p node
         *
         * @since 0.17 (KDE 4.11)
         */
        void setTaggingProperties( const QDomNode & node );

        /**
         * Returns the page to which the tagging is attached
         */
        const Page * page() const;


    protected:
        /// @cond PRIVATE
        Tagging( TaggingPrivate &dd );
        Tagging( TaggingPrivate &dd, const QDomNode &description );
        Q_DECLARE_PRIVATE( Tagging )
        TaggingPrivate *d_ptr;
        /// @endcond

    private:
        Q_DISABLE_COPY( Tagging )
};

class OKULARCORE_EXPORT TextTagging : public Tagging
{
    public:
        /**
         * Creates a new text tagging.
         */
        TextTagging();

        TextTagging( const RegularAreaRect * );

        /**
         * Creates a new text tagging from the xml @p description
         */
        TextTagging( const QDomNode &description );

        /**
         * Destroys the text tagging.
         */
        ~TextTagging();

        /**
         * Returns the sub type of the text tagging.
         */
        SubType subType() const;

        const RegularAreaRect * transformedTextArea () const;

        /**
         * Stores the tagging as xml in @p document under the given parent @p node.
         */
        void store( QDomNode &node, QDomDocument &document ) const;

    private:
        Q_DECLARE_PRIVATE( TextTagging )
        Q_DISABLE_COPY( TextTagging )
};

class OKULARCORE_EXPORT BoxTagging : public Tagging
{
    public:
        /**
         * Creates a new box tagging.
         */
        BoxTagging( );

        BoxTagging( const NormalizedRect *rect );

        /**
         * Creates a new box tagging from the xml @p description
         */
        BoxTagging( const QDomNode &description );

        /**
         * Destroys the text tagging.
         */
        ~BoxTagging();

        /**
         * Returns the sub type of the box tagging.
         */
        SubType subType() const;

        /**
         * Stores the tagging as xml in @p document under the given parent @p node.
         */
        void store( QDomNode &node, QDomDocument &document ) const;

    private:
        Q_DECLARE_PRIVATE( BoxTagging )
        Q_DISABLE_COPY( BoxTagging )
};

/**
 * @short Helper class for node retrieval/storage.
 */
class OKULARCORE_EXPORT NodeUtils
{
    public:
        static QList< Node * > * Nodes ;

        static Node * retrieveNode(int id);
};

class OKULARCORE_EXPORT Node
{
    friend class NodeUtils;

	public:
        Node();
		~Node();

        unsigned int color() const;
        int id()             const;

    protected:
        int m_id;
};

}

#endif
