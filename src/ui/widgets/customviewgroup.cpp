#include <QVBoxLayout>

#include "customviewgroup.h"

CustomViewGroup::CustomViewGroup(QWidget *parent) {
    groupBox = new QGroupBox(parent);
    auto *layout = new QVBoxLayout(groupBox);
    auto *chart = new QChart();
    view = new ZoomAndScroll(chart);
    layout->addWidget(view);
}

QGroupBox *CustomViewGroup::getGroupBox() const {
    return groupBox;
}

ZoomAndScroll *CustomViewGroup::getView() const {
    return view;
}
