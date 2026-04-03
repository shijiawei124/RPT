#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPolygonF>
#include <QPixmap>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MyGraphicsView(QWidget *parent = nullptr);
    ~MyGraphicsView();

    void setPixmap(const QPixmap &pixmap, bool fitInView = true);
    void setDrawMode(bool enable);
    QList<QPolygonF> getPolygons() const;
    void cleanItems();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    bool drawMode;
    QPolygonF currentPolygon;
    QList<QPolygonF> polygons;
    QGraphicsPathItem *currentPathItem;
};

#endif // MYGRAPHICSVIEW_H
