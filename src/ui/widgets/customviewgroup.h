#pragma once

#include <QGraphicsScene>
#include <QGroupBox>

#include "customEvents.h"

class CustomViewGroup {
public:
    explicit CustomViewGroup(QWidget *parent = nullptr);

    [[nodiscard]] auto getGroupBox() const -> QGroupBox *;

    [[nodiscard]] auto getView() const -> ZoomAndScroll *;

private:
    QGroupBox *groupBox;
    ZoomAndScroll *view;
};
