/***************************************************************************************************
* Copyright (C)2024, EN+.
* File Name                             :   pile_cn_gate_rcd_rpt.h
* Description                           :   国标充电桩实现 内网协议用户程序部分实现 之 充电记录上报
* Version                               :   
* Author                                :   Hall
* Creat Date                            :   2024-05-31
* notice                                :   
****************************************************************************************************/
#ifndef _pile_cn_gate_rcd_rpt_h_
#define _pile_cn_gate_rcd_rpt_h_













































extern void sPileCnGateNewRcdRpt(ePileGunIndex_t eGunIndex);
extern void sPileCnGateOldRcdRpt(ePileGunIndex_t eGunIndex);
extern void sPileCnGateRptAck   (ePileGunIndex_t eGunIndex);

extern void sPileCnGateNewRcdInvalid    (ePileGunIndex_t eGunIndex);
extern void sPileCnGateNewRcdInitForPtc (ePileGunIndex_t eGunIndex);
extern void sPileCnGateNewRcdInitForRmt (ePileGunIndex_t eGunIndex, stPrivDrvCmdA6_t *pRmt,         stPrivDrvCmdA7_t *pTransAck);
extern void sPileCnGateNewRcdInitForCard(ePileGunIndex_t eGunIndex, stPrivDrvCmdA5_t *pCardAuthAck, stPrivDrvCmdA7_t *pTransAck);
extern void sPileCnGateNewRcdInitForVin (ePileGunIndex_t eGunIndex, stPrivDrvCmdA5_t *pVinAuthAck,  stPrivDrvCmdA7_t *pTransAck);

extern i32  sPileCnGateGetSoc(ePileGunIndex_t eGunIndex);



#endif
















