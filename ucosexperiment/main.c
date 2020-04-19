/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*********************************************************************************************************
*/
/*
for book:Ƕ��ʽʵʱ����ϵͳ��COSԭ����ʵ��
¬����
2011�ڳɶ����ӿƼ���ѧ
*/
#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE               512       /* Size of each task's stacks (# of WORDs)            */
#define TaskStart_Prio    1
#define Task1_Prio        2


// begin EDF
// begin EDF
//struct edf_task {
//    // ����
//    INT32U T;
//    // ִ��ʱ��
//    INT32U C;
//    // ���Խ�ֹʱ��
//    INT32U ADL;
//    // ��Խ�ֹʱ��
//    INT32U RDL;
//    // ����ʱ��
//    INT32U AT;
//};
//INT32U *readyList;
//INT32U readyNum;
//INT32U edfTaskNum;
//int *hPeriod,*hCompute,*hAbsDeadline,*hRelDeadline,*hArriveTime,*readyList,readyNum,taskNum;
void EDFTaskSet();

void EDFSchedTask(void *pParam);

void EDFTask1(void *pParam);

// end EDF

// begin RM

typedef struct rm_task {
    INT32U C;
    INT32U T;
    INT32U prior;
} rm_task;

void RMTaskSet();

void RMTask1(void *pParam);

// end RM


OS_STK TaskStk[OS_MAX_TASKS][TASK_STK_SIZE];    // Tasks stacks
HANDLE mainhandle;        //���߳̾��
CONTEXT Context;        //���߳��л�������
BOOLEAN FlagEn = 1;        //����һ��ȫ�ֱ�������Ϊ�Ƿ�ʱ�ӵ��ȵı�־


void TaskStart(void *pParam);
//void Task1(void * pParam) ;                            /* Function prototypes of tasks                  */

void VCInit(void);                        //��ʼ����ر���,һ����Ҫ

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/


int main(int argc, char **argv) {
    int p[2], Experiment;
    p[0] = 0;
    p[1] = 100;
    VCInit();
    printf("0.û���û�����\n");
    printf("1.��һ�����ӣ�һ���û�����\n");
    printf("2.�ڶ������ӣ�����������CPU��������\n");
    printf("3.���������ӣ�����Ĺ���ͻָ�\n");
    printf("4.���ĸ����ӣ��ź�������\n");
    printf("5.��������ӣ������ź�������\n");
    printf("6.���������ӣ��¼���־��\n");
    printf("7.���߸����ӣ���Ϣ����\n");
    printf("8.�ڰ˸����ӣ���Ϣ����\n");
    printf("9.�ھŸ����ӣ��ڴ����\n");
    printf("10.��10�����ӣ�131-7\n");
    printf("11.��11�����ӣ�131-8\n");
    printf("12.��12�����ӣ�131-9\n");
    printf("13.RM RM RM\n");
    printf("14.EDF EDF EDF\n");

    printf("���������ѡ������:\n");
    scanf("%d", &Experiment);
    if ((Experiment < 0) || (Experiment > 100)) {
        printf("��Ч������!");
        return (1);
    }
    OSInit();
    OSTaskCreate(TaskStart, 0, &TaskStk[1][TASK_STK_SIZE - 1], TaskStart_Prio);
    switch (Experiment) {
        case 1://һ����������?
            OSTaskCreate(FirstTask, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
            break;
        case 2://����������CPU
            OSTaskCreate(E2_task1, 0, &TaskStk[5][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(E2_task2, 0, &TaskStk[6][TASK_STK_SIZE - 1], 5);
            break;
        case 3://����Ĺ���ͻָ�
            OSTaskCreate(E3_task0, 0, &TaskStk[5][TASK_STK_SIZE - 1], 7);
            OSTaskCreate(E3_task1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(E3_task2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 5);
            break;
        case 4://�ź�����������
            OSTaskCreate(UserTaskSemA, 0, &TaskStk[5][TASK_STK_SIZE - 1], 7);
            OSTaskCreate(UserTaskSemB, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(UserTaskSemC, 0, &TaskStk[7][TASK_STK_SIZE - 1], 5);
            break;
        case 10:
            OSTaskCreate(hyl6A, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
            OSTaskCreate(hyl6B, 0, &TaskStk[6][TASK_STK_SIZE - 1], 7);
            OSTaskCreate(hyl6C, 0, &TaskStk[8][TASK_STK_SIZE - 1], 8);
            OSTaskCreate(hyl6D, 0, &TaskStk[7][TASK_STK_SIZE - 1], 6);
            break;
        case 11:
            OSTaskCreate(hylProcess, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
            OSTaskCreate(hylsubProcess1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(hylsubProcess2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
            break;
        case 12:
            OSTaskCreate(TaskMA, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
            OSTaskCreate(TaskMB, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            break;
        case 13:
            RMTaskSet();
            break;
        case 14:
            printf("CurrentTime Event [From Task ID] [To Task ID]\n");
            break;
        case 5://�����ź�����������
            OSTaskCreate(TaskMutex1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskMutex2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 50);
            OSTaskCreate(TaskPrint, 0, &TaskStk[8][TASK_STK_SIZE - 1], 30);
            break;
        case 6://ʱ���־���������
            OSTaskCreate(TaskDataProcess, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
            OSTaskCreate(TaskIO1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskIO2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
            OSTaskCreate(TaskIO3, 0, &TaskStk[8][TASK_STK_SIZE - 1], 8);
            OSTaskCreate(TaskIO4, 0, &TaskStk[9][TASK_STK_SIZE - 1], 9);
            break;
        case 7://��Ϣ����
            OSTaskCreate(TaskMessageSen, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskMessageRec, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
            break;
        case 8://��Ϣ����
            OSTaskCreate(TaskQSen, 0, &TaskStk[7][TASK_STK_SIZE - 1], 5);
            OSTaskCreate(TaskQRec, 0, &TaskStk[8][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskQRec, 0, &TaskStk[9][TASK_STK_SIZE - 1], 7);
            break;
        case 9://�ڴ����
            OSTaskCreate(TaskM, 0, &TaskStk[8][TASK_STK_SIZE - 1], 6);
            break;
        default:;
    }

    OSStart();
    return (0);
}

void VCInit(void) {
    HANDLE cp, ct;
    Context.ContextFlags = CONTEXT_CONTROL;
    cp = GetCurrentProcess();    //�õ���ǰ���̾��
    ct = GetCurrentThread();    //�õ���ǰ�߳�α���
    DuplicateHandle(cp, ct, cp, &mainhandle, 0, TRUE, 2);    //α���ת��,�õ��߳�����

}

void TaskStart(void *pParam) {
    char err;
    OS_EVENT *sem1;

    /*ģ�����ö�ʱ���жϡ�����һ����ʱ���߳�,ÿ���ж�100��,�жϷ������OSTickISRuser*/
    timeSetEvent(1000 / OS_TICKS_PER_SEC, 0, OSTickISRuser, 0, TIME_PERIODIC);
    OSStatInit(); /*ͳ�������ʼ��*/
    sem1 = OSSemCreate(0);
    OSSemPend(sem1, 0, &err);   //�ȴ��¼���������������
}

void RMTaskSet() {
    // ��������ĸ���������N��C��T��
    rm_task *rmTask;
    int i, j, n, t, c, min;
    printf("enter number of task\n");
    scanf("%d", &n);
    rmTask = (rm_task *) calloc(n, sizeof(rm_task) * n);
    for (i = 0; i < n; i++) {
        printf("enter task C and T\n");
        scanf("%d%d", &c, &t);
        rmTask[i].C = c;
        rmTask[i].T = t;
    }

    // select sort
    for (i = 0; i < n; ++i) {
        min = i;
        for (j = i + 1; j < n; ++j) {
            if (rmTask[j].T < rmTask[min].T) {
                min = j;
            }
        }
        if (min != i) {
            rm_task temp;
            temp.T = rmTask[i].T;
            temp.C = rmTask[i].C;

            rmTask[i].T = rmTask[min].T;
            rmTask[i].C = rmTask[min].C;

            rmTask[min].T = temp.T;
            rmTask[min].C = temp.C;
        }
    }

    // RM priority allocate
    for (i = 0; i < n; ++i) {
        rmTask[i].prior = 5 + i;
        printf("%d���� %d %d %d\n", i, rmTask[i].T, rmTask[i].C, rmTask[i].prior);
    }

    // create RM task;
    for (i = 0; i < n; i++) {
        OSTaskCreate(RMTask1, rmTask, &TaskStk[rmTask[i].prior][TASK_STK_SIZE - 1], rmTask[i].prior);
    }
}

void RMTask1(void *pParam) {
    int i, j, k, t, c, p, start_time, cRest;
    rm_task *all_task = (rm_task *) pParam;
    rm_task task = all_task[OSPrioCur - 5];
    printf("���� C T prior %d %d %d\n", task.C, task.T, task.prior);
    t = task.T;
    c = task.C;
    p = task.prior;
    cRest = c;
    OSTimeDly(20);
    while (1) {
        if (cRest > 0) {
            OSTimeTick();
            cRest--;
            printf("===time:%d===, taskPri=%d, ��ִ��%d�룬����ִ��%d��\n", OSTimeGet(), p, c - cRest, cRest);
        }
        // cRest����Ϊ0ʱ��dealy��ǰ��������һ�����ڵ�ʱ�䡣
        k = (OSTimeGet() - 20) % t;
        if (cRest == 0) {
            if (k == 0) {
                //do nothing
            } else {
                j = t - k;
                printf("===time:%d===, taskPri=%d, ִ�н�����delay %d��\n", OSTimeGet(), p, j);
                OSTimeDly(j);
            }
            cRest = c;
        }
        for (i = 0; i < 3; i++) {
            if ((OSTimeGet() - 20) % all_task[i].T == 0 && (i < (OSPrioCur - 5))) {
                printf("===time:%d===, taskPri=%d, ������ռ����.\n", OSTimeGet(), i + 5);
                OSTimeDly(1);
            }
        }
    }
}