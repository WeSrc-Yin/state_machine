#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "stateMachine.h"

typedef struct _StmData {
    int id;
    char str[16];
}StmData;

typedef struct _StmDo {
    int action;
    void (*proc)(StmData *data);
}StmDo;

typedef struct _EventStruct {
	int event;
	char *string;
}EventStruct;

enum {
	EVENT_0,
	EVENT_1,
	EVENT_2,
	EVENT_3,
};

enum {
	ST_0,
	ST_1,
	ST_2,
	ST_3,
};

enum {
	DO_0,
	DO_1,
	DO_2,
	DO_3,
};

static StateTable state_table[] =
{
	{EVENT_0,	ST_0,	ST_1,	DO_0},
	{EVENT_0,	ST_1,	ST_2,	DO_1},
	{EVENT_0,	ST_2,	ST_3,	DO_2},
	{EVENT_0,	ST_3,	ST_0,	DO_3},

	{EVENT_1,	ST_0,	ST_2,	DO_1},
	{EVENT_1,	ST_1,	ST_3,	DO_2},
	{EVENT_1,	ST_2,	ST_0,	DO_3},
	{EVENT_1,	ST_3,	ST_1,	DO_0},

	{EVENT_2,	ST_0,	ST_3,	DO_2},
	{EVENT_2,	ST_1,	ST_0,	DO_3},
	{EVENT_2,	ST_2,	ST_1,	DO_0},
	{EVENT_2,	ST_3,	ST_2,	DO_1},

	{EVENT_3,	ST_0,	ST_0,	DO_3},
	{EVENT_3,	ST_1,	ST_1,	DO_0},
	{EVENT_3,	ST_2,	ST_2,	DO_1},
	{EVENT_3,	ST_3,	ST_3,	DO_2},
};

static void stmDo0(StmData *data)
{
	printf("[%s]id:%d,str:%s\n", __FUNCTION__,data->id,data->str);
}
static void stmDo1(StmData *data)
{
	printf("[%s]id:%d,str:%s\n", __FUNCTION__,data->id,data->str);
}
static void stmDo2(StmData *data)
{
	printf("[%s]id:%d,str:%s\n", __FUNCTION__,data->id,data->str);
}
static void stmDo3(StmData *data)
{
	printf("[%s]id:%d,str:%s\n", __FUNCTION__,data->id,data->str);
}
static StmDo stm_do[] =
{
	{DO_0,stmDo0},
	{DO_1,stmDo1},
	{DO_2,stmDo2},
	{DO_3,stmDo3},
};

static void stmHandle(StMachine *This,void *data)
{
	StmData *st_data = (StmData *)data;
	stm_do[This->getCurRun(This)].proc(st_data);
}

static void *thread(void *arg)
{
	StMachine *st_machine = (StMachine *)arg;
	while(1) {
		st_machine->run(st_machine);
		usleep(10000);
	}
	pthread_exit(NULL);
	return NULL;
}

static EventStruct event_struct[] = 
{
	{EVENT_0,"event0"},
	{EVENT_1,"event1"},
	{EVENT_2,"event2"},
	{EVENT_3,"event3"},
};

int main(int argc, char *argv[])
{
	int result;
	StmData *data;
	pthread_t m_pthread;                    //线程号
	pthread_attr_t threadAttr1;             //线程属性
	StMachine* stm;
	stm = stateMachineCreate(ST_0,
			state_table,
			sizeof (state_table) / sizeof ((state_table) [0]),
			0,
			stmHandle);

	pthread_attr_init(&threadAttr1);        //附加参数
	//设置线程为自动销毁
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);
	result = pthread_create(&m_pthread,&threadAttr1,thread,stm);
	if(result) {
	   printf("[%s] pthread failt,Error code:%d\n",__FUNCTION__,result);
	}
	pthread_attr_destroy(&threadAttr1);     //释放附加参数

	int index = 0;
	while (1) {
		
		 data = (StmData *)stm->initPara(stm,
				          sizeof(StmData));
		 data->id = index;
		 sprintf(data->str,"%s",event_struct[index].string);
		 stm->msgPost(stm,event_struct[index++].event,data);
		 if (index == 4)
			 index = 0;
		sleep(3);
	}
	return 0;
}
