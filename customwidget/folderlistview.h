#ifndef FOLDERLISTVIEW_H
#define FOLDERLISTVIEW_H

#include <QWidget>
#include <QListView>

class FolderListView : public QListView
{
    Q_OBJECT
public:
    explicit FolderListView(QWidget *parent = 0);

    int rowCount();

signals:

public slots:
protected slots:
    void paintEvent(QPaintEvent *e);
};

#endif // FOLDERLISTVIEW_H
