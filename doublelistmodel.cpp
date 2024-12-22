#include "doublelistmodel.h"

DoubleListModel::DoubleListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QVariant DoubleListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return QString("Column %1").arg(section); // Название колонок
        } else if (orientation == Qt::Vertical) {
            return QString::number(section + 1); // Нумерация строк
        }
    }
    return QVariant();
    // FIXME: Implement me! DONE!
}

bool DoubleListModel::setHeaderData(int section,
                                    Qt::Orientation orientation,
                                    const QVariant &value,
                                    int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me! DONT NEED!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int DoubleListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_values.size();
    // FIXME: Implement me! DONE!
}

QVariant DoubleListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return QString::number(m_values.at(index.row()), 'f', 6); // Форматируем с 2 знаками
    } else if (role == Qt::EditRole) {
        return m_values.at(index.row()); // Возвращаем оригинальный double для редактирования
    }
    // FIXME: Implement me! DONE!
    return QVariant();
}

bool DoubleListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_values.size()) {
        return false;
    }

    if (role == Qt::EditRole) {
        bool ok;
        double newValue = value.toDouble(&ok);
        if (ok) {
            m_values[index.row()] = newValue;
            emit dataChanged(index, index, {role});
            return true;
        }
    } // DONE!
    return false;
}

Qt::ItemFlags DoubleListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable; // FIXME: Implement me!
}

bool DoubleListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > m_values.size()) {
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_values.insert(row, 0.0); // Добавляем значения по умолчанию
    }
    endInsertRows();
    return true;
}

bool DoubleListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > m_values.size()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_values.removeAt(row);
    }
    endRemoveRows();
    return true;
}

// Установить значения
void DoubleListModel::setValues(QVector<double> &values)
{
    beginResetModel();
    m_values = values;
    endResetModel();
}

// Получить значения
QVector<double> DoubleListModel::values() const
{
    return m_values;
}
