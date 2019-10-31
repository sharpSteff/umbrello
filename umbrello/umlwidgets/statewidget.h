/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2014                                               *
 *   Umbrello UML Modeller Authors <umbrello-devel@kde.org>                *
***************************************************************************/

#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include "umlwidget.h"

#include <QPainter>
#include <QStringList>

#define STATE_MARGIN 5
#define STATE_WIDTH 30
#define STATE_HEIGHT 10

/**
 * This class is the graphical version of a UML State.
 *
 * A StateWidget is created by a @ref UMLView.  A StateWidget belongs to
 * only one @ref UMLView instance.
 * When the @ref UMLView instance that this class belongs to is destroyed,
 * it will be automatically deleted.
 *
 * The StateWidget class inherits from the @ref UMLWidget class
 * which adds most of the functionality to this class.
 *
 * @short  A graphical version of a UML State.
 * @author Paul Hensgen <phensgen@techie.com>
 * Bugs and comments to umbrello-devel@kde.org or http://bugs.kde.org
 */
class StateWidget : public UMLWidget
{
    Q_OBJECT
    Q_ENUMS(StateType)
public:
    /// Enumeration that codes the different types of state.
    enum StateType
    {
        Initial = 0,     // Pseudostate
        Normal,
        End,
        Fork,            // Pseudostate
        Join,            // Pseudostate
        Junction,        // Pseudostate
        DeepHistory,     // Pseudostate
        ShallowHistory,  // Pseudostate
        Choice,          // Pseudostate
        //Terminate        // Pseudostate
        //EntryPoint       // Pseudostate
        //ExitPoint        // Pseudostate
        Combined         // Pseudostate
    };

    explicit StateWidget(UMLScene * scene, StateType stateType = Normal, Uml::ID::Type id = Uml::ID::None);
    virtual ~StateWidget();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    StateType stateType() const;
    QString stateTypeStr() const;
    void setStateType(StateType stateType);

    bool addActivity(const QString &activity);
    bool removeActivity(const QString &activity);
    bool renameActivity(const QString &activity, const QString &newName);

    QStringList activities() const;
    void setActivities(const QStringList &list);

    bool drawVertical() const;
    void setDrawVertical(bool to = true);

    virtual bool showPropertiesDialog();

    Uml::ID::Type diagramLink();
    bool setDiagramLink(const Uml::ID::Type &id);

    virtual bool loadFromXMI1(QDomElement & qElement);
    virtual void saveToXMI1(QDomDocument & qDoc, QDomElement & qElement);
    virtual bool activate(IDChangeLog* changeLog = 0);

protected:
    QSizeF minimumSize() const;
    QSizeF maximumSize();
    void setAspectRatioMode();
    void setupEvent(QGraphicsSceneMouseEvent &e, QGraphicsSceneMouseEvent *event, const QPointF &pos);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public Q_SLOTS:
    virtual void slotMenuSelection(QAction* action);

private:
    StateType   m_stateType;   ///< Type of state.
    bool m_drawVertical;   ///< whether to draw the fork/join horizontally or vertically
    QStringList m_Activities;  ///< List of activities for the state.
    Uml::ID::Type m_diagramLinkId; ///< id of linked diagram
    QPointer<UMLScene> m_linkedDiagram; ///< pointer to linked diagram
    QRectF m_clientRect; ///< widget area for embedded diagram
    QRectF m_sceneRect;  ///< scene rectangle used for internal calculations
    QSizeF m_size;       ///< widget size used by combined state
};

#endif
