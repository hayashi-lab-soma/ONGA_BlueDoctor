#ifndef STANBY_H
#define STANBY_H

#include <QDebug>
#include <QString>

#include "../Config/config.h"
#include "../Data/data.h"
#include "./statebase.h"

class Stanby : public StateBase
{
public:
	Stanby();

	// StateBase interface
protected:
	int _isTransition(Data *data);
	int _Enter(Data *data);
	int _Process(Data *data);
	void _Exit(Data *data);
};

#endif // STANBY_H
