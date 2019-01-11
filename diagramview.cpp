#include "diagramview.h"
#include <QKeyEvent>
#include <QDebug>
#include "diagramitem.h"

DiagramView::DiagramView(QWidget* parent): QGraphicsView(parent) {
}

DiagramView::DiagramView(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent) {

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
            DiagramItem* diagramItem = qgraphicsitem_cast<DiagramItem*>(item);
            if (diagramItem->isMoved) {
                needsEmit = true;
                diagramItem->isMoved = false;
            }
        }
    }
    if (needsEmit) emit needsUndoBackup();
    qDebug() << "last";
}
