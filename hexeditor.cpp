#include "hexeditor.h"

HexEditor::HexEditor(QWidget *parent)
{
    setReadOnly(true);
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

