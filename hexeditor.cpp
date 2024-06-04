#include "hexeditor.h"

HexEditor::HexEditor(QWidget *parent)
{
    setReadOnly(true);
    //setCursorWidth(0);
}

void HexEditor::set_data(const QByteArray &data, const int size)
{
    QString hex_string;
    for(int i = 0; i < data.size(); i++)
    {
        hex_string.append(QString("%1 ").arg(static_cast<unsigned char>(data[i]), 2, 16, QChar('0')).toUpper());
        if(i % 16 == size)
            hex_string.append('\n');
    }
    setPlainText(hex_string.trimmed());
}

void HexEditor::keyPressEvent(QKeyEvent *event)
{
    int pos = textCursor().position();
    switch (event->key())
    {
    case Qt::Key_Left:
        if (pos > 0) setTextCursor(textCursorAt(pos - 3));
        break;
    case Qt::Key_Right:
        if (pos < toPlainText().length() - 2) setTextCursor(textCursorAt(pos + 3));
        break;
    case Qt::Key_Up:
        if (pos >= 48) setTextCursor(textCursorAt(pos - 48));
        break;
    case Qt::Key_Down:
        if (pos <= toPlainText().length() - 48) setTextCursor(textCursorAt(pos + 48));
        break;
    default:
        QPlainTextEdit::keyPressEvent(event);
    }
}

QTextCursor HexEditor::textCursorAt(int pos)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    return cursor;
}
