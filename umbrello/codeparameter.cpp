
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*  This code generated by: 
 *      Author : thomas
 *      Date   : Fri Jun 20 2003
 */

#include <kdebug.h>
#include "codeparameter.h"
#include "association.h"
#include "codegenerator.h"
#include "classifiercodedocument.h"
#include "umldoc.h"
#include "umlobject.h"

// Constructors/Destructors
//  

CodeParameter::CodeParameter ( ClassifierCodeDocument * parentDoc, UMLObject * parentObject )
    : QObject ( (QObject*) parentObject, "ACodeParam") 
{
	initFields( parentDoc, parentObject );
}

CodeParameter::~CodeParameter ( ) { }

//  
// Methods
//  


// Accessor methods
//  

/**
 * Utility method to get the value of parent object abstract value
 * @return the value of parent object abstrtact
 */
bool CodeParameter::getAbstract ( ) {
        return m_parentObject->getAbstract();
}

/**
 * Utility method to get the value of parent object static
 * Whether or not this is static.
 * @return the value of static 
 */
bool CodeParameter::getStatic ( ) {
        return m_parentObject->getStatic();
}

/**
 * Utility method to get the value of parent object name 
 * The name of this code parameter.
 * @return the value 
 */
QString CodeParameter::getName ( ) const {
	return m_parentObject->getName();
}

/**
 * Utility method to get the value of parent object type.
 * the typeName of this parameters (e.g. boolean, int, etc or perhaps Class name of
 * an object)
 * @return the value of type
 */
QString CodeParameter::getTypeName ( ) {
	UMLAttribute * at = (UMLAttribute*) m_parentObject;
	return at->getTypeName();
}

/**
 * Utility method to get the value of parent object scope. 
 * The visibility of this code parameter.
 * @return the value of parent object scope
 */
Uml::Scope CodeParameter::getVisibility ( ) const {
	return m_parentObject->getScope();
}

/**
 * Set the value of m_initialValue
 * The initial value of this code parameter
 * @param new_var the new value of m_initialValue
 */
void CodeParameter::setInitialValue ( QString new_var ) {
	m_initialValue = new_var;
}

/**
 * Get the value of m_initialValue
 * The initial value of this code parameter
 * @return the value of m_initialValue
 */
QString CodeParameter::getInitialValue ( ) {
	return m_initialValue;
}

/**
 * Set a Comment object.
 */
void CodeParameter::setComment ( CodeComment * object ) {
	m_comment = object;
}

/**
 * Get the Comment on this object.
 */
CodeComment * CodeParameter::getComment ( ) {
	return m_comment;
}


ClassifierCodeDocument * CodeParameter::getParentDocument ( ) {
        return m_parentDocument;
}

/**
 * Get the ParentObject object
 */
UMLObject * CodeParameter::getParentObject ( ) {
        return m_parentObject;
}

CodeGenerator * CodeParameter::getParentGenerator ( ) {
        return getParentDocument()->getParentGenerator();
}

// need to get the ID of the parent object
// this is kind of broken for UMLRoles.
QString CodeParameter::getID () {
	UMLRole * role = dynamic_cast<UMLRole*>(m_parentObject);
	if(role)
	{
		// cant use Role "ID" as that is used to distinquish if its
		// role "A" or "B"
		UMLAssociation *assoc = role->getParentAssociation();
        	return QString::number(assoc->getID());
	} else
        	return QString::number(getParentObject()->getID());
}

// Other methods
//  

void CodeParameter::setAttributesOnNode ( QDomDocument & doc, QDomElement & blockElement)
{


        // set local attributes 
	blockElement.setAttribute("parent_id",getID());

	// setting ID's takes special treatment
	// as UMLRoles arent properly stored in the XMI right now.
	// (change would break the XMI format..save for big version change )
	UMLRole * role = dynamic_cast<UMLRole*>(m_parentObject);
	if(role) 
		blockElement.setAttribute("role_id",role->getRoleID());
	else 
		blockElement.setAttribute("role_id","-1");

	blockElement.setAttribute("initialValue",getInitialValue());

        // a comment which we will store in its own separate child node block
        QDomElement commElement = doc.createElement( "header" );
        getComment()->saveToXMI(doc, commElement); // comment
        blockElement.appendChild( commElement);

}

/** set the class attributes of this object from
 * the passed element node.
 */
void CodeParameter::setAttributesFromNode ( QDomElement & root) {

        // set local attributes, parent object first
	int id = root.attribute("parent_id","-1").toInt();

	// always disconnect
	m_parentObject->disconnect(this);

	// now, what is the new object we want to set?
	UMLObject * obj = m_parentDocument->getParentGenerator()->getDocument()->findUMLObject(id);
	if(obj)
	{

		// FIX..one day.
		// Ugh. This is UGLY, but we have to do it this way because UMLRoles
		// dont go into the document list of UMLobjects, and have the same
		// ID as their parent UMLAssociations. So..the drill is then special
		// for Associations..in that case we need to find out which role will
		// serve as the parametger here. The REAL fix, of course, would be to
		// treat UMLRoles on a more even footing, but im not sure how that change
		// might ripple throughout the code and cause problems. Thus, since the
		// change appears to be needed for only this part, I'll do this crappy
		// change instead. -b.t.
		UMLAssociation * assoc = dynamic_cast<UMLAssociation*>(obj);
		if(assoc) {
			// In this case we init with indicated role child obj. 
			UMLRole * role = 0;
			int role_id = root.attribute("role_id","-1").toInt();
			// deal with OLD save files (make go away some time soon)
			if (role_id > 1) {
				if(assoc->getUMLRoleA()->getID() == role_id)
					role = assoc->getUMLRoleA();
				else if(assoc->getUMLRoleB()->getID() == role_id)
					role = assoc->getUMLRoleB();
				else // this will cause a crash
				kdError()<<"ERROR! corrupt save file? cant get proper UMLRole for codeparameter:"<<id<<" w/role_id:"<<role_id<<endl;

			} else {

                                if(role_id == 0) 
                                        role = assoc->getUMLRoleA();
                                if(role_id == 1) 
                                        role = assoc->getUMLRoleB();
                                else 
					kdError()<<"ERROR! corrupt save file? cant get proper UMLRole for codeparameter:"<<id<<" w/role_id:"<<role_id<<endl;
			}

			// init using UMLRole obj
			initFields ( m_parentDocument, role); 

		} else
			initFields ( m_parentDocument, obj); // just the regular approach 

	} else
		kdError()<<"CANT LOAD CODE PARARMETER: parentUMLObject w/id:"<<id<<" not found, corrupt save file?"<<endl;

	// other attribs now
	setInitialValue(root.attribute("initialValue",""));

        // load comment now
        // by looking for our particular child element
        QDomNode node = root.firstChild();
        QDomElement element = node.toElement();
        bool gotComment = false;
        while( !element.isNull() ) {
                QString tag = element.tagName();
                if( tag == "header" ) {
                        QDomNode cnode = element.firstChild();
                        QDomElement celem = cnode.toElement();
                        getComment()->loadFromXMI(celem);
                        gotComment = true;
                        break;
                }
                node = element.nextSibling();
                element = node.toElement();
        }

        if(!gotComment)
                kdWarning()<<" loadFromXMI : Warning: unable to initialize CodeComment in codeparam:"<<this<<endl;


}

/**
 * create the string representation of this code parameter.
 * @return	QString
 */
void CodeParameter::syncToParent( ) {

	getComment()->setText(getParentObject()->getDoc());

        updateContent();
}

void CodeParameter::initFields ( ClassifierCodeDocument * doc, UMLObject * obj) {

	m_parentObject = obj;

	m_parentDocument = doc;
	m_initialValue = QString("");

	m_comment = m_parentDocument->newCodeComment();
	m_comment->setText(getParentObject()->getDoc());

	connect(m_parentObject,SIGNAL(modified()),this,SLOT(syncToParent()));
}

#include "codeparameter.moc"
