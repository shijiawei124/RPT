#include "mygraphicsview.h"
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPen>

MyGraphicsView::MyGraphicsView(QWidget *parent) :
    QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    pixmapItem(nullptr),
    drawMode(false),
    currentPathItem(nullptr)
{
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

MyGraphicsView::~MyGraphicsView()
{
    delete scene;
}

void MyGraphicsView::setPixmap(const QPixmap &pixmap, bool fitInView)
{
    if (pixmapItem) {
        scene->removeItem(pixmapItem);
        delete pixmapItem;
        pixmapItem = nullptr;
    }
    
    if (!pixmap.isNull()) {
        pixmapItem = scene->addPixmap(pixmap);
        if (fitInView) {
            this->fitInView(pixmapItem, Qt::KeepAspectRatio);
        }
    }
}

void MyGraphicsView::setDrawMode(bool enable)
{
    drawMode = enable;
    if (!enable) {
        cleanItems();
    }
}

QList<QPolygonF> MyGraphicsView::getPolygons() const
{
    return polygons;
}

void MyGraphicsView::cleanItems()
{
    polygons.clear();
    if (currentPathItem) {
        scene->removeItem(currentPathItem);
        delete currentPathItem;
        currentPathItem = nullptr;
    }
    currentPolygon.clear();
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (drawMode) {
        QPointF scenePoint = mapToScene(event->pos());
        currentPolygon.clear();
        currentPolygon.append(scenePoint);
        
        if (currentPathItem) {
            scene->removeItem(currentPathItem);
            delete currentPathItem;
            currentPathItem = nullptr;
        }
        
        QPainterPath path;
        path.moveTo(scenePoint);
        currentPathItem = scene->addPath(path, QPen(Qt::red, 2));
    }
    QGraphicsView::mousePressEvent(event);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (drawMode && !currentPolygon.isEmpty()) {
        QPointF scenePoint = mapToScene(event->pos());
        currentPolygon.append(scenePoint);
        
        QPainterPath path;
        path.moveTo(currentPolygon.first());
        for (int i = 1; i < currentPolygon.size(); i++) {
            path.lineTo(currentPolygon[i]);
        }
        
        if (currentPathItem) {
            scene->removeItem(currentPathItem);
            delete currentPathItem;
        }
        currentPathItem = scene->addPath(path, QPen(Qt::red, 2));
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (drawMode && !currentPolygon.isEmpty()) {
        QPointF scenePoint = mapToScene(event->pos());
        currentPolygon.append(scenePoint);
        
        QPainterPath path;
        path.moveTo(currentPolygon.first());
        for (int i = 1; i < currentPolygon.size(); i++) {
            path.lineTo(currentPolygon[i]);
        }
        path.closeSubpath();
        
        if (currentPathItem) {
            scene->removeItem(currentPathItem);
            delete currentPathItem;
            currentPathItem = nullptr;
        }
        
        scene->addPath(path, QPen(Qt::red, 2), QBrush(QColor(255, 0, 0, 50)));
        polygons.append(currentPolygon);
        currentPolygon.clear();
    }
    QGraphicsView::mouseReleaseEvent(event);
}
