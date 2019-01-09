#include "diagramview.h"
#include <QKeyEvent>

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
