#ifndef FSM_H
#define FSM_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QMap>

#include "../Config/config.h"
#include "../Data/data.h"
#include "./statebase.h"

#include "./stanby.h"
#include "./measurement.h"
#include "./refresh.h"

class FSM : public QObject
{
	Q_OBJECT
public:
	explicit FSM(QObject *parent = nullptr);

	int initialize();
	void main(Data *data);

signals:

public slots:

private:
	Config *cfg;
	QMap<int, StateBase*> fsm;

	//State classes
	Stanby *stanby;
	Measurement *measurement;
	Refresh *refresh;
};

#endif // ANALYS_H
