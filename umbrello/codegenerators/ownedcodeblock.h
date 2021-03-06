/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2003      Brian Thomas <thomas@mail630.gsfc.nasa.gov>   *
 *   copyright (C) 2004-2020                                               *
 *   Umbrello UML Modeller Authors <umbrello-devel@kde.org>                *
 ***************************************************************************/

#ifndef OWNEDCODEBLOCK_H
#define OWNEDCODEBLOCK_H

#include <QDomDocument>
#include <QDomElement>
#include <QObject>

class TextBlock;
class UMLObject;

/**
 * Describes any codeblock which is 'owned' by a UMLobject of some sort and should
 * be in sync with that parent.
 */
class OwnedCodeBlock : public QObject
{
    Q_OBJECT
public:

    explicit OwnedCodeBlock (UMLObject * parent);
    virtual ~OwnedCodeBlock ();

    UMLObject * getParentObject ();

    virtual void updateContent () = 0;

protected:

    virtual void release ();

    virtual void setAttributesOnNode (QDomDocument & doc, QDomElement & blockElement);
    virtual void setAttributesFromNode (QDomElement & element);
    virtual void setAttributesFromObject (TextBlock * obj);

private:

    void initFields (UMLObject * parent);

    UMLObject * m_parentObject;

public slots:

    virtual void syncToParent ();

};

#endif // OWNEDCODEBLOCK_H
