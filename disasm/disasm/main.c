// Free Disassembler and Assembler -- Demo program
//
// Copyright (C) 2001 Oleh Yuschuk
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#define STRICT
#define MAINPROG                       // Place all unique variables here


#include <windows.h>
#include <stdio.h>
//#include <string.h>
//#include <ctype.h>
//#include <dir.h>
//#include <math.h>
//#include <float.h>
#pragma hdrstop
#include "disasm.h"
//#include "asmserv.c"
//#include "assembl.c"
//#include "disasm.c"


/*
55 8B EC 51 C6 45 FF 00 50 64 A1 30 00 00 00 3E 8A 40 02 88 45 FF 58 80 7D FF 00 6A 00 6A 00 74
07 68 F8 20 D0 00 EB 05 68 FC 20 D0 00 6A 00 FF 15 34 20 D0 00 68 00 21 D0 00 FF 15 A8 20 D0 00
83 C4 04 33 C0 8B E5 5D C3

*/

void AnalyseAsm()
{
	int nBaseAddress = 0xa51000;
	int nIndex = 0;
	char szBuffer[] = "\x55\x8B\xEC\x51\xC6\x45\xFF\x00\x50\x64\xA1\x30\x00\x00\x00\x3E\x8A\x40\x02\x88\x45\xFF\x58\x80\x7D\xFF\x00\x6A\x00\x6A\x00\x74\x07\x68\xF8\x20\xD0\x00\xEB\x05\x68\xFC\x20\xD0\x00\x6A\x00\xFF\x15\x34\x20\xD0\x00\x68\x00\x21\xD0\x00\xFF\x15\xA8\x20\xD0\x00\x83\xC4\x04\x33\xC0\x8B\xE5\x5D\xC3";
	
	while (1)
	{
		if(nIndex >= sizeof(szBuffer)/sizeof(char))
			break;

		char* pBuf = szBuffer + nIndex;
			
		t_disasm stAsm;
		ulong nLen = Disasm(pBuf, 20, nBaseAddress + nIndex, &stAsm, DISASM_FILE);
		printf("%2i  %-20s  %-24s	%-20s\n", nLen, stAsm.uszDump, stAsm.uszResult,stAsm.uszComment);
		nIndex += nLen;
	}
	getchar();
}

void main(void) 
{                      // Old form. So what?
	AnalyseAsm();
	int i, j, n;
	ulong l;
	unsigned char *pasm;
	t_disasm da;
	t_asmmodel am;
	unsigned char s[TEXTLEN], errtext[TEXTLEN];

  memset(&da,0,sizeof(t_disasm));
  memset(&am,0,sizeof(t_asmmodel));
  // Demonstration of Disassembler.
  printf("Disassembler:\n");

  // Quickly determine size of command.
  l=Disasm("\x81\x05\xE0\x5A\x47\x00\x01\x00\x00\x00\x11\x22\x33\x44\x55\x66",
    10,0x400000,&da,DISASM_SIZE);
  printf("Size of command = %i bytes\n",l);

  // ADD [475AE0],1 MASM mode, lowercase, don't show default segment
  /*
  ideal=0; lowercase=1; putdefseg=0;
  l=Disasm("\x81\x05\xE0\x5A\x47\x00\x01\x00\x00\x00",
    10,0x400000,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   (MASM)\n",l,da.dump,da.result);
*/
	g_nIdeal=0; g_nLowerCase=1; g_nPutDefSeg=0;
	l=Disasm("\x8b\x5d\x0c\x29\x75\x10\x8d\x4e\x01\x00",
   9,0x40e9a5,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   (MASM)\n",l,da.uszDump,da.uszResult);

  // ADD [475AE0],1 IDEAL mode, uppercase, show default segment
  g_nIdeal=1; g_nLowerCase=0; g_nPutDefSeg=1;
  l=Disasm("\x81\x05\xE0\x5A\x47\x00\x01\x00\x00\x00",
    10,0x400000,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   (IDEAL)\n",l,da.uszDump,da.uszResult);

  // CALL 45187C
  l=Disasm("\xE8\x1F\x14\x00\x00",
    5,0x450458,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   jmpconst=%08X\n",l,da.uszDump,da.uszResult,da.ulJmpConst);

  // JNZ 450517
  l=Disasm("\x75\x72",
    2,0x4504A3,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   jmpconst=%08X\n",l,da.uszDump,da.uszResult,da.ulJmpConst);

  // Demonstration of Assembler.
  printf("\nAssembler:\n");

  // Assemble one of the commands above. First try form with 32-bit immediate.
  pasm="ADD [DWORD 475AE0],1";
  printf("%s:\n",pasm);
  j=Assemble(pasm,0x400000,&am,0,0,errtext);
  n=sprintf(s,"%3i  ",j);
  for (i=0; i<j; i++) n+=sprintf(s+n,"%02X ",am.uszCode[i]);
  if (j<=0) sprintf(s+n,"  error=\"%s\"",errtext);
  printf("%s\n",s);

  // Then variant with 8-bit immediate constant.
  j=Assemble(pasm,0x400000,&am,0,2,errtext);
  n=sprintf(s,"%3i  ",j);
  for (i=0; i<j; i++) n+=sprintf(s+n,"%02X ",am.uszCode[i]);
  if (j<=0) sprintf(s+n,"  error=\"%s\"",errtext);
  printf("%s\n",s);

  // Error, unable to determine size of operands.
  pasm="MOV [475AE0],1";
  printf("%s:\n",pasm);
  j=Assemble(pasm,0x400000,&am,0,4,errtext);
  n=sprintf(s,"%3i  ",j);
  for (i=0; i<j; i++) n+=sprintf(s+n,"%02X ",am.uszCode[i]);
  if (j<=0) sprintf(s+n,"  error=\"%s\"",errtext);
  printf("%s\n",s);

  // Show results.
  Sleep(10000);
};
