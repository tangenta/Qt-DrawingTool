/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "diagramscene.h"
#include "arrow.h"

#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

QPen const DiagramScene::penForLines = QPen(QBrush(QColor(Qt::black)), 2, Qt::PenStyle::DashLine);

DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Step;
    line = nullptr;
    textItem = nullptr;
    myItemColor = Qt::white;
    myTextColor = Qt::black;
    myLineColor = Qt::black;
}

void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    foreach (QGraphicsItem* p, selectedItems()) {
        if (p->type() == Arrow::Type) {
            Arrow* item = qgraphicsitem_cast<Arrow*>(p);
            item->setColor(myLineColor);
            update();
        }
    }
}

void DiagramScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    foreach (QGraphicsItem* p, selectedItems()) {
        if (p->type() == DiagramTextItem::Type) {
            DiagramTextItem* item = qgraphicsitem_cast<DiagramTextItem*>(p);
            item->setDefaultTextColor(myTextColor);
        }
    }
}
//! [2]

//! [3]
void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    foreach (QGraphicsItem* p, selectedItems()) {
        if (p->type() == DiagramItem::Type) {
            DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
            item->setBrush(myItemColor);
        }
    }
}

void DiagramScene::setFont(const QFont &font)
{
    myFont = font;
    foreach (QGraphicsItem* p, selectedItems()) {
        if (p->type() == DiagramTextItem::Type) {
            DiagramTextItem* item = qgraphicsitem_cast<DiagramTextItem*>(p);
            item->setFont(myFont);
        }
    }
}

void DiagramScene::deleteItems(QList<QGraphicsItem*> const& items) {
    qDebug() << "delete items" << items;

    QList<QGraphicsItem*> diagramItems;
    foreach (QGraphicsItem *item, items) {
        if (item->type() == Arrow::Type) {
            removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        } else diagramItems.append(item);
    }

    foreach (QGraphicsItem *item, diagramItems) {
        if (item->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
        removeItem(item);
        delete item;
    }
}
//! [4]

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}

//! [5]
void DiagramScene::editorLostFocus(DiagramTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    } else {
        if (item->contentIsUpdated()) {
            qDebug() << "content update ---";
            emit textChanged();
        }
    }

}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    DiagramItem *item;
    switch (myMode) {
        case InsertItem:
            item = new DiagramItem(myItemType, myItemMenu);
            item->setBrush(myItemColor);
            addItem(item);
            item->setPos(mouseEvent->scenePos());
            qDebug() << "insert item at: " << mouseEvent->scenePos();
            qDebug() << "\ttype: " << myItemType << " color: " << myItemColor;
            emit itemInserted(item);
            hasItemSelected = itemAt(mouseEvent->scenePos(), QTransform()) != nullptr;
            break;

        case InsertLine:
            if (itemAt(mouseEvent->scenePos(), QTransform()) == nullptr) break;
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;

        case InsertText:
            textItem = new DiagramTextItem();
            textItem->setFont(myFont);
            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
            textItem->setZValue(1000.0);
            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)),
                    this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
                    this, SIGNAL(itemSelected(QGraphicsItem*)));
            addItem(textItem);
            textItem->setDefaultTextColor(myTextColor);
            textItem->setPos(mouseEvent->scenePos());
            emit textInserted(textItem);
            qDebug() << "text inserted at" << textItem->scenePos();
            break;

    default:
        hasItemSelected = itemAt(mouseEvent->scenePos(), QTransform()) != nullptr;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (myMode == InsertLine && line != nullptr) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        if (hasItemSelected)
            mouseDraggingMoveEvent(mouseEvent);
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    hasItemSelected = false;

    // leave sticky mode
    horizontalStickyMode = false;
    verticalStickyMode = false;
    foreach(QGraphicsItem* p, selectedItems())
        p->setFlag(QGraphicsItem::ItemIsMovable);

    clearOrthogonalLines();
    if (line != nullptr && myMode == InsertLine) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems.first());
            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            addItem(arrow);
            arrow->updatePosition();
            emit arrowInserted();
        }
    }

    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DiagramScene::wheelEvent(QGraphicsSceneWheelEvent* wheelEvent) {
    // ctrl key is being pressed
    if ((wheelEvent->modifiers() & Qt::KeyboardModifier::ControlModifier) != 0) {
        emit scaleChanging(wheelEvent->delta());
        wheelEvent->accept();
    } else {
        QGraphicsScene::wheelEvent(wheelEvent);
    }
}

void DiagramScene::mouseDraggingMoveEvent(QGraphicsSceneMouseEvent* event) {
    clearOrthogonalLines();
    if ((event->buttons() & Qt::LeftButton) != 0 && selectedItems().size() == 1) {
        QGraphicsItem* itemUnderCursor = selectedItems().first();
        QPointF curCenter = itemUnderCursor->scenePos();
        QPointF const& mousePos = event->scenePos();

        foreach(QGraphicsItem* p, items()) {
            if (p->type() != DiagramItem::Type || p == itemUnderCursor) continue;

            DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
            QPointF const& objPoint = item->scenePos();
            LineAttr lineAttr;

            tryEnteringStickyMode(itemUnderCursor, objPoint, mousePos);

            if ((lineAttr = getPointsRelationship(objPoint, curCenter)) != Other) {
                if ((lineAttr & Horizontal) != 0) {
                    QGraphicsLineItem* newHLine = new QGraphicsLineItem();
                    newHLine->setLine(QLineF(QPointF(0, objPoint.y()),
                                             QPointF(sceneRect().width(), objPoint.y())));
                    newHLine->setPen(penForLines);
                    orthogonalLines.append(newHLine);
                }
                if ((lineAttr & Vertical) != 0) {
                    QGraphicsLineItem* newVLine = new QGraphicsLineItem();
                    newVLine->setLine(QLineF(QPointF(objPoint.x(), 0),
                                             QPointF(objPoint.x(), sceneRect().height())));
                    newVLine->setPen(penForLines);
                    orthogonalLines.append(newVLine);
                }
            }
        }
        tryLeavingStickyMode(itemUnderCursor, mousePos);
    }
    foreach(QGraphicsLineItem* p, orthogonalLines) {
        addItem(p);
    }
}

void DiagramScene::clearOrthogonalLines() {
    foreach(QGraphicsLineItem* p, orthogonalLines) {
        removeItem(p);
        delete p;
    }
    orthogonalLines.clear();
}

bool DiagramScene::closeEnough(qreal x, qreal y, qreal delta) {
    return std::abs(x - y) < delta;
}

DiagramScene::LineAttr DiagramScene::getPointsRelationship(const QPointF& p1,
                                                           const QPointF& p2) {
    int ret = Other;
    ret |= closeEnough(p1.x(), p2.x(), Delta) ? Vertical : Other;
    ret |= closeEnough(p1.y(), p2.y(), Delta) ? Horizontal : Other;
    return static_cast<DiagramScene::LineAttr>(ret);
}

void DiagramScene::tryEnteringStickyMode(QGraphicsItem* item, const QPointF& target,
                                         const QPointF& mousePos) {
    QPointF const& itemPos = item->scenePos();
    if (!verticalStickyMode) {
        if (closeEnough(itemPos.x(), target.x(), stickyDistance)) {  // enter stickyMode
            verticalStickyMode = true;
            verticalStickPoint = mousePos;
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setPos(QPointF(target.x(), itemPos.y()));
        }
    }
    if (!horizontalStickyMode) {
        if (closeEnough(itemPos.y(), target.y(), stickyDistance)) {
            horizontalStickyMode = true;
            horizontalStickPoint = mousePos;
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
            item->setPos(QPointF(itemPos.x(), target.y()));
        }
    }
}

void DiagramScene::tryLeavingStickyMode(QGraphicsItem* item, const QPointF& mousePos) {
    if (verticalStickyMode) { // already in stickyMode, item should be able to move vertically
        item->moveBy(0, mousePos.y() - verticalStickPoint.y());
        verticalStickPoint.setY(mousePos.y());

        // when to exit stickyMode?
        if (!closeEnough(mousePos.x(), verticalStickPoint.x(), stickyDistance)) {
            verticalStickyMode = false;
            item->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
    }
    if (horizontalStickyMode) {
        item->moveBy(mousePos.x() - horizontalStickPoint.x(), 0);
        horizontalStickPoint.setX(mousePos.x());

        if (!closeEnough(mousePos.y(), horizontalStickPoint.y(), stickyDistance)) {
            horizontalStickyMode = false;
            item->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
    }
}

