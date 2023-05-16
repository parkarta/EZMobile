/*
*********************************************************************
*
*	Model definitions for MoBy of different models
*
*********************************************************************
* FileName:        Models.h
* Dependencies:    None
* Processor:       PIC24F, PIC24H, dsPIC30F, dsPIC33F
* Compiler:        Microchip XC16 v1.11 or higher
* Company:
*
*********************************************************************
* Author               Date		Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Alexey Litvinenko	23.10.2013	Initial release
*********************************************************************
*/
#pragma once
#ifndef MODELS_H
#define MODELS_H

#define	DEV_MOBY			0x01
#define	CODE_MOBY_S			0x01
#define CODE_MOBY_SP		0x11
#define	CODE_MOBY_SG		0x05
#define CODE_MOBY_SGE		0x0D
#define	CODE_MOBY_U			0x02
#define	CODE_MOBY_UG		0x06
#define	CODE_MOBY_UGE		0x0E
#define CODE_MOBY_UGC		0x46
#define CODE_MOBY_UEC		0x4E
#define	CODE_MOBY_R			0x21
#define	CODE_MOBY_RG		0x25
#define	CODE_MOBY_RGE		0x2D
#define UNKNOWN_CODE		0xFF

#define sMOBY_FLAG			0x01
#define uMOBY_FLAG			0x02
#define cMOBY_FLAG			0x40

#if defined(MOBY_SP) || defined(MOBY_SGE) || defined(MOBY_SG) || defined(MOBY_S) || defined(MOBY_R) || defined(MOBY_RG) || defined(MOBY_RGE)
  #define MOBY				1
  #define sMOBY				1
#if 0
  #if !defined(SAMD20)
	#define SAMD20			1
  #endif
  #if !defined(SAMD2x)
	#define SAMD2x			1
  #endif
#endif
  #if defined(MOBY_SP)
	#define pMOBY			1
	#define INPUTs			0
	#define PGMs			0
  #endif
  #if defined(MOBY_SGE) || defined(MOBY_SG) || defined(MOBY_S)
	#define INPUTs			2
	#define PGMs			4
  #endif
  #if defined(MOBY_R) || defined(MOBY_RG) || defined(MOBY_RGE)
	#define rMOBY			1
	#define INPUTs			3
	#define PGMs			3
  #endif

#elif defined(MOBY_U) || defined(MOBY_UG) || defined(MOBY_UGE) || defined(MOBY_UEC) || defined(MOBY_UGC)
  #define MOBY				1
  #define uMOBY				1
  #define PANEL_LED			1
#if 0
  #if !defined(SAMx5x)
	#define SAMx5x			1
  #endif
  #if (REV <= 1)
	#define SAMD5x			1
  #endif
#endif
  #if !defined(MOBY_UGC)
	#define ETHERNET_USED	1
  #endif
  #if defined(MOBY_UEC) || defined(MOBY_UGC)
	#define cMOBY			1
	#define INPUTs			3
	#define PGMs			3
	#if defined(MOBY_UGC)
	  #define USB_USED		1
	#endif
  #elif defined(MOBY_U) || defined(MOBY_UG) || defined(MOBY_UGE)
	#define LINE_CONTROL	1
	#define INPUTs			2
	#define PGMs			4
  #endif

#endif

#if defined(MOBY_SP) || defined(MOBY_SGE) || defined(MOBY_UG) || defined(MOBY_UGE) || defined(MOBY_RG) || defined(MOBY_RGE) || defined(MOBY_UEC) || defined(MOBY_UGC)
  #define GSM_MODULE		1
  #if defined(MOBY_SP) || defined(MOBY_SGE) || defined(MOBY_UGE) || defined(MOBY_RGE) || defined(MOBY_UEC)
	#define EXT_MODULE		1
  #endif
  #if !defined(BOOTLOADER)
	#define GSM				1
  #endif
#endif

#endif	// MODELS_H
