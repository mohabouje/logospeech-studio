#include "folderlistview.h"

#include <QPainter>

FolderListView::FolderListView(QWidget *parent) : QListView(parent)
{

}


int FolderListView::rowCount(){
    if( model() == NULL)
        return 0;
    else
        return model()->rowCount();
}



void FolderListView::paintEvent(QPaintEvent *e){
    if(rowCount() == 0) {
        QPainter painter(viewport());
        painter.setPen(Qt::black);
        painter.setFont(QFont());
        painter.drawText(viewport()->geometry(), Qt::AlignCenter, "No supported files founded");
    }
    QListView::paintEvent(e);
}
