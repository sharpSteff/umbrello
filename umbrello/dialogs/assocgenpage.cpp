/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2003-2013                                               *
 *   Umbrello UML Modeller Authors <umbrello-devel@kde.org>                *
 ***************************************************************************/

// own header
#include "assocgenpage.h"

// local includes
#include "associationwidget.h"
#include "assocrules.h"
#include "debug_utils.h"
#include "dialog_utils.h"
#include "objectwidget.h"
#include "umldoc.h"
#include "umlobject.h"

// kde includes
#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktextedit.h>

// qt includes
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QVBoxLayout>

/**
 *  Sets up the AssocGenPage.
 *
 *  @param  d       The UMLDoc which controls controls object creation.
 *  @param  parent  The parent to the AssocGenPage.
 *  @param  a       The AssociationWidget to display the properties of.
 */
AssocGenPage::AssocGenPage (UMLDoc *d, QWidget *parent, AssociationWidget *assoc)
  : QWidget(parent),
    m_pAssocNameLE(0),
    m_pTypeCB(0),
    m_pAssociationWidget(assoc),
    m_pUmldoc(d),
    m_pWidget(0)
{
    constructWidget();
}

/**
 *  Standard destructor.
 */
AssocGenPage::~AssocGenPage()
{
}

/**
 * Construct all the widgets for this dialog.
 */
void AssocGenPage::constructWidget()
{
    // general configuration of the GUI
    int margin = fontMetrics().height();
    setMinimumSize(310, 330);
    QVBoxLayout * topLayout = new QVBoxLayout(this);
    topLayout->setSpacing(6);

    // group boxes for name, documentation properties
    QGroupBox *nameGB = new QGroupBox(this);
    QGroupBox *docGB = new QGroupBox(this);
    nameGB->setTitle(i18n("Properties"));
    docGB->setTitle(i18n("Documentation"));
    topLayout->addWidget(nameGB);
    topLayout->addWidget(docGB);

    QGridLayout * nameLayout = new QGridLayout(nameGB);
    nameLayout->setSpacing(6);
    nameLayout->setMargin(margin);

    //Association name
    QLabel *pAssocNameL = NULL;
    KLineEdit* nameField = Dialog_Utils::makeLabeledEditField(nameGB, nameLayout, 0,
                           pAssocNameL, i18nc("name of association widget", "Name:"),
                           m_pAssocNameLE, m_pAssociationWidget->name());
    nameField->setFocus();

    // document
    QHBoxLayout * docLayout = new QHBoxLayout(docGB);
    docLayout->setMargin(margin);

    m_doc = new KTextEdit(docGB);
    docLayout->addWidget(m_doc);
    m_doc->setText(m_pAssociationWidget->documentation());
    Uml::AssociationType::Enum currentType =  m_pAssociationWidget->associationType();
    QString currentTypeAsString = Uml::AssociationType::toStringI18n(currentType);
    QLabel *pTypeL = new QLabel(i18n("Type:"), nameGB);
    nameLayout->addWidget(pTypeL, 1, 0);

    // Here is a list of all the supported choices for changing
    // association types.

    m_AssocTypes.clear();

    m_AssocTypes << currentType;
    uDebug() << "current type = " << Uml::AssociationType::toString(currentType);

    // dynamically load all allowed associations
    for (int i = Uml::AssociationType::Generalization; i < Uml::AssociationType::Reserved;  ++i) {
        // we don't need to check for current type
        Uml::AssociationType::Enum assocType = Uml::AssociationType::fromInt(i);
        if (assocType == currentType)
            continue;

        // UMLScene based checks
        if (m_pAssociationWidget->umlScene()->type() == Uml::DiagramType::Collaboration
                && !(assocType == Uml::AssociationType::Coll_Message_Asynchronous
                    || assocType == Uml::AssociationType::Coll_Message_Synchronous
                    || assocType == Uml::AssociationType::Anchor))
            continue;

        if (AssocRules::allowAssociation(assocType,
                                         m_pAssociationWidget->widgetForRole(Uml::RoleType::A),
                                         m_pAssociationWidget->widgetForRole(Uml::RoleType::B))) {
            m_AssocTypes << assocType;
            uDebug() << "to type list = " << Uml::AssociationType::toString(assocType);
        }
    }

    bool found = false;
    m_AssocTypeStrings.clear();
    for (int i = 0; i < m_AssocTypes.size(); ++i) {
        if (m_AssocTypes[i] == currentType) {
            found = true;
        }
        m_AssocTypeStrings << Uml::AssociationType::toStringI18n(m_AssocTypes[i]);
    }

    if (!found) {
        m_AssocTypes.clear();
        m_AssocTypes << currentType;
        m_AssocTypeStrings.clear();
        m_AssocTypeStrings << currentTypeAsString;
    }

    m_pTypeCB = new KComboBox(nameGB);
    pTypeL->setBuddy(m_pTypeCB);
    m_pTypeCB->addItems(m_AssocTypeStrings);
    m_pTypeCB->setCompletedItems(m_AssocTypeStrings);

    m_pTypeCB->setDuplicatesEnabled(false); // only allow one of each type in box
    m_pTypeCB->setCompletionMode(KGlobalSettings::CompletionPopup);
    m_doc->setWordWrapMode(QTextOption::WordWrap);
    nameLayout->addWidget(m_pTypeCB, 1, 1);
}

/**
 *  Will move information from the dialog into the object.
 *  Call when the ok or apply button is pressed.
 */
void AssocGenPage::updateObject()
{
    if (m_pAssociationWidget) {
        int comboBoxItem = m_pTypeCB->currentIndex();
        Uml::AssociationType::Enum newType = m_AssocTypes[comboBoxItem];
        m_pAssociationWidget->setAssociationType(newType);
        m_pAssociationWidget->setName(m_pAssocNameLE->text());
        m_pAssociationWidget->setDocumentation(m_doc->toPlainText());
    }
}

#include "assocgenpage.moc"
