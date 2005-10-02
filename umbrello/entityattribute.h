/*
 *  copyright (C) 2002-2004
 *  Umbrello UML Modeller Authors <uml-devel@ uml.sf.net>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ENTITYATTRIBUTE_H
#define ENTITYATTRIBUTE_H

#include "classifierlistitem.h"
#include "umlnamespace.h"

/**
 * This class is used to set up information for an entityattribute.  This is a database field
 * It has a type, name, index type and default value.
 *
 * @short Sets up entityattribute information.
 * @author Jonathan Riddell <jr @jriddell.org>
 * @see UMLObject
 * Bugs and comments to uml-devel@lists.sf.net or http://bugs.kde.org
 */
class UMLEntityAttribute : public UMLClassifierListItem {
public:
    /**
     * Sets up an entityattribute.
     *
     * @param parent    The parent of this UMLEntityAttribute.
     * @param name      The name of this UMLEntityAttribute.
     * @param id        The unique id given to this UMLEntityAttribute.
     * @param s         The visibility of the UMLEntityAttribute.
     * @param type      The type of this UMLEntityAttribute.
     * @param iv        The initial value of the entityattribute.
     */
    UMLEntityAttribute(const UMLObject* parent, QString name, Uml::IDType id = Uml::id_None,
               Uml::Visibility s = Uml::Visibility::Private, QString type = "int", QString iv = 0);

    /**
     * Sets up an entityattribute.
     *
     * @param parent    The parent of this UMLEntityAttribute.
     */
    UMLEntityAttribute(const UMLObject* parent);

    /**
     * Overloaded '==' operator
     */
    bool operator==( UMLEntityAttribute& rhs);

    /**
     * destructor.
     */
    virtual ~UMLEntityAttribute();

    /**
     * Copy the internal presentation of this object into the UMLEntityAttribute
     * object.
     */
    virtual void copyInto(UMLEntityAttribute* rhs) const;

    /**
     * Make a clone of the UMLEntityAttribute.
     */
    virtual UMLObject* clone() const;

    /**
     * Returns The initial value of the UMLEntityAttribute.
     *
     * @return  The inital value of the Atrtibute.
     */
    QString getInitialValue();

    /**
     * Sets the initial value of the UMLEntityAttribute.
     *
     * @param iv                The inital value of the UMLEntityAttribute.
     */
    void setInitialValue( const QString& iv );

    /**
     * Returns The value of the UMLEntityAttribute's attributes property.
     *
     * @return  The value of the UMLEntityAttribute's attributes property.
     */
    QString getAttributes();

    /**
     * Sets the initial value of the UMLEntityAttribute's attributes property.
     *
     * @param iv  The inital value of the UMLEntityAttribute's attributes property.
     */
    void setAttributes(const QString& attributes);

    /**
     * Returns The initial value of the UMLEntityAttribute's index type property.
     *
     * @return  The inital value of the UMLEntityAttribute's index type property.
     */
    Uml::DBIndex_Type getIndexType();

    /**
     * Sets the initial value of the UMLEntityAttribute's index type property.
     *
     * @param iv        The inital value of the UMLEntityAttribute's index type property.
     */
    void setIndexType(const Uml::DBIndex_Type indexType);

    /**
     * Returns The initial value of the UMLEntityAttribute's length/values property.
     *
     * @return  The inital value of the UMLEntityAttribute's length/values property.
     */
    QString getValues();

    /**
     * Sets the initial value of the UMLEntityAttribute's length/values property.
     *
     * @param iv        The inital value of the UMLEntityAttribute's length/values property.
     */
    void setValues(const QString& values);

    /**
     * Returns The initial value of the UMLEntityAttribute's auto_increment boolean
     *
     * @return  The inital value of the UMLEntityAttribute's auto_increment boolean
     */
    bool getAutoIncrement();

    /**
     * Sets the initial value of the UMLEntityAttribute's auto_increment boolean
     *
     * @param iv        The inital value of the UMLEntityAttribute's auto_increment boolean
     */
    void setAutoIncrement(const bool autoIncrement);

    /**
     * Returns The initial value of the UMLEntityAttribute's allow null value.
     *
     * @return  The inital value of the UMLEntityAttribute's allow null value.
     */
    bool getNull();

    /**
     * Sets the initial value of the UMLEntityAttribute's allow null value.
     *
     * @param iv        The inital value of the UMLEntityAttribute's allow null value.
     */
    void setNull(const bool null);

    /**
     * Returns a string representation of the UMLEntityAttribute.
     *
     * @param sig               If true will show the entityattribute type and
     *                  inital value.
     * @return  Returns a string representation of the UMLEntityAttribute.
     */
    QString toString(Uml::Signature_Type sig = Uml::st_NoSig);

    /**
     * Creates the <UML:EntityAttribute> XMI element.
     */
    void saveToXMI(QDomDocument& qDoc, QDomElement& qElement);

    /**
     * Display the properties configuration dialogue for the entityattribute.
     */
    bool showPropertiesDialogue(QWidget* parent);

    void setParmKind (Uml::Parameter_Direction pk);
    Uml::Parameter_Direction getParmKind () const;

protected:
    /**
     * Loads the <UML:EntityAttribute> XMI element.
     */
    bool load(QDomElement& element);

private:
    QString m_InitialValue; ///< text for the entityattribute's initial value.
    Uml::Parameter_Direction m_ParmKind;
    Uml::DBIndex_Type m_indexType;
    QString m_values;
    QString m_attributes;
    bool m_autoIncrement;
    bool m_null;
};

#endif

