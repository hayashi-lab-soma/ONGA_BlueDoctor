#include "stanby.h"

Stanby::Stanby()
{

}

int Stanby::_isTransition(Data *data)
{
	if(data->isMeasure){
		//Transition to Measurement state
		return FSM_Values::Measurement;
	}
	if(data->isRefresh){
		//Transition to Refresh state
		return FSM_Values::Refresh;
	}

	//Staying
	return FSM_Values::Stanby;
}

int Stanby::_Enter(Data *data)
{
	return 0;
}

int Stanby::_Process(Data *data)
{
	return 0;
}

void Stanby::_Exit(Data *data)
{
}
