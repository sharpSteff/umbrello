/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "umlattributedialog.h"
#include "../attribute.h"
#include "../interface.h"
#include "../concept.h"
#include "../umldoc.h"
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <qlayout.h>

UMLAttributeDialog::UMLAttributeDialog( QWidget * pParent, UMLAttribute * pAttribute ) : KDialogBase( Plain, i18n("Attribute Properties"), Help | Ok | Cancel , Ok, pParent, "_UMLATTRIBUTEDLG_", true, true) {
	m_pAttribute = pAttribute;
	setupDialog();
}

UMLAttributeDialog::~UMLAttributeDialog() {}

void UMLAttributeDialog::setupDialog() {
	UMLConcept * pConcept = dynamic_cast<UMLConcept *>( m_pAttribute -> parent() );
	UMLDoc * pDoc = dynamic_cast<UMLDoc *>( pConcept -> parent() );
	int margin = fontMetrics().height();

	QVBoxLayout * mainLayout = new QVBoxLayout( plainPage() );

	m_pValuesGB = new QGroupBox(i18n("General Properties"), plainPage() );
	QGridLayout * valuesLayout = new QGridLayout(m_pValuesGB, 4, 2);
	valuesLayout -> setMargin(margin);
	valuesLayout -> setSpacing(10);

	m_pTypeL = new QLabel(i18n("Type:"), m_pValuesGB);
	valuesLayout -> addWidget(m_pTypeL, 0, 0);

	m_pNameL = new QLabel(i18n("Name:"), m_pValuesGB);
	valuesLayout -> addWidget(m_pNameL, 1, 0);

	m_pInitialL = new QLabel(i18n("Initial value:"), m_pValuesGB);
	valuesLayout -> addWidget(m_pInitialL, 2, 0);

	m_pTypeCB = new QComboBox(m_pValuesGB);
	valuesLayout -> addWidget(m_pTypeCB, 0, 1);

	m_pNameLE = new QLineEdit(m_pValuesGB);
	valuesLayout -> addWidget(m_pNameLE, 1, 1);
	m_pNameLE -> setText( m_pAttribute -> getName() );

	m_pInitialLE = new QLineEdit(m_pValuesGB);
	valuesLayout -> addWidget(m_pInitialLE, 2, 1);

	m_pStaticCB = new QCheckBox( i18n("Classifier &scope (\"static\")"), m_pValuesGB );
	m_pStaticCB -> setChecked( m_pAttribute -> getStatic() );
	valuesLayout -> addWidget(m_pStaticCB, 3, 0);

	mainLayout -> addWidget(m_pValuesGB);
	m_pInitialLE -> setText( m_pAttribute -> getInitialValue() );


	m_pScopeBG = new QButtonGroup(i18n("Visibility"), plainPage() );
	QHBoxLayout * scopeLayout = new QHBoxLayout(m_pScopeBG);
	scopeLayout -> setMargin(margin);

	m_pPublicRB = new QRadioButton(i18n("&Public"), m_pScopeBG);
	scopeLayout -> addWidget(m_pPublicRB);

	m_pPrivateRB = new QRadioButton(i18n("P&rivate"), m_pScopeBG);
	scopeLayout -> addWidget(m_pPrivateRB);

	m_pProtectedRB = new QRadioButton(i18n("Pro&tected"), m_pScopeBG);
	scopeLayout -> addWidget(m_pProtectedRB);
	mainLayout -> addWidget(m_pScopeBG);
	Uml::Scope scope = m_pAttribute -> getScope();
	if( scope == Uml::Public )
		m_pPublicRB -> setChecked( true );
	else if( scope == Uml::Private )
		m_pPrivateRB -> setChecked( true );
	else
		m_pProtectedRB -> setChecked( true );

	//add some standard attribute types to combo box
	QString types[] = {
	                      i18n("int"), i18n("long"), i18n("bool"), i18n("string"),
			      i18n("double"), i18n("float"), i18n("date")
	                  };

	for (int i=0; i<7; i++) {
		m_pTypeCB->insertItem(types[i]);
	}

	m_pTypeCB->setEditable(true);
	m_pTypeCB->setDuplicatesEnabled(false);//only allow one of each type in box
	m_pTypeCB->setAutoCompletion(true);

	//now add the Concepts
	QList<UMLConcept> namesList( pDoc->getConcepts() );
	UMLConcept* obj;
	for (obj=namesList.first(); obj!=0; obj=namesList.next()) {
		m_pTypeCB->insertItem( obj->getName() );
	}
	QPtrList<UMLInterface> interfaceList( pDoc->getInterfaces() );
	UMLInterface* pInterface = 0;
	for(pInterface=interfaceList.first(); pInterface!=0 ;pInterface=interfaceList.next()) {
		m_pTypeCB->insertItem( pInterface->getName() );
	}

	//work out which one to select
	int typeBoxCount = 0;
	bool foundType = false;
	while (typeBoxCount < m_pTypeCB->count() && foundType == false) {
		QString typeBoxString = m_pTypeCB->text(typeBoxCount);
		if ( typeBoxString == m_pAttribute->getTypeName() ) {
			foundType = true;
			m_pTypeCB->setCurrentItem(typeBoxCount);
		} else {
			typeBoxCount++;
		}
	}

	if (!foundType) {
		m_pTypeCB->insertItem( m_pAttribute->getTypeName(), 0 );
		m_pTypeCB->setCurrentItem(0);
	}

	m_pNameLE->setFocus();
}

bool UMLAttributeDialog::apply() {
	UMLConcept * pConcept = dynamic_cast<UMLConcept *>( m_pAttribute->parent() );

	if ( m_pPublicRB->isChecked() ) {
		m_pAttribute->setScope(Uml::Public);
	} else if ( m_pPrivateRB -> isChecked() ) {
		m_pAttribute->setScope(Uml::Private);
	} else {
		m_pAttribute->setScope(Uml::Protected);
	}

	m_pAttribute->setTypeName( m_pTypeCB->currentText() );
	m_pAttribute->setInitialValue( m_pInitialLE->text() );
	QString name = m_pNameLE->text();
	if( name.length() == 0 ) {
		KMessageBox::error(this, i18n("You have entered an invalid attribute name."),
		                   i18n("Attribute Name Invalid"), false);
		m_pNameLE->setText( m_pAttribute->getName() );
		return false;
	}

	QPtrList<UMLObject> list= pConcept->findChildObject(Uml::ot_Attribute, name);
	if( list.count() != 0 && list.findRef( m_pAttribute ) ) {
		KMessageBox::error(this, i18n("The attribute name you have chosen is already being used in this operation."),
		                   i18n("Attribute Name Not Unique"), false);
		m_pNameLE->setText( m_pAttribute->getName() );
		return false;
	}
	m_pAttribute->setName(name);
	m_pAttribute->setStatic( m_pStaticCB->isChecked() );
	return true;
}

void UMLAttributeDialog::slotApply() {
	apply();
}

void UMLAttributeDialog::slotOk() {
	if ( apply() ) {
		accept();
	}
}



#include "umlattributedialog.moc"
