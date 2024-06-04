#ifndef HEXEDITOR_H
#define HEXEDITOR_H

#include <QPlainTextEdit>
#include <QKeyEvent>

class HexEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    HexEditor(QWidget *parent = nullptr);
    void set_data(const QByteArray &data, const int size);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QTextCursor textCursorAt(int pos);
};

#endif // HEXEDITOR_H
