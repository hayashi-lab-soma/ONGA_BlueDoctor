#include "measurement.h"

Measurement::Measurement()
{
}

int Measurement::_isTransition(Data *data)
{
	if(!data->isMeasure && !data->isRefresh){
		return FSM_Values::Stanby;
	}
	return FSM_Values::Measurement;
}

//計測開始時に呼び出される関数
int Measurement::_Enter(Data *data)
{
	return 0;
}

//計測中に呼び出される関数（毎フレーム）
int Measurement::_Process(Data *data)
{
	//data->BDs << data->BD;
	//qInfo() << data->BD.XY << data->posBD_m;
	//	qInfo() << data->posBD_m;

	//ログ保存しましょう

	return 0;
}

//計測終了後に呼び出される関数
void Measurement::_Exit(Data *data)
{
}
