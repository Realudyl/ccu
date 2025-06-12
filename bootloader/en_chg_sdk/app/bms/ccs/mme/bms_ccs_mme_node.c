/***************************************************************************************************
* Copyright (C)2022, EN+.
* File Name                             :   bms_ccs_mme_node.c
* Description                           :   SLAC 匹配过程中 节点管理实现
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2022-06-25
* notice                                :   从联芯通<HLE_SDK_v1.1>工程移植过来 再做命名和排版优化
****************************************************************************************************/
/*  plc hle utils project {{{
 *
 * Copyright (C) 2018 MStar Semiconductor
 *
 * <<<Licence>>>
 *
 * }}} */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "en_common.h"
#include "en_log.h"
#include "en_mem.h"

#include "bms_ccs_mme_node.h"

#include "bms_ccs_mme_slac_param.h"






//设定本文件的日志打印等级和文件标签
#define cLogLevel eEnLogLevelDebug
static const char *TAG = "bms_ccs_mme_node";





pev_item *create_pev_item(void *pMmeDev, uint8_t *run_id, uint8_t *mac)
{
    pev_item *new_node = (pev_item *)MALLOC(sizeof (pev_item));
    new_node->i32SoundsRx = 0;
    new_node->i32SlacState = eRecvCmSlacParmReq;
    new_node->i32AttenCharIndRetryCnt = C_EV_match_retry;
    new_node->i32TtMatchJoinWaitCnt = 0;
    new_node->bSlacMatchFlag = false;
    
    memcpy(new_node->u8RunId,     run_id, sizeof(new_node->u8RunId    ));
    memcpy(new_node->u8MacEvHost, mac,    sizeof(new_node->u8MacEvHost));
    memset(new_node->AAG, 0, sizeof(new_node->AAG));
    memcpy(&new_node->stMmeDev, (stMmeDev_t *)pMmeDev, sizeof(stMmeDev_t));
    new_node->xEvtSlac = xEventGroupCreate();
    
    new_node->next = NULL;
    
    return new_node;
}







void insert_pev_item(void *pMmeDev, struct pev_item *head, uint8_t *run_id, uint8_t *mac)
{
    pev_item *tmp = create_pev_item(pMmeDev, run_id, mac);
    pev_item *current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = tmp;
}






/***************************************************************************************************
* Description                           :   MME 节点组件 之 删除一个pev节点
* Author                                :   Hall
* Creat Date                            :   2024-01-03
* notice                                :   
****************************************************************************************************/
void delete_pev_item(struct pev_item *head, struct pev_item *pPev)
{
    char u8Head[32];
    pev_item *current = head;
    
    while (current->next != NULL)
    {
        if(current->next == pPev)
        {
            EN_SLOGI(TAG, "删除当前 pev 列表的节点:");
            memset(u8Head, 0, sizeof(u8Head));
            snprintf(u8Head, sizeof(u8Head), "pev->MAC addr = ");
            EN_HLOGI(TAG, u8Head, pPev->u8MacEvHost, sizeof(pPev->u8MacEvHost));
            
            memset(u8Head, 0, sizeof(u8Head));
            snprintf(u8Head, sizeof(u8Head), "pev->run_id   = ");
            EN_HLOGI(TAG, u8Head, pPev->u8RunId, sizeof(pPev->u8RunId));
            
            vEventGroupDelete(pPev->xEvtSlac);
            
            current->next = pPev->next;
            FREE(pPev);
            pPev = NULL;
            break;
        }
        current = current->next;
    }
    
}








i32 print_pev_item_list(struct pev_item *head, bool bPrintFlag)
{
    i32  i32Num = 0;
    char u8Head[32];
    
    pev_item *pPev = head;
    
    EN_SLOGI(TAG, "当前 pev 列表如下:");
    while (pPev->next != NULL)
    {
        pPev = pPev->next;
        
        if(bPrintFlag == true)
        {
            memset(u8Head, 0, sizeof(u8Head));
            snprintf(u8Head, sizeof(u8Head), "pev[%d]->MAC addr = ", i32Num);
            EN_HLOGI(TAG, u8Head, pPev->u8MacEvHost, sizeof(pPev->u8MacEvHost));
            memset(u8Head, 0, sizeof(u8Head));
            snprintf(u8Head, sizeof(u8Head), "pev[%d]->run_id   = ", i32Num);
            EN_HLOGI(TAG, u8Head, pPev->u8RunId, sizeof(pPev->u8RunId));
            EN_SLOGI(TAG, "\n");
        }
        
        i32Num++;
    }
    
    return(i32Num);
}







void update_pev_item(void *pMmeDev, pev_item *pev, uint8_t *run_id, uint8_t *mac)
{
    pev->i32SoundsRx = 0;
    pev->i32SlacState = eRecvCmSlacParmReq;
    pev->i32AttenCharIndRetryCnt = C_EV_match_retry;
    pev->i32TtMatchJoinWaitCnt = 0;
    pev->bSlacMatchFlag = false;
    
    memcpy(pev->u8RunId,     run_id, sizeof(pev->u8RunId    ));
    memcpy(pev->u8MacEvHost, mac,    sizeof(pev->u8MacEvHost));
    memset(pev->AAG, 0, sizeof(pev->AAG));
    memcpy(&pev->stMmeDev, (stMmeDev_t *)pMmeDev, sizeof(stMmeDev_t));
    
    
}






pev_item *search_pev_item_list(struct pev_item *head, uint8_t *mac)
{
    pev_item *current = head;
    while(current->next != NULL)
    {
        current = current->next;
        
        if(memcmp(current->u8MacEvHost, mac, sizeof(current->u8MacEvHost)) == 0)
        {
            //EN_SLOGD(TAG, "PEV 节点匹配到");
            return current;
        }
    }
    EN_SLOGD(TAG, "PEV 节点未匹配到");
    
    return NULL;
}






/***************************************************************************************************
* Description                           :   MME 节点管理组件 之 寻找最旧的pev节点
* Author                                :   Hall
* Creat Date                            :   2024-01-06
* notice                                :   
****************************************************************************************************/
pev_item *search_pev_item_oldest(struct pev_item *head)
{
    pev_item *current = head;
    while(current->next != NULL)
    {
        current = current->next;
        return current;
    }
    EN_SLOGD(TAG, "PEV 未找到最旧的节点");
    
    return NULL;
}








