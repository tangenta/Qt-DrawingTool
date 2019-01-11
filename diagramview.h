#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H
#include <QGraphicsView>

class DiagramView : public QGraphicsView {
    Q_OBJECT
public:
    DiagramView(QGraphicsScene *scene, QWidget *parent = Q_NULLPTR);
signals:
    void needsUndoBackup();
protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    void clearOrthogonalLines();
    enum LineAttr { Other = 0, Horizontal, Vertical, Both};
    LineAttr isHorizontalOrVertical(QPointF const& p1, QPointF const& p2);
    QList<QGraphicsLineItem*> orthogonalLines;
//    bool leftButtonDown = false;
    static const QPen penForLines;
    static constexpr qreal delta = 0.01;
    static constexpr qreal stickyDistance = 5;
};

#endif // DIAGRAMVIEW_H
