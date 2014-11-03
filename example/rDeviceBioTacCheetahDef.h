/* RoboticsLab, Copyright 2008-2011 SimLab Co., Ltd. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF SimLab Co., LTD.
 */

#ifndef __RDEVICEBIOTACCHEETAHDEF_H__
#define __RDEVICEBIOTACCHEETAHDEF_H__

#include <stdio.h>
#include <stdlib.h>

enum eBioTacDataChannel {
	BT_CHNL_Pac = 0,
	BT_CHNL_Pdc,
	BT_CHNL_Tac,
	BT_CHNL_Tdc,
	BT_CHNL_HallSensor = 15,
	BT_CHNL_Electrod1 = 17,
	BT_CHNL_Electrod2,
	BT_CHNL_Electrod3,
	BT_CHNL_Electrod4,
	BT_CHNL_Electrod5,
	BT_CHNL_Electrod6,
	BT_CHNL_Electrod7,
	BT_CHNL_Electrod8,
	BT_CHNL_Electrod9,
	BT_CHNL_Electrod10,
	BT_CHNL_Electrod11,
	BT_CHNL_Electrod12,
	BT_CHNL_Electrod13,
	BT_CHNL_Electrod14,
	BT_CHNL_Electrod15,
	BT_CHNL_Electrod16,
	BT_CHNL_Electrod17,
	BT_CHNL_Electrod18,
	BT_CHNL_Electrod19, //35
	BT_CHNL_COUNT
};

typedef struct
{
	double time;
	union
	{
		unsigned short word;
		unsigned char byte[2];
	} d;
} BioTac_Sample;

typedef struct
{
	BioTac_Sample sample[BT_CHNL_COUNT];
} BioTac_Frame;

typedef struct
{
	BioTac_Frame frame[5];
} BioTac_Data;

#endif// __RDEVICEBIOTACCHEETAHDEF_H__
