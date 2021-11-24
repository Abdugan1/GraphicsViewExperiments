#include "view.h"
#include "scene.h"
#include "flowchartitems.h"

#include <QWheelEvent>
#include <QTimeLine>
#include <QLabel>
#include <QToolButton>
#include <QBoxLayout>
#include <QDebug>

const QString CursorPositionText            = QObject::tr("Cursor position: (%1, %2)");
const QString SelectedItemPositionText      = QObject::tr("Item position: (%1, %2)");
const QString SelectedItemFigureTypeText    = QObject::tr("Figure type: %1");
const QPen GridPen = QPen(Qt::lightGray, 0.5);

View::View(Scene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
    , scene_(scene)
    , cursorPositionLabel_(new QLabel(CursorPositionText.arg("0").arg("0")))
    , selectedItemFigureTypeLabel_(new QLabel(SelectedItemFigureTypeText))
    , selectedItemPositionLabel_(new QLabel(SelectedItemPositionText))
{
    init();
}

void View::drawBackground(QPainter *painter, const QRectF &rect)
{
    constexpr int gridSize = 20;
    int left = int(rect.left() - (int(rect.left()) % gridSize));
    int top  = int(rect.top()  - (int(rect.top())  % gridSize));

    QVarLengthArray<QLineF, 100> lines; // It works faster than reserving. IDK why
    for (int x = left; x < rect.right(); x += gridSize) {
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    }
    for (int y = top; y < rect.bottom(); y += gridSize) {
        lines.append(QLineF(rect.left(), y, rect.right(), y));
    }

    painter->setPen(GridPen);
    painter->drawLines(lines.data(), lines.size());

    QGraphicsView::drawBackground(painter, rect);
}

void View::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::CTRL) {
        int numDegrees = event->delta() / 8;
        qDebug() << "numDegrees:" << numDegrees;
        int numSteps = numDegrees / 15;
        numScheduledScalings_ += numSteps;
        if (numScheduledScalings_ * numSteps < 0)
            numScheduledScalings_ = numSteps;

        QTimeLine* anim = new QTimeLine(350, this);
        anim->setUpdateInterval(20);
        connect(anim, &QTimeLine::valueChanged, this, &View::scalingTime);
        connect(anim, &QTimeLine::finished, this, &View::animFinished);
        anim->start();

    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    updateCursorPositionLabel(event->pos());
    QGraphicsView::mouseMoveEvent(event);
}

void View::showAndUpdateItemInfoLabels(FlowchartItem *selectedItem)
{
    selectedItemPositionLabel_->show();
    selectedItemFigureTypeLabel_->show();
    updateItemInfoLabels(selectedItem);
}

void View::hideItemInfoLabels()
{
    selectedItemPositionLabel_->hide();
    selectedItemFigureTypeLabel_->hide();
}

void View::updateItemInfoLabels(FlowchartItem *selectedItem)
{
    updateSelectedItemFigureTypeLabel(selectedItem);
    updateSelectedItemPositionLabel(selectedItem);
}

void View::scalingTime(qreal x)
{
    qreal factor = 1.0f + qreal(numScheduledScalings_) / 300.0f;
//    qDebug() << "factor:" << factor;
    scale(factor, factor);
}

void View::animFinished()
{
    if (numScheduledScalings_ > 0) {
        --numScheduledScalings_;
    } else {
        ++numScheduledScalings_;
        sender()->deleteLater();
    }
}

void View::init()
{
    initFlags();
    initSomething();
    initLayout();
    initConnection();
}

void View::initFlags()
{
    setViewportUpdateMode(FullViewportUpdate);
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(AnchorUnderMouse);
}

void View::initSomething()
{
    QFont font = this->font();
    font.setPointSize(12);
    setFont(font);
}

void View::initLayout()
{
    auto createHorizontalSpacer = [=]() {
        return new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    };
    auto createVerticalSpacer = []() {
        return new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    };

    selectedItemFigureTypeLabel_->setAlignment(Qt::AlignRight);
    selectedItemFigureTypeLabel_->hide();

    selectedItemPositionLabel_->setAlignment(Qt::AlignRight);
    selectedItemPositionLabel_->hide();

    cursorPositionLabel_->setAlignment(Qt::AlignRight);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(selectedItemFigureTypeLabel_);
    vLayout->addWidget(selectedItemPositionLabel_);
    vLayout->addWidget(new QLabel()); // Spacer
    vLayout->addWidget(cursorPositionLabel_);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    QGridLayout* gLayout = new QGridLayout;
    gLayout->addItem(createHorizontalSpacer(), 0, 0);
    gLayout->addItem(createVerticalSpacer(), 0, 1);
    gLayout->addLayout(vLayout, 1, 1);

    gLayout->setContentsMargins(0, 0, 0, 0);
    gLayout->setSpacing(0);

    setLayout(gLayout);
}

void View::initConnection()
{
    connect(scene_, &Scene::itemSelected, this, &View::showAndUpdateItemInfoLabels);

    connect(scene_, &Scene::itemMoved, this, &View::updateItemInfoLabels);

    connect(scene_, &Scene::itemLostSelection, this, &View::hideItemInfoLabels);
}

void View::updateSelectedItemPositionLabel(FlowchartItem *selectedItem)
{
    QPointF pos = selectedItem->pos();
    selectedItemPositionLabel_->setText(SelectedItemPositionText.arg(pos.x()).arg(pos.y()));
}

void View::updateSelectedItemFigureTypeLabel(FlowchartItem *selectedItem)
{
    selectedItemFigureTypeLabel_->setText(SelectedItemFigureTypeText.arg(selectedItem->figureType()));
}

void View::updateCursorPositionLabel(const QPoint &pos)
{
    QPoint cursorPos = mapToScene(pos).toPoint();
    cursorPositionLabel_->setText(CursorPositionText.arg(cursorPos.x()).arg(cursorPos.y()));
}
