// BioTac.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
extern "C" 
{
#include "cheetah.h"
#include "biotac.h"
}
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <process.h>
#include <stdlib.h>
#include <windows.h>
#include "rDeviceBioTacCheetahDef.h"

/************************************/
/* --- BioTac: Define variables --- */
/************************************/
bt_info biotac;
bt_property biotac_property[MAX_BIOTACS_PER_CHEETAH];
Cheetah ch_handle;
BioTac_Data* _data;

/************************************/
/* --- BioTac: Funtions         --- */
/************************************/
void PrintInstruction();
bool InitializeBioTac();
void TerminateBioTac();
void ProcessMessage(bt_data* data, int number_of_samples);
void DumpBioTacData();

/************************************/
/* --- BioTac: I/O thread       --- */
/************************************/
uintptr_t _ioThreadCheetah;
bool _ioThreadCheetahRun;
unsigned int __stdcall ioThreadCheetah(void* inst);

int _tmain(int argc, _TCHAR* argv[])
{
	//////////////////////////////////////////////////////////////
	// print out some instructions
	PrintInstruction();

	//////////////////////////////////////////////////////////////
	// initialize communication with BioTac sensors
	if (!InitializeBioTac()) {
		printf("failed to initialize BioTac sensor.\n");
		printf("Press any key to quit...");
		getch();
		return false;
	}
	
	//////////////////////////////////////////////////////////////
	// run main-loop to gather data from sensors
	bool bRun = true;
	while (bRun)
	{
		if (!_kbhit())
		{
			//fcu.Update();
#ifdef WIN32
			Sleep(100);
#else
			sleep(0.1);
#endif
		}
		else
		{
			int c = _getch();
			switch (c)
			{
			case 'p':
			case 'P':
				DumpBioTacData();
				break;
			case 'q':
			case 'Q':
				bRun = false;
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////
	// terminate program
	TerminateBioTac();
	printf("Press any key to quit...");
	getch();
	return 0;
}

void PrintInstruction()
{
	printf("********** BioTac sample program ********\n");
	printf("*                                       *\n");
	printf("*      Copyrights SimLab Co., Ltd.      *\n");
	printf("*                                       *\n");
	printf("*****************************************\n");
	printf("\n");
	printf("Press \'q\' to quit this program.\n");
	printf("Press \'p\' to print current sensor value.\n");
	printf("\n");
}

bool InitBioTacCheetah()
{
	BioTac bt_err_code;
	int i;

	/**************************************************************************/
	/* --- Initialize BioTac settings (only default values are supported) --- */
    /**************************************************************************/
	biotac.spi_clock_speed = BT_SPI_BITRATE_KHZ_DEFAULT;
	biotac.number_of_biotacs = 0;
	biotac.sample_rate_Hz = BT_SAMPLE_RATE_HZ_DEFAULT;
	biotac.frame.frame_type = 0;
	biotac.batch.batch_frame_count = BT_FRAMES_IN_BATCH_DEFAULT;
	//biotac.batch.batch_ms = BT_BATCH_MS_DEFAULT;
	biotac.batch.batch_ms = 10; // 10msec, 1 frame per 1 batch

	// Check if any initial settings are wrong
	if (MAX_BIOTACS_PER_CHEETAH != 3 && MAX_BIOTACS_PER_CHEETAH != 5)
	{
		bt_err_code = BT_WRONG_MAX_BIOTAC_NUMBER;
		bt_display_errors(bt_err_code);
		//exit(1);
		return false;
	}

    /******************************************/
	/* --- Initialize the Cheetah devices --- */
    /******************************************/
	ch_handle = bt_cheetah_initialize(&biotac);

	/*********************************************************/
	/* --- Get and print out properties of the BioTac(s) --- */
	/*********************************************************/
	for (i = 0; i < MAX_BIOTACS_PER_CHEETAH; i++)
	{
		bt_err_code = bt_cheetah_get_properties(ch_handle, i+1, &(biotac_property[i]));

		if (biotac_property[i].bt_connected == YES)
		{
			(biotac.number_of_biotacs)++;
		}

		if (bt_err_code)
		{
			bt_display_errors(bt_err_code);
			//exit(1);
			return false;
		}
	}

	// Check if any BioTacs are detected
	if (biotac.number_of_biotacs == 0)
	{
		bt_err_code = BT_NO_BIOTAC_DETECTED;
		bt_display_errors(bt_err_code);
		//return bt_err_code;
		return false;
	}
	else
	{
		printf("\n%d BioTac(s) detected.\n\n", biotac.number_of_biotacs);
	}

	return true;
}

void CloseBioTacCheetah()
{
	if (ch_handle) {
	    bt_cheetah_close(ch_handle);
		ch_handle = 0;
	}
}

bool InitializeBioTac()
{
	_data = new BioTac_Data;
	memset(&biotac, 0, sizeof(bt_info));
	memset(biotac_property, 0, sizeof(bt_property)*MAX_BIOTACS_PER_CHEETAH);
	
	if (!InitBioTacCheetah())
		return false;

	printf("starts listening data frames.\n");
	_ioThreadCheetahRun = true;
	_ioThreadCheetah = _beginthreadex(NULL, 0, ioThreadCheetah, NULL, 0, NULL);

	return true;
}
void TerminateBioTac()
{
	if (_ioThreadCheetahRun) {
		_ioThreadCheetahRun = false;
		WaitForSingleObject((HANDLE)_ioThreadCheetah, INFINITE);
		CloseHandle((HANDLE)_ioThreadCheetah);
		_ioThreadCheetah = 0;
		printf("stoped listening data frames.\n");
	}
	CloseBioTacCheetah();
	delete _data;
	_data = NULL;
}

void ProcessMessage(bt_data* data, int number_of_samples)
{
	int i;
	int si;

	for (si=0; si<number_of_samples; si++) {
		for (i=0; i<MAX_BIOTACS_PER_CHEETAH; i++) {
			_data->frame[i].sample[data[si].channel_id].time = data[si].time;
			_data->frame[i].sample[data[si].channel_id].d.word = data[si].d[i].word;
		}
	}
}

void DumpBioTacData()
{
	int port = 0;

	printf("\n\n");
	printf("=====================================================================\n");
	printf("Pac/Pdc/Tac/Tdc/HallSensor : ");	
	printf("%6d", _data->frame[port].sample[BT_CHNL_Pac].d.word);
	printf("%6d", _data->frame[port].sample[BT_CHNL_Pdc].d.word);
	printf("%6d", _data->frame[port].sample[BT_CHNL_Tac].d.word);
	printf("%6d", _data->frame[port].sample[BT_CHNL_Tdc].d.word);
	printf("%6d", _data->frame[port].sample[BT_CHNL_HallSensor].d.word);
	printf("\n");

	printf("Elec1 ~ Elec9    : ");
	for (int i=BT_CHNL_Electrod1; i<=BT_CHNL_Electrod9; i++)
		printf("%6d", _data->frame[port].sample[i].d.word);
	printf("\n");

	printf("Elec10 ~ Elec19  : ");
	for (int i=BT_CHNL_Electrod10; i<=BT_CHNL_Electrod19; i++)
		printf("%6d", _data->frame[port].sample[i].d.word);
	printf("\n");
}

unsigned int __stdcall ioThreadCheetah(void* inst)
{
	bt_data *data;
	BioTac bt_err_code;
	double length_of_data_in_second;
	int number_of_samples;
	int number_of_loops;
	int i;

	// Set the duration of the run time
	length_of_data_in_second = 0.01;
	number_of_samples = (int)(BT_SAMPLE_RATE_HZ_DEFAULT * length_of_data_in_second);

	/*************************************/
	/* --- Configure the save buffer --- */
	/*************************************/
	data = bt_configure_save_buffer(number_of_samples);

	/*******************************/
	/* --- Configure the batch --- */
	/*******************************/
	bt_err_code = bt_cheetah_configure_batch(ch_handle, &biotac, number_of_samples);
	if (bt_err_code < 0)
	{
		bt_display_errors(bt_err_code);
		exit(1);
	}
	else
	{
		printf("\nConfigured the batch\n");
	}

	/***************************************************************/
	/* --- Collect the batch and display the data (if desired) --- */
	/***************************************************************/
	number_of_loops = (int)(number_of_samples / ((double)(biotac.frame.frame_size * biotac.batch.batch_frame_count)));
	printf("Start collecting BioTac data for %.3f second(s)...\n", length_of_data_in_second);

	while (_ioThreadCheetahRun)
	{
		for (i = 0; i < number_of_loops; i++)
		{
			// To print out data on Terminal, set the fourth argument to YES (NO by default)
			bt_cheetah_collect_batch(ch_handle, &biotac, data, NO);
			ProcessMessage(data, number_of_samples);
		}
		bt_reset_save_buffer();
	}

	/***********************/
    /* --- Free memory --- */
	/***********************/
	free(data);

	return 0;
}
