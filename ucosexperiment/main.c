/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*********************************************************************************************************
*/
/*
for book:嵌入式实时操作系统μCOS原理与实践
卢有亮
2011于成都电子科技大学
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
//    // 周期
//    INT32U T;
//    // 执行时间
//    INT32U C;
//    // 绝对截止时间
//    INT32U ADL;
//    // 相对截止时间
//    INT32U RDL;
//    // 到达时间
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
HANDLE mainhandle;        //主线程句柄
CONTEXT Context;        //主线程切换上下文
BOOLEAN FlagEn = 1;        //增加一个全局变量，做为是否时钟调度的标志


void TaskStart(void *pParam);
//void Task1(void * pParam) ;                            /* Function prototypes of tasks                  */

void VCInit(void);                        //初始化相关变量,一定需要

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
    printf("0.没有用户任务\n");
    printf("1.第一个例子，一个用户任务\n");
    printf("2.第二个例子，两个任务共享CPU交替运行\n");
    printf("3.第三个例子，任务的挂起和恢复\n");
    printf("4.第四个例子，信号量管理\n");
    printf("5.第五个例子，互斥信号量管理\n");
    printf("6.第六个例子，事件标志组\n");
    printf("7.第七个例子，消息邮箱\n");
    printf("8.第八个例子，消息队列\n");
    printf("9.第九个例子，内存管理\n");
    printf("10.第10个例子，131-7\n");
    printf("11.第11个例子，131-8\n");
    printf("12.第12个例子，131-9\n");
    printf("13.RM RM RM\n");
    printf("14.EDF EDF EDF\n");

    printf("请输入序号选择例子:\n");
    scanf("%d", &Experiment);
    if ((Experiment < 0) || (Experiment > 100)) {
        printf("无效的输入!");
        return (1);
    }
    OSInit();
    OSTaskCreate(TaskStart, 0, &TaskStk[1][TASK_STK_SIZE - 1], TaskStart_Prio);
    switch (Experiment) {
        case 1://一个任务运行?
            OSTaskCreate(FirstTask, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
            break;
        case 2://两个任务共享CPU
            OSTaskCreate(E2_task1, 0, &TaskStk[5][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(E2_task2, 0, &TaskStk[6][TASK_STK_SIZE - 1], 5);
            break;
        case 3://任务的挂起和恢复
            OSTaskCreate(E3_task0, 0, &TaskStk[5][TASK_STK_SIZE - 1], 7);
            OSTaskCreate(E3_task1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(E3_task2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 5);
            break;
        case 4://信号量管理例程
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
        case 5://互斥信号量管理例程
            OSTaskCreate(TaskMutex1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskMutex2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 50);
            OSTaskCreate(TaskPrint, 0, &TaskStk[8][TASK_STK_SIZE - 1], 30);
            break;
        case 6://时间标志组管理例程
            OSTaskCreate(TaskDataProcess, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
            OSTaskCreate(TaskIO1, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskIO2, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
            OSTaskCreate(TaskIO3, 0, &TaskStk[8][TASK_STK_SIZE - 1], 8);
            OSTaskCreate(TaskIO4, 0, &TaskStk[9][TASK_STK_SIZE - 1], 9);
            break;
        case 7://消息邮箱
            OSTaskCreate(TaskMessageSen, 0, &TaskStk[6][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskMessageRec, 0, &TaskStk[7][TASK_STK_SIZE - 1], 7);
            break;
        case 8://消息队列
            OSTaskCreate(TaskQSen, 0, &TaskStk[7][TASK_STK_SIZE - 1], 5);
            OSTaskCreate(TaskQRec, 0, &TaskStk[8][TASK_STK_SIZE - 1], 6);
            OSTaskCreate(TaskQRec, 0, &TaskStk[9][TASK_STK_SIZE - 1], 7);
            break;
        case 9://内存管理
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
    cp = GetCurrentProcess();    //得到当前进程句柄
    ct = GetCurrentThread();    //得到当前线程伪句柄
    DuplicateHandle(cp, ct, cp, &mainhandle, 0, TRUE, 2);    //伪句柄转换,得到线程真句柄

}

void TaskStart(void *pParam) {
    char err;
    OS_EVENT *sem1;

    /*模拟设置定时器中断。开启一个定时器线程,每秒中断100次,中断服务程序OSTickISRuser*/
    timeSetEvent(1000 / OS_TICKS_PER_SEC, 0, OSTickISRuser, 0, TIME_PERIODIC);
    OSStatInit(); /*统计任务初始化*/
    sem1 = OSSemCreate(0);
    OSSemPend(sem1, 0, &err);   //等待事件发生，被阻塞；
}

void RMTaskSet() {
    // 输入任务的各个参数，N、C、T等
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
        printf("%d任务 %d %d %d\n", i, rmTask[i].T, rmTask[i].C, rmTask[i].prior);
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
    printf("任务 C T prior %d %d %d\n", task.C, task.T, task.prior);
    t = task.T;
    c = task.C;
    p = task.prior;
    cRest = c;
    OSTimeDly(20);
    while (1) {
        if (cRest > 0) {
            OSTimeTick();
            cRest--;
            printf("===time:%d===, taskPri=%d, 已执行%d秒，还需执行%d秒\n", OSTimeGet(), p, c - cRest, cRest);
        }
        // cRest减至为0时，dealy当前任务至下一个周期点时间。
        k = (OSTimeGet() - 20) % t;
        if (cRest == 0) {
            if (k == 0) {
                //do nothing
            } else {
                j = t - k;
                printf("===time:%d===, taskPri=%d, 执行结束，delay %d秒\n", OSTimeGet(), p, j);
                OSTimeDly(j);
            }
            cRest = c;
        }
        for (i = 0; i < 3; i++) {
            if ((OSTimeGet() - 20) % all_task[i].T == 0 && (i < (OSPrioCur - 5))) {
                printf("===time:%d===, taskPri=%d, 进行抢占调度.\n", OSTimeGet(), i + 5);
                OSTimeDly(1);
            }
        }
    }
}