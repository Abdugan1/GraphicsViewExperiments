#ifndef ITEMRESIZER_H
#define ITEMRESIZER_H

#include <QObject>
#include <QRectF>

class ResizeHandle;
class FlowchartItem;

class ItemResizer : public QObject
{
    Q_OBJECT
public:
    explicit ItemResizer(FlowchartItem* targetResizeItem, QObject *parent = nullptr);

    void setCompareRect(const QRectF &newCompareRect);

signals:
    void resizeBeenMade();

public slots:
    void onHandleMoved(ResizeHandle* resizeHandle, qreal dx, qreal dy);

private:
    void updateCompareRect(const ResizeHandle* resizeHandle, qreal dx, qreal dy);
    void doResize();
    void limitResize();

private:
    FlowchartItem* targetResizeItem_ = nullptr;
    QRectF compareRect_;
    QRectF oldCompareRect_;

    qreal minWidth_  = 0.0f;
    qreal minHegith_ = 0.0f;
    qreal maxWidth_  = INFINITY;
    qreal maxHeight_ = INFINITY;
};

#endif // ITEMRESIZER_H
