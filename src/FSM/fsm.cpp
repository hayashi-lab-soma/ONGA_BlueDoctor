#include "fsm.h"

FSM::FSM(QObject *parent) : QObject(parent)
{
	cfg = new Config();

	stanby = new Stanby();
	measurement = new Measurement();
	refresh = new Refresh();

	fsm[FSM_Values::Stanby] = stanby;
	fsm[FSM_Values::Measurement] = measurement;
	fsm[FSM_Values::Refresh] = refresh;
}

int FSM::initialize()
{
	return 0;
}

void FSM::main(Data *data)
{
	int next = fsm[data->ana_state]->isTransition(data);

	if(next != data->ana_state){
		fsm[data->ana_state]->Exit(data);
		fsm[next]->Enter(data);
		data->ana_state = next;
		return;
	}

	fsm[data->ana_state]->Process(data);
	return;
}
