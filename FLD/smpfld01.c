/*****************************************************************************
 *      ソフトウェアライブラリ
 *
 *      Copyright (c) 1994 SEGA
 *
 * Library: ファイルローダ
 * Module : サンプル１
 * File   : smpfld01.c
 * Date   : 1995-10-02
 * Version: 1.00
 * Auther : N.K
 *
 *      ゲームの単純な起動。
 *
 *****************************************************************************/

/*****************************************************************************
 *      インクルードファイル
 *****************************************************************************/


/*****************************************************************************/
#include <machine.h>

#include  <sl_def.h>
#include  <fld_load.h>
#include  <sega_def.h>
#include  <sega_gfs.h>
#include  <sega_int.h>
#include  <sega_scl.h>
#include  <sega_per.h>
#include    "per_x.h"
//#include    "string.h"

#define TITI 1
#define BITMAP_PIXEL_WIDTH  352
#define BITMAP_PIXEL_HEIGHT 240
#define	NBG1_VDP2_VRAM_BASE_ADDR	SCL_VDP2_VRAM_B0
#define GAME_BY_PAGE 7

unsigned char selected = 0;
unsigned char col[4]={0,1,2,3};
extern Uint8	FntAsciiFontData2bpp[1600];

/*****************************************************************************
 *      定数マクロ
 *****************************************************************************/
/* 同時にオープンするファイルの数 */
#define MAX_OPEN        4

/* ルートディレクトリにあるファイル数 */
#define MAX_DIR        20

/*****************************************************************************
 *      関数の宣言
 *****************************************************************************/
//extern	void	exit(Sint32);
/*****************************************************************************
 *      変数の定義
 *****************************************************************************/
/* ライブラリ作業領域 */
Uint32 lib_work[GFS_WORK_SIZE(MAX_OPEN) / sizeof(Uint32)];

/* ディレクトリ情報管理領域 */
GfsDirTbl dirtbl; 

/* ディレクトリ情報格納領域 */
GfsDirName dirname[MAX_DIR];

SysPort		*__port;
//const SysDevice	*device;


/*****************************************************************************
 *      関数の定義
 *****************************************************************************/
#if 1
void	UsrVblankIn( void ){
	SCL_VblankStart();
}


void   UsrVblankOut( void ){
	SCL_VblankEnd();
	PER_GetPort(__port);
}

void	SetVblank( void ){
	
	/* V-Blank割り込みルーチンの登録 */
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_SetScuFunc(INT_SCU_VBLK_IN,UsrVblankIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT,UsrVblankOut);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);

	__port = PER_OpenPort();
}

void	ClrVram(volatile Uint8 *buff)
{
	Uint32	i;

	for(i=0;i<(512 * 512);i++)	buff[i]=0x00;
}

#endif

#ifdef TITI
//--------------------------------------------------------------------------------------------------------------------------------------
int LoadFile(char *name, Uint8 *location, Uint32 size)
{
	Sint32 		fid, i;

	for (i = 0; i < 10; i++) {
		fid = GFS_NameToId(name);
		if (fid >= 0) {
			GFS_Load(fid, 0, location, size);
			return 0;
		}
	}
	return -1;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void load_color_proms(char *name,unsigned short *palette)
{
	int i;
	unsigned char buffer[513];

	if (LoadFile(name, &buffer[0x000], 512)==0)
	{
		for(i=1;i<513;i+=2)
		{
			palette[(i-1)/2] = (buffer[i]>> 0) + (buffer[i-1] << 8);
		}
	}
}
//--------------------------------------------------------------------------------------------------------------
void VDP2_InitVRAM(void)
{
	/* Variables. */

	Uint32	loop;
	Uint32	maxloop;

	/* Code. */

	/* Fascist clear of all VDP2 VRAM */

	loop = 0;
	maxloop = 512*1024;

	while (loop < maxloop)
	{
		*((Uint32 *) (SCL_VDP2_VRAM_A0 + loop)) = 0;

		loop += 4;
	}

	/* Now set layer NBG1 to be a nice blue colour */

	loop = 0;
	maxloop = BITMAP_PIXEL_WIDTH * BITMAP_PIXEL_HEIGHT;

	while (loop < maxloop)
	{
		*((Uint8 *) (NBG1_VDP2_VRAM_BASE_ADDR + loop)) = 1;

		loop += 1;
	}
	
}
//--------------------------------------------------------------------------------------------------------------
#endif

int main()
{
    Sint32 ret;
	Sint32 tno = 2;				// ＣＤ－ＤＡの先頭トラック番号 
//	Uint16	dummy_pad = 0;
//	int i = 0;
//	Uint16		BackCol;
//	SclConfig	Nbg0Scfg;
    SclVramConfig	vramConfig;
	
Uint16			cyclePat[] =
      			{
     				0x4444, 0xeeee,		// NBG0 - 512x512 bitmap - 256 colours - bank A 
					0x4444, 0xeeee,
					0x5555, 0xeeee,		// NBG1 - 512x512 bitmap - 256 colours - bank B 
					0x5555, 0xeeee					
				};

	SclConfig	config;

    /* ディレクトリ情報管理領域のsetup */
//	*((Uint32 *)GFS_DDS_ADDR) = 0x20202020;	
    FLD_INIT_DDS(); // equivalent a la ligne du dessus
    GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
    GFS_DIRTBL_DIRNAME(&dirtbl) = dirname;
    GFS_DIRTBL_NDIR(&dirtbl) = MAX_DIR;

    /* ファイルシステムの初期化 */
    FLD_INIT_DDS();
    ret = GFS_Init(MAX_OPEN, lib_work, &dirtbl);
    if (ret <= 0) {
		/*	失敗すると、自分自身を起動する */
		SYS_Exit(0);
    }
#if 1
	*(volatile Uint16 *)0x25F80000 &= 0x7fff;
		
	set_imask(0);
	
    SCL_Vdp2Init();
   	SCL_SetColRamMode(SCL_CRM15_1024);
	SetVblank();

    SCL_SetFrameInterval(-1);

    SCL_DisplayFrame();

//	(void)LoadFile("MENU.BIN",(unsigned char *)&bitmap_data[0] , sizeof(bitmap_data));
//	load_color_proms("MENUPAL.BIN",&palette[0]);
	
//    SCL_AllocColRam(SCL_NBG0,256,OFF); 
//   	SCL_SetColRam(SCL_NBG0,0,256,palette);
	
	SCL_SetPriority(SCL_NBG1,5);
	SCL_SetPriority(SCL_NBG0,6);	

   	Uint16 BackCol = 0x0000;
	SCL_SetBack(SCL_VDP2_VRAM+0x80000-2,1,&BackCol);

	/* Configure VDP2 VRAM */
	SCL_InitVramConfigTb(&vramConfig);
//	vramConfig.vramModeA = ON;
//	vramConfig.vramModeB = ON;
//	SCL_SetVramConfig(&vramConfig);

	SCL_InitConfigTb(&config);
	config.dispenbl 	 = ON;
	config.bmpsize 		 = SCL_BMP_SIZE_512X512;
	config.coltype 		 = SCL_COL_TYPE_256;
	config.datatype 	 = SCL_BITMAP;
	config.mapover       = SCL_OVER_0;
	config.plate_addr[0] = SCL_VDP2_VRAM_A0;
	SCL_SetConfig(SCL_NBG0, &config);

	SCL_InitConfigTb(&config);
	config.dispenbl 	 = ON;
	config.bmpsize 		 = SCL_BMP_SIZE_512X512;
	config.coltype 		 = SCL_COL_TYPE_16;
	config.datatype 	 = SCL_BITMAP;
	config.mapover       = SCL_OVER_0;
	config.plate_addr[0] = SCL_VDP2_VRAM_B0;
	SCL_SetConfig(SCL_NBG1, &config);

	/*	VRAM Access Pattern Set            */
	SCL_SetCycleTable(cyclePat);	
	

	SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_B); //normalA 320 normalB 352
	
	SCL_SetFrameInterval(1);	
	VDP2_InitVRAM();

	SCL_SET_N0PRIN(2);
    SCL_SET_N1PRIN(3);
	
	PER_SMPC_RES_ENA();/* リセットボタン有効 */

    changePic("MULTI");

	ClrVram((volatile Uint8 *)SCL_VDP2_VRAM_B0);

unsigned short palette[16]=
{
RGB( 31, 31, 31),RGB( 0,0,0 ),RGB( 31, 31, 31),RGB( 221>>3, 220>>3, 219>>3 ),
RGB( 164>>3, 247>>3, 197>>3 ),RGB( 82>>3,181>>2,132>>3 ),RGB( 241>>3, 192>>3, 0>>3 ),RGB( 252>>3, 201>>3, 0>>3 ),
RGB( 254>>3, 0>>3, 254>>3),RGB( 0,0,0 ),RGB( 254>>3, 0>>3, 254>>3),RGB( 221>>3, 220>>3, 219>>3 ),
RGB( 164>>3, 247>>3, 197>>3 )/*bordure*/,RGB( 0>>3,0>>2,0>>3 ),RGB( 82>>3, 181>>3, 132>>3 )/*haut*/,RGB( 164>>3, 247>>3, 197>>3 )/*couleur haut*/,
};
    SCL_AllocColRam(SCL_NBG1,16,OFF); 
   	SCL_SetColRam(SCL_NBG1,0,16,palette);
		
	GFS_Load(GFS_NameToId((Sint8*)"FONT.BIN"),0,(void *)&FntAsciiFontData2bpp[0],1600);
#endif	
	display_menu();
    return 0;
}

#if 1
//---------------------------------------------------------------------------------------------
void changePic(char *fileName)
{
   	unsigned short palette[256];
	Uint8 bitmap_data[BITMAP_PIXEL_WIDTH * BITMAP_PIXEL_HEIGHT];
	Uint8 *VramWorkP;
	int l=0,x=0,j=0,y=0;
	char xx[50];
	
//---------------------------------------------------------------------------------------------
    sprintf(xx,"%s.BIN",fileName);
	(void)LoadFile(xx,(unsigned char *)&bitmap_data[0] , sizeof(bitmap_data));
    sprintf(xx,"%sPAL.BIN",fileName);
	load_color_proms(xx,&palette[0]);
	
    SCL_AllocColRam(SCL_NBG0,256,OFF); 
   	SCL_SetColRam(SCL_NBG0,0,256,palette);
   	
	VramWorkP = (Uint8 *)SCL_VDP2_VRAM_A0;
	l=0;
	j=0;
	
    for (y = 0; y < BITMAP_PIXEL_HEIGHT; y++)
    {
        for (x = 0; x < BITMAP_PIXEL_WIDTH; x++, l++,j++)
        {
            VramWorkP[l] = bitmap_data[j];
        }
        l += 512-BITMAP_PIXEL_WIDTH;   
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
static unsigned char update_input(unsigned char *modified)
{
trigger_t	pltrigger[2],pltriggerE[2];
	
	Uint16 pad_asign[]={
		PER_DGT_U,PER_DGT_D,PER_DGT_R,PER_DGT_L,
		PER_DGT_A,PER_DGT_B,PER_DGT_C,PER_DGT_S,
	};

	unsigned int i=0;
	SysDevice	*device;

	if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
	{
		pltriggerE[0] = pltrigger[0];
		pltrigger[0]  = PER_GetTrigger( device );
		pltriggerE[0] = (pltrigger[0]) ^ (pltriggerE[0]);
		pltriggerE[0] = (pltrigger[0]) & (pltriggerE[0]);

		for(i=0;i<8;i++)
		{
			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltriggerE[0] & pad_asign[i] )
				{
				/*	case PER_DGT_A: 
					load_img(1);
					*modified = 1;
					break;
					case PER_DGT_C: 
					load_img(2);
					*modified = 1;
					break;*/

					case PER_DGT_D: 
					if(selected < GAME_BY_PAGE-1) selected++;
					*modified = 1;
					break;

					case PER_DGT_U:
					if(selected > 0)	selected--;
					*modified = 1;
					break;
/*
					case PER_DGT_L: 
					if(*current_page > 1) selected = (--(*current_page)-1) * GAME_BY_PAGE;
					*modified = 1;
					break;
					case PER_DGT_R: 
					if(*current_page * GAME_BY_PAGE  < nBurnDrvCount) selected = (++(*current_page)-1) * GAME_BY_PAGE;
					*modified = 1;
					break;*/

					case PER_DGT_S:

						return 1;


					break;

					default:
					break;
				}
			}
		}
	}
	return 0;
}
//--------------------------------------------------------------------------------------------------------------
#endif

//--------------------------------------------------------------------------------------------------------------------------------------
void ChangeDir(char *dir_name)
{
#ifndef ACTION_REPLAY	
    Sint32 fid;
	GfsDirTbl dirtbl; 
	
    fid = GFS_NameToId((Sint8 *)dir_name);

	GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dirtbl) = dirname;
	GFS_DIRTBL_NDIR(&dirtbl) = MAX_DIR;

	GFS_LoadDir(fid,&dirtbl);
//	GFS_SetDir(&dirtbl) ;
	for (;;) {
		if (GFS_SetDir(&dirtbl) == 0) {
			break;
		}				 
	}	 
#endif	
}
//--------------------------------------------------------------------------------------------------------------------------------------


void display_menu(void)
{
unsigned char pGames[GAME_BY_PAGE][40]={
	"Wolfenstein 3D Shareware",
	"Wolfenstein 3D FULL",
	"Spear Of Destiny : Missions 1",
	"Spear Of Destiny : Missions 2",
	"Spear Of Destiny : Missions 3",
	"XMas Wolf",
	"Burgerstein"};

	unsigned int l;
	unsigned int m;
	unsigned char modified = 1;
	unsigned char ret = 1;
	char FOLDERNAME[20], FNAME[20];
	Sint32 tno = 2;				// ＣＤ－ＤＡ
	
	char game_name[40];

	do
	{
		m=80;

		if(modified==1)
		{
			for (l=0;l<GAME_BY_PAGE ;l++ )
			{
				sprintf(game_name,"%-38s",pGames[l]);
				if(l==selected)	 FNT_Print256_2bppSel((volatile Uint8 *)SCL_VDP2_VRAM_B0,(Uint8 *)game_name,20,m);	  //12+m
				else			 FNT_Print256_2bpp   ((volatile Uint8 *)SCL_VDP2_VRAM_B0,(Uint8 *)game_name,20,m);
				m+=12;
			}
			modified=0;
		}
		ret = update_input(&modified);
		SCL_DisplayFrame();		
	}while(!ret);

/*
    SCL_SetFrameInterval(-1);

    SCL_DisplayFrame();
*/	
	sprintf(FOLDERNAME,"G%02d",selected+1);
	sprintf(FNAME,"G%02dIP.BIN",selected+1);
	
	ClrVram((volatile Uint8 *)SCL_VDP2_VRAM_A0);
	ClrVram((volatile Uint8 *)SCL_VDP2_VRAM_B0);						
	set_imask(0);

//	SCL_InitVramConfigTb(&vramConfig);
//	SCL_SetConfig(SCL_NBG1, &config);	
	VDP2_InitVRAM();
	
    SCL_SetFrameInterval(-1);
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT  |INT_MSK_PAD);
	
//  Sint32 FLD_ExecGame(Sint8 *ip_name, Sint8 *dir_name, Sint32 tno);
//	ChangeDir(FOLDERNAME);
	ret = FLD_ExecGame(FNAME,FOLDERNAME,tno);

	if (ret == NG) {
	SetVblank();

	ClrVram((volatile Uint8 *)SCL_VDP2_VRAM_B0);
	FNT_Print256_2bpp((volatile Uint8 *)SCL_VDP2_VRAM_B0,
		(Uint8 *)"Boot Menu -Error",40,10,2,0);
	}	
}
//-------------------------------------------------------------------------------------------------------------------------------------

