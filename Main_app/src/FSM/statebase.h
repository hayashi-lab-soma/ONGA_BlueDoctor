#ifndef STATEBASE_H
#define STATEBASE_H

#include <QString>
#include <QDebug>

#include "../Config/config.h"
#include "../Data/Data.h"

#define IS_TRACE 0
#if IS_TRACE == 1
#define TRACE(msg) qDebug() << msg
#else
#define TRACE(msg)
#endif

class StateBase
{
public:
	StateBase();

	int initialize();
	int isTransition(Data *data);
	int Enter(Data *data);
	int Process(Data *data);
	void Exit(Data *data);

protected:
	virtual int _isTransition(Data *data) = 0;
	virtual int _Enter(Data *data) = 0;
	virtual int _Process(Data *data) = 0;
	virtual void _Exit(Data *data) = 0;

protected:
};

#endif // STATEBASE_H
