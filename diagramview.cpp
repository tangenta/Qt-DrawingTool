#include "diagramview.h"
#include <QKeyEvent>
#include <QDebug>
#include "diagramitem.h"
#include "diagramtextitem.h"

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

