#ifndef STARTMEASUREMENTSTATE_H
#define STARTMEASUREMENTSTATE_H

#include <QDebug>
#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#include "../Config/config.h"
#include "../Data/data.h"
#include "./statebase.h"

class Measurement : public StateBase
{
public:
	Measurement();

	// StateBase interface
protected:
	int _isTransition(Data *data);
	int _Enter(Data *data);
	int _Process(Data *data);
	void _Exit(Data *data);
};

#endif // STARTMEASUREMENTSTATE_H
