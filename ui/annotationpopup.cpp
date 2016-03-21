/***************************************************************************
 *   Copyright (C) 2006 by Tobias Koenig <tokoe@kde.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "annotationpopup.h"

#include <KLocalizedString>
#include <qapplication.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <QMenu>
#include <QIcon>

#include "annotationpropertiesdialog.h"
#include "taggingpropertiesdialog.h"

#include "core/annotations.h"
#include "core/document.h"
#include "core/page.h"
#include "core/tagging.h"
#include "guiutils.h"

Q_DECLARE_METATYPE( AnnotationPopup::AnnotOrTagPagePair )

namespace {

bool annotationHasFileAttachment( Okular::Annotation *annotation )
{
    return ( annotation->subType() == Okular::Annotation::AFileAttachment || annotation->subType() == Okular::Annotation::ARichMedia );
}

Okular::EmbeddedFile* embeddedFileFromAnnotation( Okular::Annotation *annotation )
{
    if ( annotation->subType() == Okular::Annotation::AFileAttachment )
    {
        const Okular::FileAttachmentAnnotation *fileAttachAnnot = static_cast<Okular::FileAttachmentAnnotation*>( annotation );
        return fileAttachAnnot->embeddedFile();
    }
    else if ( annotation->subType() == Okular::Annotation::ARichMedia )
    {
        const Okular::RichMediaAnnotation *richMediaAnnot = static_cast<Okular::RichMediaAnnotation*>( annotation );
        return richMediaAnnot->embeddedFile();
    }
    else
    {
        return 0;
    }
}

}

AnnotationPopup::AnnotationPopup( Okular::Document *document, MenuMode mode,
                                  QWidget *parent )
    : mParent( parent ), mDocument( document ), mMenuMode( mode )
{
}

void AnnotationPopup::addAnnotation( Okular::Annotation* annotation, int pageNumber )
{
    AnnotOrTagPagePair pair( annotation, pageNumber );
    if ( !mAnnotsAndTags.contains( pair ) )
      mAnnotsAndTags.append( pair );
}

void AnnotationPopup::addTagging( Okular::Tagging* tagging, int pageNumber )
{
    AnnotOrTagPagePair pair( tagging, pageNumber );
    if ( !mAnnotsAndTags.contains( pair ) )
      mAnnotsAndTags.append( pair );
}

void AnnotationPopup::exec( const QPoint &point )
{
    exec( 0, point );
}

void AnnotationPopup::exec( PageView *pageView, const QPoint &point )
{
    if ( mAnnotsAndTags.isEmpty() && mAnnotsAndTags.isEmpty() )
        return;

    QMenu menu( mParent );

    QAction *action = 0;

    const char *actionTypeId = "actionType";

    const QString openId = QStringLiteral( "open" );
    const QString deleteId = QStringLiteral( "delete" );
    const QString deleteAllId = QStringLiteral( "deleteAll" );
    const QString propertiesId = QStringLiteral( "properties" );
    const QString saveId = QStringLiteral( "save" );
    const QString copyId = QStringLiteral( "copy" );

    if ( mMenuMode == SingleAnnotationMode )
    {
        const bool onlyOne = (mAnnotsAndTags.count() == 1);

        const AnnotOrTagPagePair &pair = mAnnotsAndTags.at(0);

        menu.addSection( i18np( "Annotation", "%1 Annotations", mAnnotsAndTags.count() ) );

        action = menu.addAction( QIcon::fromTheme( QStringLiteral("comment") ), i18n( "&Open Pop-up Note" ) );
        action->setData( QVariant::fromValue( pair ) );
        action->setEnabled( onlyOne );
        action->setProperty( actionTypeId, openId );

        action = menu.addAction( QIcon::fromTheme( QStringLiteral("list-remove") ), i18n( "&Delete" ) );
        action->setEnabled( mDocument->isAllowed( Okular::AllowNotes ) );
        action->setProperty( actionTypeId, deleteAllId );

        foreach ( const AnnotOrTagPagePair& pair, mAnnotsAndTags ) if ( pair.annotation )
        {
            if ( !mDocument->canRemovePageAnnotation( pair.annotation ) )
                action->setEnabled( false );
        }

        action = menu.addAction( QIcon::fromTheme( QStringLiteral("configure") ), i18n( "&Properties" ) );
        action->setData( QVariant::fromValue( pair ) );
        action->setEnabled( onlyOne );
        action->setProperty( actionTypeId, propertiesId );

        if ( onlyOne && annotationHasFileAttachment( pair.annotation ) )
        {
            const Okular::EmbeddedFile *embeddedFile = embeddedFileFromAnnotation( pair.annotation );
            if ( embeddedFile )
            {
                const QString saveText = i18nc( "%1 is the name of the file to save", "&Save '%1'...", embeddedFile->name() );

                menu.addSeparator();
                action = menu.addAction( QIcon::fromTheme( QStringLiteral("document-save") ), saveText );
                action->setData( QVariant::fromValue( pair ) );
                action->setProperty( actionTypeId, saveId );
            }
        }
    }
    else
    {
        foreach ( const AnnotOrTagPagePair& pair, mAnnotsAndTags ) if ( pair.annotation )
        {
            menu.addSection( GuiUtils::captionForAnnotation( pair.annotation ) );

            action = menu.addAction( QIcon::fromTheme( QStringLiteral("comment") ), i18n( "&Open Pop-up Note" ) );
            action->setData( QVariant::fromValue( pair ) );
            action->setProperty( actionTypeId, openId );

            action = menu.addAction( QIcon::fromTheme( QStringLiteral("list-remove") ), i18n( "&Delete" ) );
            action->setEnabled( mDocument->isAllowed( Okular::AllowNotes ) &&
                                mDocument->canRemovePageAnnotation( pair.annotation ) );
            action->setData( QVariant::fromValue( pair ) );
            action->setProperty( actionTypeId, deleteId );

            action = menu.addAction( QIcon::fromTheme( QStringLiteral("configure") ), i18n( "&Properties" ) );
            action->setData( QVariant::fromValue( pair ) );
            action->setProperty( actionTypeId, propertiesId );

            if ( annotationHasFileAttachment( pair.annotation ) )
            {
                const Okular::EmbeddedFile *embeddedFile = embeddedFileFromAnnotation( pair.annotation );
                if ( embeddedFile )
                {
                    const QString saveText = i18nc( "%1 is the name of the file to save", "&Save '%1'...", embeddedFile->name() );

                    menu.addSeparator();
                    action = menu.addAction( QIcon::fromTheme( QStringLiteral("document-save") ), saveText );
                    action->setData( QVariant::fromValue( pair ) );
                    action->setProperty( actionTypeId, saveId );
                }
            }
        }
    }

    foreach ( const AnnotOrTagPagePair& pair, mAnnotsAndTags ) if ( pair.tagging )
    {
        // menu.addSection( GuiUtils::captionForTagging( pair.tagging ) );
        menu.addSection( "Tagging" );

        action = menu.addAction( QIcon::fromTheme( QStringLiteral("edit-copy") ), i18n( "&Copy" ) );
        action->setData( QVariant::fromValue( pair ) );
        action->setProperty( actionTypeId, copyId );

        action = menu.addAction( QIcon::fromTheme( QStringLiteral("comment") ), i18n( "&Comment" ) );
        action->setData( QVariant::fromValue( pair ) );
        action->setProperty( actionTypeId, openId );

        action = menu.addAction( QIcon::fromTheme( QStringLiteral("list-remove") ), i18n( "&Delete" ) );
        action->setData( QVariant::fromValue( pair ) );
        action->setProperty( actionTypeId, deleteId );

        action = menu.addAction( QIcon::fromTheme( QStringLiteral("configure") ), i18n( "&Properties" ) );
        action->setData( QVariant::fromValue( pair ) );
        action->setProperty( actionTypeId, propertiesId );
    }

    QAction *choice = menu.exec( point.isNull() ? QCursor::pos() : point );

    // check if the user really selected an action
    if ( choice ) {
        const AnnotOrTagPagePair pair = choice->data().value<AnnotOrTagPagePair>();

        const QString actionType = choice->property( actionTypeId ).toString();
        if ( actionType == openId ) {
            if ( pair.annotation )
                emit openAnnotationWindow( pair.annotation, pair.pageNumber );
            else
                emit openTaggingWindow( pair.tagging, pair.pageNumber );
        } else if( actionType == deleteId ) {
            if ( pair.pageNumber != -1 ) {
                if ( pair.annotation )
                    mDocument->removePageAnnotation( pair.pageNumber, pair.annotation );
                else
                    mDocument->removePageTagging( pair.pageNumber, pair.tagging );
            }
        } else if( actionType == deleteAllId ) {
            Q_FOREACH ( const AnnotOrTagPagePair& pair, mAnnotsAndTags )
            {
                if ( pair.pageNumber != -1 ) {
                    if ( pair.annotation )
                        mDocument->removePageAnnotation( pair.pageNumber, pair.annotation );
                }
            }
        } else if( actionType == propertiesId ) {
            if ( pair.pageNumber != -1 ) {
                if ( pair.annotation ) {
                    AnnotsPropertiesDialog propdialog( mParent, mDocument, pair.pageNumber, pair.annotation );
                    propdialog.exec();
                } else {
                    TaggingsPropertiesDialog propdialog( mParent, mDocument, pair.pageNumber, pair.tagging );
                    propdialog.exec();
                }
            }
        } else if( actionType == saveId ) {
            Okular::EmbeddedFile *embeddedFile = embeddedFileFromAnnotation( pair.annotation );
            GuiUtils::saveEmbeddedFile( embeddedFile, mParent );
        } else if( actionType == copyId ) {
            switch ( pair.tagging->subType() )
            {
                case Okular::Tagging::TBox:
                {
                    const QVector< PageViewItem * > items = pageView->items();

                    QVector< PageViewItem * >::const_iterator iIt = items.constBegin(), iEnd = items.constEnd();
                    for ( ; iIt != iEnd; ++iIt )
                    {
                        PageViewItem * item = *iIt;
                        const Okular::Page *okularPage = item->page();
                        if ( okularPage != pair.tagging->page()
                        ||  !item->isVisible() )
                            continue;

                        QRect tagRect   = pair.tagging->transformedBoundingRectangle().geometry( item->uncroppedWidth(), item->uncroppedHeight() ).translated( item->uncroppedGeometry().topLeft() );
                        QRect itemRect  = item->croppedGeometry();
                        QRect intersect = tagRect.intersect (itemRect);
                        if ( !intersect.isNull() )
                        {
//  This is code to select text in a rectangle
//                             intersect.translate( -item->uncroppedGeometry().topLeft() );
//                             Okular::RegularAreaRect rects;
//                             rects.append( Okular::NormalizedRect( intersect, item->uncroppedWidth(), item->uncroppedHeight() ) );
//                             QString tagText = okularPage->text( &rects, Okular::TextPage::CentralPixelTextAreaInclusionBehaviour );
//                             QClipboard *cb = QApplication::clipboard();
//                             cb->setText( tagText, QClipboard::Clipboard );
//                             if ( cb->supportsSelection() )
//                                 cb->setText( tagText, QClipboard::Selection );
//                             d->messageWindow->display( i18n( "Text (%1 characters) copied to clipboard.", tagText.length() ) );

                            // renders page into a pixmap
                            QPixmap copyPix( tagRect.width(), tagRect.height() );
                            QPainter copyPainter( &copyPix );
                            copyPainter.translate( -tagRect.left(), -tagRect.top() );
                            pageView->drawDocumentOnPainter( tagRect, &copyPainter );
                            copyPainter.end();
                            QClipboard *cb = QApplication::clipboard();
                            cb->setPixmap( copyPix, QClipboard::Clipboard );
                            if ( cb->supportsSelection() )
                                cb->setPixmap( copyPix, QClipboard::Selection );
//                             d->messageWindow->display( i18n( "Image [%1x%2] copied to clipboard.", copyPix.width(), copyPix.height() ) );
                        }
                    }
                    break;
                }
                case Okular::Tagging::TText:
                {
                    Okular::TextTagging * textTagging = static_cast< Okular::TextTagging * >(pair.tagging);
                    QString tagText = textTagging->page()->text( textTagging->transformedTextArea(), Okular::TextPage::CentralPixelTextAreaInclusionBehaviour );
                    QClipboard *cb = QApplication::clipboard();
                    cb->setText( tagText, QClipboard::Clipboard );
                    if ( cb->supportsSelection() )
                        cb->setText( tagText, QClipboard::Selection );
                    break;
                }
            }
        }
    }
}

#include "moc_annotationpopup.cpp"
