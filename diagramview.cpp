#include "diagramview.h"
#include <QKeyEvent>
#include <QDebug>
#include "diagramitem.h"
#include "diagramtextitem.h"
#include <QApplication>

QPen const DiagramView::penForLines = QPen(QBrush(QColor(Qt::black)), 2);

DiagramView::DiagramView(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent) {
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
}


void DiagramView::keyPressEvent(QKeyEvent* event) {
    if ((event->modifiers() & Qt::KeyboardModifier::ControlModifier) != 0) {
        setDragMode(DragMode::ScrollHandDrag);
    }
    QGraphicsView::keyPressEvent(event);
}

void DiagramView::keyReleaseEvent(QKeyEvent* event) {
    if ((event->modifiers() & Qt::KeyboardModifier::ControlModifier) == 0) {
        setDragMode(DragMode::RubberBandDrag);
    }
}

void DiagramView::mouseReleaseEvent(QMouseEvent* event) {
//    if (event->buttons() == Qt::LeftButton)
    clearOrthogonalLines();
    QGraphicsView::mouseReleaseEvent(event);
    bool needsEmit = false;
    foreach(QGraphicsItem* item, scene()->selectedItems()) {
        if (item->type() == DiagramItem::Type) {
            DiagramItem* p = qgraphicsitem_cast<DiagramItem*>(item);
            if (p->isMoved) {
                needsEmit = true;
                p->isMoved = false;
            }
            if (p->isResized) {
                needsEmit = true;
                p->isResized = false;
            }
        } else if (item->type() == DiagramTextItem::Type) {
            DiagramTextItem* p = qgraphicsitem_cast<DiagramTextItem*>(item);
            if (p->positionIsUpdated()) {
                qDebug() << "isUpdated!";
                needsEmit = true;
                p->setUpdated();
            }
        }
    }
    if (needsEmit) emit needsUndoBackup();
}

void DiagramView::mouseMoveEvent(QMouseEvent* event) {
    clearOrthogonalLines();
    if ((event->buttons() & Qt::LeftButton) != 0 && !scene()->selectedItems().empty()) {
        QGraphicsItem* itemUnderCursor = scene()->selectedItems().first();
        QPointF curCenter = itemUnderCursor->scenePos();
        foreach(QGraphicsItem* p, scene()->items()) {
            if (p == itemUnderCursor || p->type() != DiagramItem::Type) continue;
            DiagramItem* item = qgraphicsitem_cast<DiagramItem*>(p);
            QPointF const& objPoint = item->scenePos();
            LineAttr lineAttr;
            if ((lineAttr = isHorizontalOrVertical(objPoint, curCenter)) != Other) {
                if ((lineAttr & Horizontal) != 0) {
                    QGraphicsLineItem* newHLine = new QGraphicsLineItem();
                    newHLine->setLine(QLineF(QPointF(0, objPoint.y()),
                                             QPointF(sceneRect().width(), objPoint.y())));
                    newHLine->setPen(penForLines);
                    orthogonalLines.append(newHLine);
                }
                if ((lineAttr & Vertical) != 0) {
                    qDebug() << "verr";
                    QGraphicsLineItem* newVLine = new QGraphicsLineItem();
                    newVLine->setLine(QLineF(QPointF(objPoint.x(), 0),
                                             QPointF(objPoint.x(), sceneRect().height())));
                    newVLine->setPen(penForLines);
                    orthogonalLines.append(newVLine);
                }
            }
        }
    }
    foreach(QGraphicsLineItem* p, orthogonalLines) {
        scene()->addItem(p);
    }

    QGraphicsView::mouseMoveEvent(event);
}

void DiagramView::clearOrthogonalLines() {
    foreach(QGraphicsLineItem* p, orthogonalLines) {
        scene()->removeItem(p);
        delete p;
    }
    orthogonalLines.clear();
}

DiagramView::LineAttr DiagramView::isHorizontalOrVertical(const QPointF& p1, const QPointF& p2) {
    int ret = Other;
    ret |= std::abs(p1.x() - p2.x()) < delta ? Vertical : Other;
    ret |= std::abs(p1.y() - p2.y()) < delta ? Horizontal : Other;
    return static_cast<DiagramView::LineAttr>(ret);
}

