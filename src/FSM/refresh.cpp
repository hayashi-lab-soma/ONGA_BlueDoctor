#include "refresh.h"

Refresh::Refresh()
{

}


int Refresh::_isTransition(Data *data)
{
	return FSM_Values::Stanby;
}

int Refresh::_Enter(Data *data)
{
	//Refresh process (execute once)
	data->BDs.clear();
	data->BDs_White.clear();
	data->BDs_Green.clear();
	data->BDs_Yellow.clear();
	data->BDs_Orange.clear();
	data->BDs_Red.clear();

	return 0;
}

int Refresh::_Process(Data *data)
{
	return 0;
}

void Refresh::_Exit(Data *data)
{
	//Execute once
	data->isRefresh = false;
	return;
}
