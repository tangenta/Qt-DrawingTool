#include "undosystem.h"
#include <QDebug>

void UndoSystem::backup(const QList<QGraphicsItem*>&& items) {
    qDebug() << "inside backup." << items.size();
    int stackSize = itemsStack.length();
    if (currentIndex < stackSize - 1) {
        for (int i = currentIndex + 1; i < stackSize; ++i) {
            free(itemsStack[i]);
        }
        itemsStack.erase(itemsStack.begin() + currentIndex + 1, itemsStack.end());
    }

    itemsStack.push_back(items);
    currentIndex++;
}

QList<QGraphicsItem*> UndoSystem::undo() {
    return itemsStack[--currentIndex];
}

QList<QGraphicsItem*> UndoSystem::redo() {
    return itemsStack[++currentIndex];
}

void UndoSystem::free(QList<QGraphicsItem*> const& items) {
    foreach(QGraphicsItem* p, items) {
        delete p;
    }
}
