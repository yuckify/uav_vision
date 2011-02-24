#ifndef SQLTABLEMODEL_HPP
#define SQLTABLEMODEL_HPP

#include<QSqlTableModel>
#include<QSqlQueryModel>

class SqlTableModel : public QSqlQueryModel {
	Q_OBJECT
public:
	
	Qt::ItemFlags flags(const QModelIndex& index) {
		return Qt::NoItemFlags;
	}
	
	bool setData(const QModelIndex &index, const QVariant &value, int role) {
		return emit signalData(index, value, role);
	}
	
signals:
	bool signalData(const QModelIndex& index, const QVariant& value, int role);
	
};

#endif // SQLTABLEMODEL_HPP
