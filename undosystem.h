#ifndef UNDOSYSTEM_H
#define UNDOSYSTEM_H
#include "diagramitem.h"
#include "diagramscene.h"

class UndoSystem {
public:
    void backup(QList<QGraphicsItem*> const&& items);
    QList<QGraphicsItem*> undo();
    QList<QGraphicsItem*> redo();
    bool isEmpty() {return currentIndex < 1;}
    bool isFull() {return currentIndex + 1 == itemsStack.length();}

private:
    void free(QList<QGraphicsItem*> const& items);
    QList<QList<QGraphicsItem*>> itemsStack;
    int currentIndex = -1;
};

#endif // UNDOSYSTEM_H
