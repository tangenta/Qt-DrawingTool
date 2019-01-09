#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H
#include <QGraphicsView>

class DiagramView : public QGraphicsView
{
public:
    DiagramView(QWidget *parent = Q_NULLPTR);
    DiagramView(QGraphicsScene *scene, QWidget *parent = Q_NULLPTR);
protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif // DIAGRAMVIEW_H
