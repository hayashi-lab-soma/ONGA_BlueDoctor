#ifndef REFRESH_H
#define REFRESH_H

#include <QDebug>
#include <QString>

#include "../Config/config.h"
#include "../Data/data.h"
#include "./statebase.h"

class Refresh : public StateBase
{
public:
	Refresh();

	// StateBase interface
protected:
	int _isTransition(Data *data);
	int _Enter(Data *data);
	int _Process(Data *data);
	void _Exit(Data *data);
};

#endif // REFRESH_H
