/***************************************************************************
 *   Copyright (C) 2016 by Jonathan Schultz <jonathan@imatix.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _TAGGINGPROPERTIESDIALOG_H_
#define _TAGGINGPROPERTIESDIALOG_H_

#include <kpagedialog.h>

class QLabel;
class QLineEdit;
class TaggingWidget;

namespace Okular {
class Tagging;
class Document;
}

class TaggingsPropertiesDialog : public KPageDialog
{
    Q_OBJECT
public:
    TaggingsPropertiesDialog( QWidget *parent, Okular::Document *document, int docpage, Okular::Tagging *ann );
    ~TaggingsPropertiesDialog();

private:
    Okular::Document *m_document;
    int m_page;
    bool modified;
    Okular::Tagging* m_tagging;    //source tagging
    //dialog widgets:
    QLineEdit *AuthorEdit;
    TaggingWidget *m_taggingWidget;
    QLabel *m_modifyDateLabel;

    void setCaptionTextbyTaggingType();

private slots:
    void setModified();
    void slotapply();
};


#endif
