/***************************************************************************
 *   Copyright (C) 2016 by Jonathan Schultz <jonathan@imatix.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "taggingpropertiesdialog.h"

// qt/kde includes
#include <qframe.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qheaderview.h>
#include <QtWidgets/qpushbutton.h>
#include <qtextedit.h>
#include <QIcon>
#include <klineedit.h>
#include <KLocalizedString>

// local includes
#include "core/document.h"
#include "core/page.h"
#include "core/tagging.h"
#include "taggingwidgets.h"


TaggingsPropertiesDialog::TaggingsPropertiesDialog( QWidget *parent, Okular::Document *document, int docpage, Okular::Tagging *tag )
    : KPageDialog( parent ), m_document( document ), m_page( docpage ), modified( false )
{
    setFaceType( Tabbed );
    m_tagging=tag;
    const bool canEditTaggings = m_document->canModifyPageTagging( tag );
    setCaptionTextbyTaggingType();
    if ( canEditTaggings )
    {
        setStandardButtons( QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel );
        button( QDialogButtonBox::Apply )->setEnabled( false );
        connect( button( QDialogButtonBox::Apply ), &QPushButton::clicked, this, &TaggingsPropertiesDialog::slotapply);
        connect( button( QDialogButtonBox::Ok ), &QPushButton::clicked, this, &TaggingsPropertiesDialog::slotapply);
    }
    else
    {
        setStandardButtons( QDialogButtonBox::Close );
        button( QDialogButtonBox::Close )->setDefault( true );
    }

    m_taggingWidget = TaggingWidgetFactory::widgetFor( tag );

    QLabel* tmplabel;
  //1. Appearance
    //BEGIN tab1
//     QWidget *appearanceWidget = m_taggingWidget->appearanceWidget();
//     appearanceWidget->setEnabled( canEditTaggings );
//     addPage( appearanceWidget, i18n( "&Appearance" ) );
    //END tab1

    //BEGIN tab 2
    QFrame* page = new QFrame( this );
    addPage( page, i18n( "&General" ) );
//    m_tabitem[1]->setIcon( QIcon::fromTheme( "fonts" ) );
    QGridLayout* gridlayout = new QGridLayout( page );
    tmplabel = new QLabel( i18n( "&Author:" ), page );
    AuthorEdit = new KLineEdit( tag->author(), page );
    AuthorEdit->setEnabled( canEditTaggings );
    tmplabel->setBuddy( AuthorEdit );
    gridlayout->addWidget( tmplabel, 0, 0, Qt::AlignRight );
    gridlayout->addWidget( AuthorEdit, 0, 1 );

    tmplabel = new QLabel( page );
    tmplabel->setText( i18n( "Created: %1", QLocale().toString( tag->creationDate(), QLocale::LongFormat ) ) );
    tmplabel->setTextInteractionFlags( Qt::TextSelectableByMouse );
    gridlayout->addWidget( tmplabel, 1, 0, 1, 2 );

    m_modifyDateLabel = new QLabel( page );
    m_modifyDateLabel->setText( i18n( "Modified: %1", QLocale().toString( tag->modificationDate(), QLocale::LongFormat ) ) );
    m_modifyDateLabel->setTextInteractionFlags( Qt::TextSelectableByMouse );
    gridlayout->addWidget( m_modifyDateLabel, 2, 0, 1, 2 );

    gridlayout->addItem( new QSpacerItem( 5, 5, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ), 3, 0 );
    //END tab 2

    QWidget * extraWidget = m_taggingWidget->extraWidget();
    if ( extraWidget )
    {
        addPage( extraWidget, extraWidget->windowTitle() );
    }

    //BEGIN connections
    connect(AuthorEdit, &QLineEdit::textChanged, this, &TaggingsPropertiesDialog::setModified);
    connect(m_taggingWidget, &TaggingWidget::dataChanged, this, &TaggingsPropertiesDialog::setModified);
    //END

#if 0
    qCDebug(OkularUiDebug) << "Tagging details:";
    qCDebug(OkularUiDebug).nospace() << " => unique name: '" << tag->uniqueName() << "'";
    qCDebug(OkularUiDebug) << " => flags:" << QString::number( m_tagging->flags(), 2 );
#endif

    resize( sizeHint() );
}
TaggingsPropertiesDialog::~TaggingsPropertiesDialog()
{
    delete m_taggingWidget;
}


void TaggingsPropertiesDialog::setCaptionTextbyTaggingType()
{
    Okular::Tagging::SubType type=m_tagging->subType();
    QString captiontext;
    switch(type)
    {
        case Okular::Tagging::TText:
            captiontext = i18n( "Text Tagging Properties" );
            break;
        case Okular::Tagging::TBox:
            captiontext = i18n( "Box Tagging Properties" );
            break;
        default:
            captiontext = i18n( "Tagging Properties" );
            break;
    }
    setWindowTitle( captiontext );
}

void TaggingsPropertiesDialog::setModified()
{
    modified = true;
    button( QDialogButtonBox::Apply )->setEnabled( true );
}

void TaggingsPropertiesDialog::slotapply()
{
    if ( !modified )
        return;

    m_document->prepareToModifyTaggingProperties( m_tagging );
    m_tagging->setAuthor( AuthorEdit->text() );
    m_tagging->setModificationDate( QDateTime::currentDateTime() );

    m_taggingWidget->applyChanges();

    m_document->modifyPageTaggingProperties( m_page, m_tagging );

    m_modifyDateLabel->setText( i18n( "Modified: %1", QLocale().toString( m_tagging->modificationDate(), QLocale::LongFormat ) ) );

    modified = false;
    button( QDialogButtonBox::Apply )->setEnabled( false );
}

#include "moc_taggingpropertiesdialog.cpp"

