/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2006-2014                                               *
 *   Umbrello UML Modeller Authors <umbrello-devel@kde.org>                *
 ***************************************************************************/

// own header
#include "widget_factory.h"

// app includes
#include "activitywidget.h"
#include "actor.h"
#include "actorwidget.h"
#include "artifact.h"
#include "artifactwidget.h"
#include "associationwidget.h"
#include "boxwidget.h"
#include "category.h"
#include "categorywidget.h"
#include "classifier.h"
#include "classifierwidget.h"
#include "cmds.h"
#include "combinedfragmentwidget.h"
#include "component.h"
#include "componentwidget.h"
#include "datatypewidget.h"
#include "debug_utils.h"
#include "entity.h"
#include "entitywidget.h"
#include "enum.h"
#include "enumwidget.h"
#include "floatingdashlinewidget.h"
#include "floatingtextwidget.h"
#include "folder.h"
#include "forkjoinwidget.h"
#include "messagewidget.h"
#include "node.h"
#include "nodewidget.h"
#include "notewidget.h"
#include "object_factory.h"
#include "objectnodewidget.h"
#include "objectwidget.h"
#include "package.h"
#include "packagewidget.h"
#include "pinwidget.h"
#include "port.h"
#include "portwidget.h"
#include "preconditionwidget.h"
#include "regionwidget.h"
#include "signalwidget.h"
#include "statewidget.h"
#include "uml.h"
#include "umldoc.h"
#include "umlscene.h"
#include "umlview.h"
#include "usecase.h"
#include "usecasewidget.h"

namespace Widget_Factory {

/**
 * Create a UMLWidget in the given view and representing the given document object.
 */
UMLWidget *createWidget(UMLScene *scene, UMLObject *o)
{
    QPointF pos = scene->pos();
    int y = pos.y();
    Uml::DiagramType::Enum diagramType = scene->type();
    UMLObject::ObjectType type = o->baseType();
    UMLWidget *newWidget = NULL;
    switch (type) {
    case UMLObject::ot_Actor:
        if (diagramType == Uml::DiagramType::Sequence) {
            ObjectWidget *ow = new ObjectWidget(scene, o);
            ow->setDrawAsActor(true);
            y = ow->topMargin();
            newWidget = ow;
        } else
            newWidget = new ActorWidget(scene, static_cast<UMLActor*>(o));
        break;
    case UMLObject::ot_UseCase:
        newWidget = new UseCaseWidget(scene, static_cast<UMLUseCase*>(o));
        break;
    case UMLObject::ot_Folder:
    case UMLObject::ot_Package:
        newWidget = new ClassifierWidget(scene, static_cast<UMLPackage*>(o));
        break;
    case UMLObject::ot_Component:
        newWidget = new ComponentWidget(scene, static_cast<UMLComponent*>(o));
        if (diagramType == Uml::DiagramType::Deployment) {
            newWidget->setIsInstance(true);
        }
        break;
    case UMLObject::ot_Port:
        {
            PinPortBase *pw = new PortWidget(scene, static_cast<UMLPort*>(o));
            pw->attachToOwner();
            newWidget = pw;
        }
        break;
    case UMLObject::ot_Node:
        newWidget = new NodeWidget(scene, static_cast<UMLNode*>(o));
        break;
    case UMLObject::ot_Artifact:
        newWidget = new ArtifactWidget(scene, static_cast<UMLArtifact*>(o));
        break;
    case UMLObject::ot_Datatype:
        newWidget = new DatatypeWidget(scene, static_cast<UMLClassifier*>(o));
        break;
    case UMLObject::ot_Enum:
        newWidget = new EnumWidget(scene, static_cast<UMLEnum*>(o));
        break;
    case UMLObject::ot_Entity:
        newWidget = new EntityWidget(scene, static_cast<UMLEntity*>(o));
        break;
    case UMLObject::ot_Interface:
        if (diagramType == Uml::DiagramType::Sequence || diagramType == Uml::DiagramType::Collaboration) {
            ObjectWidget *ow = new ObjectWidget(scene, o);
            if (diagramType == Uml::DiagramType::Sequence) {
                y = ow->topMargin();
            }
            newWidget = ow;
        } else {
            UMLClassifier *c = static_cast<UMLClassifier*>(o);
            ClassifierWidget* interfaceWidget = new ClassifierWidget(scene, c);
            if (diagramType == Uml::DiagramType::Component || diagramType == Uml::DiagramType::Deployment) {
                interfaceWidget->setDrawAsCircle(true);
            }
            newWidget = interfaceWidget;
        }
        break;
    case UMLObject::ot_Class:
        //see if we really want an object widget or class widget
        if (diagramType == Uml::DiagramType::Class || diagramType == Uml::DiagramType::Component) {
            UMLClassifier *c = static_cast<UMLClassifier*>(o);
            ClassifierWidget *cw = new ClassifierWidget(scene, c);
            if (diagramType == Uml::DiagramType::Component)
                cw->setDrawAsCircle(true);
            newWidget = cw;
        } else {
            ObjectWidget *ow = new ObjectWidget(scene, o);
            if (diagramType == Uml::DiagramType::Sequence) {
                y = ow->topMargin();
            }
            newWidget = ow;
        }
        break;
    case UMLObject::ot_Category:
        newWidget = new CategoryWidget(scene, static_cast<UMLCategory*>(o));
        break;
    default:
        uWarning() << "trying to create an invalid widget (" << UMLObject::toString(type) << ")";
    }

    if (newWidget) {
        newWidget->setX(pos.x());
        newWidget->setY(y);
    }

    return newWidget;
}

bool validateObjType(UMLObject::ObjectType expected, UMLObject* &o, Uml::ID::Type id)
{
    if (o == NULL) {
        uDebug() << "Widget_Factory::validateObjType: creating new object of type "
                 << expected;
        QString artificialName = QLatin1String("LOST_") + Uml::ID::toString(id);
        o = Object_Factory::createUMLObject(expected, artificialName, NULL, false);
        if (o == NULL)
            return false;
        o->setID(id);
        UMLPackage *parentPkg = o->umlPackage();
        parentPkg->addObject(o);
        return true;
    }
    UMLObject::ObjectType actual = o->baseType();
    if (actual == expected)
        return true;
    uError() << "validateObjType(" << o->name()
        << "): expected type " << UMLObject::toString(expected) << ", actual type "
        << UMLObject::toString(actual);
    return false;
}

/**
 * Create a UMLWidget according to the given XMI tag.
 */
UMLWidget* makeWidgetFromXMI(const QString& tag,
                             const QString& idStr, UMLScene *scene)
{
    UMLWidget *widget = NULL;

        // Loading of widgets which do NOT represent any UMLObject,
        // just graphic stuff with no real model information
        //FIXME while boxes and texts are just diagram objects, activities and
        // states should be UMLObjects
    if (tag == QLatin1String("statewidget") || tag == QLatin1String("UML:StateWidget")) {
        widget = new StateWidget(scene, StateWidget::Normal, Uml::ID::Reserved);
    } else if (tag == QLatin1String("notewidget") || tag == QLatin1String("UML:NoteWidget")) {
        widget = new NoteWidget(scene, NoteWidget::Normal, Uml::ID::Reserved);
    } else if (tag == QLatin1String("boxwidget")) {
        widget = new BoxWidget(scene, Uml::ID::Reserved);
    } else if (tag == QLatin1String("floatingtext") || tag == QLatin1String("UML:FloatingTextWidget")) {
        widget = new FloatingTextWidget(scene, Uml::TextRole::Floating, QString(), Uml::ID::Reserved);
    } else if (tag == QLatin1String("activitywidget") || tag == QLatin1String("UML:ActivityWidget")) {
        widget = new ActivityWidget(scene, ActivityWidget::Initial, Uml::ID::Reserved);
    } else if (tag == QLatin1String("messagewidget")) {
        widget = new MessageWidget(scene, Uml::SequenceMessage::Asynchronous, Uml::ID::Reserved);
    } else if (tag == QLatin1String("forkjoin")) {
        widget = new ForkJoinWidget(scene, Qt::Vertical, Uml::ID::Reserved);
    } else if (tag == QLatin1String("preconditionwidget")) {
        widget = new PreconditionWidget(scene, NULL, Uml::ID::Reserved);
    } else if (tag == QLatin1String("combinedFragmentwidget")) {
        widget = new CombinedFragmentWidget(scene, CombinedFragmentWidget::Ref, Uml::ID::Reserved);
    } else if (tag == QLatin1String("signalwidget")) {
        widget = new SignalWidget(scene, SignalWidget::Send,  Uml::ID::Reserved);
    } else if (tag == QLatin1String("floatingdashlinewidget")) {
        widget = new FloatingDashLineWidget(scene, Uml::ID::Reserved);
    } else if (tag == QLatin1String("objectnodewidget")) {
        widget = new ObjectNodeWidget(scene, ObjectNodeWidget::Normal, Uml::ID::Reserved);
    } else if (tag == QLatin1String("regionwidget")) {
        widget = new RegionWidget(scene, Uml::ID::Reserved);
    } else if (tag == QLatin1String("pinwidget")) {
        PinPortBase *pw = new PinWidget(scene, NULL, Uml::ID::Reserved);
        pw->attachToOwner();
        widget = pw;
    }
    else
    {
        // Loading of widgets which represent an UMLObject

        // Find the UMLObject and create the Widget to represent it
        Uml::ID::Type id = Uml::ID::fromString(idStr);
        UMLDoc *umldoc = UMLApp::app()->document();
        UMLObject *o = umldoc->findObjectById(id);
        if (o == NULL) {
            uDebug() << "makeWidgetFromXMI: cannot find object with id "
                << Uml::ID::toString(id);
        }

        if (tag == QLatin1String("actorwidget") || tag == QLatin1String("UML:ActorWidget")) {
            if (validateObjType(UMLObject::ot_Actor, o, id))
                widget = new ActorWidget(scene, static_cast<UMLActor*>(o));
        } else if (tag == QLatin1String("usecasewidget") || tag ==  QLatin1String("UML:UseCaseWidget")) {
            if (validateObjType(UMLObject::ot_UseCase, o, id))
                widget = new UseCaseWidget(scene, static_cast<UMLUseCase*>(o));
        } else if (tag == QLatin1String("classwidget") ||
                   tag == QLatin1String("UML:ClassWidget") || tag == QLatin1String("UML:ConceptWidget")) {
            if (validateObjType(UMLObject::ot_Class, o, id) || validateObjType(UMLObject::ot_Package, o, id))
                widget = new ClassifierWidget(scene, static_cast<UMLClassifier*>(o));
        } else if (tag == QLatin1String("packagewidget")) {
            if (validateObjType(UMLObject::ot_Package, o, id))
                widget = new ClassifierWidget(scene, static_cast<UMLPackage*>(o));
        } else if (tag == QLatin1String("componentwidget")) {
            if (validateObjType(UMLObject::ot_Component, o, id))
                widget = new ComponentWidget(scene, static_cast<UMLComponent*>(o));
        } else if (tag == QLatin1String("portwidget")) {
            if (validateObjType(UMLObject::ot_Port, o, id))
                widget = new PortWidget(scene, static_cast<UMLPort*>(o));
        } else if (tag == QLatin1String("nodewidget")) {
            if (validateObjType(UMLObject::ot_Node, o, id))
                widget = new NodeWidget(scene, static_cast<UMLNode*>(o));
        } else if (tag == QLatin1String("artifactwidget")) {
            if (validateObjType(UMLObject::ot_Artifact, o, id))
                widget = new ArtifactWidget(scene, static_cast<UMLArtifact*>(o));
        } else if (tag == QLatin1String("interfacewidget")) {
            if (validateObjType(UMLObject::ot_Interface, o, id))
                widget = new ClassifierWidget(scene, static_cast<UMLClassifier*>(o));
        } else if (tag == QLatin1String("datatypewidget")) {
            if (validateObjType(UMLObject::ot_Datatype, o, id))
                widget = new DatatypeWidget(scene, static_cast<UMLClassifier*>(o));
        } else if (tag == QLatin1String("enumwidget")) {
            if (validateObjType(UMLObject::ot_Enum, o, id))
                widget = new EnumWidget(scene, static_cast<UMLEnum*>(o));
        } else if (tag == QLatin1String("entitywidget")) {
            if (validateObjType(UMLObject::ot_Entity, o, id))
                widget = new EntityWidget(scene, static_cast<UMLEntity*>(o));
        } else if (tag == QLatin1String("categorywidget")) {
            if (validateObjType(UMLObject::ot_Category, o, id))
                widget = new CategoryWidget(scene, static_cast<UMLCategory*>(o));
        } else if (tag == QLatin1String("objectwidget") || tag == QLatin1String("UML:ObjectWidget")) {
            widget = new ObjectWidget(scene, o);
        } else {
            uWarning() << "Trying to create an unknown widget:" << tag;
        }
    }
    return widget;
}

}   // end namespace Widget_Factory

