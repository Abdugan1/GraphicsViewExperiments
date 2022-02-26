#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

class Scene;
class FlowchartShapeItem;
class QUndoStack;

class QLabel;

class View : public QGraphicsView
{
    Q_OBJECT
public:
    explicit View(Scene *scene, QWidget *parent = nullptr);
    ~View();

signals:
    void scaleChanged(qreal scale);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void wheelEvent(QWheelEvent* event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent* event) override;

public slots:
    void addToUndoStackMoveCommand(QGraphicsItem* item, const QPointF& oldPos);

    void updateGridColor(QColor color);

    void zoom(qreal zoomRatio);

private slots:
    void showAndUpdateItemInfoLabels(FlowchartShapeItem* selectedItem);
    void hideItemInfoLabels();
    void updateItemInfoLabels(FlowchartShapeItem* selectedItem);

private:
    void init();
    void initFlags();
    void initFont();
    void initLayout();
    void initActions();
    void initConnection();

    void zoomIn();
    void zoomOut();

    void updateSelectedItemPositionLabel(FlowchartShapeItem* selectedItem);
    void updateSelectedItemFigureTypeLabel(FlowchartShapeItem* selectedItem);

    void updateCursorPositionLabel(const QPoint& pos);

private:
    Scene* scene_ = nullptr;

    QLabel* cursorPositionLabel_         = nullptr;

    // Selected item info labels
    QLabel* selectedItemFigureTypeLabel_ = nullptr;
    QLabel* selectedItemPositionLabel_   = nullptr;

    QUndoStack* undoStack_ = nullptr;
    QAction* undoAction_ = nullptr;
    QAction* redoAction_ = nullptr;

    QColor gridColor_ = Qt::lightGray;
};

#endif // VIEW_H
