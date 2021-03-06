#include "../../MyApplications/Inc/static_queue.h"


/************************************************************************************************************
 *            C语言现实顺序列队数据结构
 *                  start
 ***********************************************************************************************************/
void QueueInit(static_queue* queue)
{
	queue->front = 0;
	queue->rear = 0;
}

/************************************************************************************************************
 *函数功能说明：入队操作
 *函数参数说明：
 * @pQ:指向队列的指针
 * @e:需要入队的元素
 *函数返回值：列队已满返回FALSE，否则返回TRUE
 *其他说明：无
 ***********************************************************************************************************/
uint8_t QueueEn(static_queue* queue, const Bluetooth_data* e)
{
    int temp = queue->rear;
    // 判断队列是否已满
    if (LOOP_FRONT_COUNTER(temp,QUEUE_MAX_SIZE) == queue->front)
        return false;

    queue->buff[queue->rear] = *e; // 将元素e赋值给队尾
    temp = LOOP_FRONT_COUNTER(queue->rear,QUEUE_MAX_SIZE); // rear指针向后移一位置，若到最后则转到数组头部
    queue->rear = temp;

    return true;
}

/************************************************************************************************************
 *函数功能说明：出队操作
 *函数参数说明：
 * @pQ:指向队列的指针
 * @e:需要出队的元素
 *函数返回值：列队为空返回FALSE，否则返回TRUE
 *其他说明：无
 ***********************************************************************************************************/
uint8_t QueueDe(static_queue* queue, Bluetooth_data* e)
{
    // 判断是否为空队
    if (queue->front == queue->rear) 
        return false;

    *e = queue->buff[queue->front]; // 将队头元素赋值给pE
    int temp = LOOP_FRONT_COUNTER(queue->front,QUEUE_MAX_SIZE); // front指针向后移一位置,若到最后则转到数组头部
    queue->front = temp;

    return true;
}

/************************************************************************************************************
 *函数功能说明：判断是否为空队列
 *函数参数说明：
 * @pQ:指向队列的指针
 *函数返回值：列队为空返回TRUE，否则返回FALSE
 *其他说明：无
 ***********************************************************************************************************/
uint8_t QueueIsEmpty(static_queue* queue)
{
    return queue->front == queue->rear ? true : false;
}
/************************************************************************************************************
 *            C++语言现实顺序列队数据结构
 *                   end
 ***********************************************************************************************************/
