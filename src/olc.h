/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  _TwyliteMud_ by Rv.                          Based on CircleMud3.0bpl9 *
*    				                                          *
*  OasisOLC - olc.h 		                                          *
*    				                                          *
*  Copyright 1996 Harvey Gilpin.                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*. Macros, defines, structs and globals for the OLC suite .*/

#ifndef __OLC_H__
#define __OLC_H__

/*. CONFIG DEFINES 
   - were moved to apropriate locations in structs.h, spells.h. */

/*. Utils exported from olc.c .*/
void strip_string(char *);
void cleanup_olc(struct descriptor_data *d, byte cleanup_type);
void get_char_cols(struct char_data *ch);
void olc_add_to_save_list(int zone, char type);
void olc_remove_from_save_list(int zone, char type);
int  real_zone(int number);
int  check_help(struct char_data* ch, char *arg, char *help_str);

void zedit_setup(struct descriptor_data *d, int room_num);
void zedit_save_to_disk(struct descriptor_data *d);
void zedit_new_zone(struct char_data *ch, int new_zone);
void medit_setup_new(struct descriptor_data *d);
void medit_setup_existing(struct descriptor_data *d, int rmob_num);
void medit_save_to_disk(struct descriptor_data *d);
void medit_free_progs(MPROG_DATA *d);
void redit_setup_new(struct descriptor_data *d);
void redit_setup_existing(struct descriptor_data *d, int rroom_num);
void redit_save_to_disk(struct descriptor_data *d);
void oedit_setup_new(struct descriptor_data *d);
void oedit_setup_existing(struct descriptor_data *d, int robj_num);
void oedit_save_to_disk(struct descriptor_data *d);
void sedit_setup_new(struct descriptor_data *d);
void sedit_setup_existing(struct descriptor_data *d, int robj_num);
void sedit_save_to_disk(struct descriptor_data *d);
void free_room(struct room_data *room);
void medit_free_mobile(struct char_data * mob);
void gedit_save_to_disk(struct descriptor_data *d);
void gedit_setup_existing(struct descriptor_data *d, int rgm_num);
void gedit_setup_new(struct descriptor_data *d);
MPROG_DATA *dup_mobprograms(MPROG_DATA *first);
long mob_account(struct char_data *mob);
int mob_exp(struct char_data *mob);
void perm_range_message(struct descriptor_data *d, int bottom, int top);
long room_account(struct room_data *room);

/*. OLC structs .*/

struct olc_data {
  int mode;
  int zone_num;
  int number;
  int value;
  struct char_data *mob;
  struct room_data *room;
  struct obj_data *obj;
  struct zone_data *zone;
  struct shop_data *shop;
  struct guild_master_data *guild;
  struct extra_descr_data *desc;
  struct clan_info *clan;
  MPROG_DATA *firstprog;
  MPROG_DATA *mprog;
  int spec_index;
  int last_mode;
  bool readonly;
  bool clan_edit;
};

struct olc_save_info {
  int zone;
  char type;
  struct olc_save_info *next;
};


/*. Exported globals .*/
#ifdef __OLC_C__
char *nrm, *grn, *cyn, *yel;
struct olc_save_info *olc_save_list = NULL;
#else
extern char *nrm, *grn, *cyn, *yel;
extern struct olc_save_info *olc_save_list;
#endif


/*. Descriptor access macros .*/
#define OLC_MODE(d) 	((d)->olc->mode) 	/*. Parse input mode	.*/
#define OLC_NUM(d) 	((d)->olc->number)	/*. Room/Obj VNUM 	.*/
#define OLC_VAL(d) 	((d)->olc->value)  	/*. Scratch variable	.*/
#define OLC_ZNUM(d) 	((d)->olc->zone_num) 	/*. Real zone number	.*/
#define OLC_ROOM(d) 	((d)->olc->room)	/*. Room structure	.*/
#define OLC_OBJ(d) 	((d)->olc->obj)	  	/*. Object structure	.*/
#define OLC_ZONE(d)     ((d)->olc->zone)	/*. Zone structure	.*/
#define OLC_MOB(d)	((d)->olc->mob)	  	/*. Mob structure	.*/
#define OLC_SHOP(d) 	((d)->olc->shop)	/*. Shop structure	.*/
#define OLC_DESC(d) 	((d)->olc->desc)	/*. Extra description	.*/
#define OLC_FPROG(d)	((d)->olc->firstprog)	/*. First mob program	.*/
#define OLC_PROG(d)	((d)->olc->mprog)	/*. Current Mob program	.*/
#define OLC_GUILD(d)	((d)->olc->guild)	/*. Edited Guild	.*/
#define OLC_SPEC(d)	((d)->olc->spec_index)	/*. SpecProc index	.*/
#define OLC_LAST(d)	((d)->olc->last_mode)	/*. Last input mode for	.*/
						/*. returning from help .*/
#define OLC_READONLY(d)	((d)->olc->readonly)	/*. readonly mode 	.*/
#define OLC_CLAN(d)	((d)->olc->clan)	/*. Clan structure	.*/
#define OLC_CLAN_EDIT(d)	((d)->olc->clan_edit)	/*. Clan building	.*/

/*. Other macros .*/

#define OLC_EXIT(d)	(OLC_ROOM(d)->dir_option[OLC_VAL(d)])
#define GET_OLC_ZONE(c)	((c)->player_specials->saved.olc_zone)

/*. Cleanup types .*/
#define CLEANUP_ALL			1	/*. Free the whole lot  .*/
#define CLEANUP_STRUCTS 		2	/*. Don't free strings  .*/

/*. Add/Remove save list types	.*/
#define OLC_SAVE_ROOM			0
#define OLC_SAVE_OBJ			1
#define OLC_SAVE_ZONE			2
#define OLC_SAVE_MOB			3
#define OLC_SAVE_SHOP			4
#define OLC_SAVE_GM			5

/* Submodes of OEDIT connectedness */
#define OEDIT_MAIN_MENU              	1
#define OEDIT_EDIT_NAMELIST          	2
#define OEDIT_SHORTDESC              	3
#define OEDIT_LONGDESC               	4
#define OEDIT_ACTDESC                	5
#define OEDIT_TYPE                   	6
#define OEDIT_EXTRAS                 	7
#define OEDIT_WEAR                  	8
#define OEDIT_WEIGHT                	9
#define OEDIT_COST                  	10
#define OEDIT_COSTPERDAY            	11
#define OEDIT_TIMER                 	12
#define OEDIT_VALUE_1               	13
#define OEDIT_VALUE_2               	14
#define OEDIT_VALUE_3               	15
#define OEDIT_VALUE_4               	16
#define OEDIT_APPLY                 	17
#define OEDIT_APPLYMOD              	18
#define OEDIT_EXTRADESC_KEY         	19
#define OEDIT_CONFIRM_SAVEDB        	20
#define OEDIT_CONFIRM_SAVESTRING    	21
#define OEDIT_PROMPT_APPLY          	22
#define OEDIT_EXTRADESC_DESCRIPTION 	23
#define OEDIT_EXTRADESC_MENU        	24
#define OEDIT_LEVEL                 	25
#define OEDIT_AFF_FLAGS			26
#define OEDIT_HELP			27
#define OEDIT_SPEC_PROC			28
#define OEDIT_BOTTOM_LEV		29
#define OEDIT_TOP_LEV			30


/* Submodes of REDIT connectedness */
#define REDIT_MAIN_MENU 		1
#define REDIT_NAME 			2
#define REDIT_DESC 			3
#define REDIT_FLAGS 			4
#define REDIT_SECTOR 			5
#define REDIT_EXIT_MENU 		6
#define REDIT_CONFIRM_SAVEDB 		7
#define REDIT_CONFIRM_SAVESTRING 	8
#define REDIT_EXIT_NUMBER 		9
#define REDIT_EXIT_DESCRIPTION 		10
#define REDIT_EXIT_KEYWORD 		11
#define REDIT_EXIT_KEY 			12
#define REDIT_EXIT_DOORFLAGS 		13
#define REDIT_EXTRADESC_MENU 		14
#define REDIT_EXTRADESC_KEY 		15
#define REDIT_EXTRADESC_DESCRIPTION 	16
#define REDIT_TELEPORT_MENU		17
#define REDIT_TELEPORT_TARGET		18
#define REDIT_TELEPORT_FREQ		19
#define REDIT_TELEPORT_OBJ		20
#define REDIT_HELP			21
#define REDIT_SPEC_PROC			22
#define REDIT_ROOM_AFFECT		23
#define REDIT_TELE_MAIN_MENU		24


/*. Submodes of ZEDIT connectedness 	.*/
#define ZEDIT_MAIN_MENU              	0
#define ZEDIT_DELETE_ENTRY		1
#define ZEDIT_NEW_ENTRY			2
#define ZEDIT_CHANGE_ENTRY		3
#define ZEDIT_COMMAND_TYPE		4
#define ZEDIT_IF_FLAG			5
#define ZEDIT_ARG1			6
#define ZEDIT_ARG2			7
#define ZEDIT_ARG3			8
#define ZEDIT_ZONE_NAME			9
#define ZEDIT_ZONE_LIFE			10
#define ZEDIT_ZONE_TOP			11
#define ZEDIT_ZONE_RESET		12
#define ZEDIT_CONFIRM_SAVESTRING	13
#define ZEDIT_CREATOR			14
#define ZEDIT_MASTER_MOB		15
#define ZEDIT_ZON_FLAGS			16
#define ZEDIT_HELP			17
#define ZEDIT_ZONE_BUILDERS             18


/*. Submodes of MEDIT connectedness 	.*/
#define MEDIT_MAIN_MENU              	0
#define MEDIT_ALIAS			1
#define MEDIT_S_DESC			2
#define MEDIT_L_DESC			3
#define MEDIT_D_DESC			4
#define MEDIT_NPC_FLAGS			5
#define MEDIT_AFF_FLAGS			6
#define MEDIT_CONFIRM_SAVESTRING	7
#define MEDIT_HELP			8
/*. Numerical responses .*/
#define MEDIT_NUMERICAL_RESPONSE	10
#define MEDIT_SEX			11
#define MEDIT_HITROLL			12
#define MEDIT_DAMROLL			13
#define MEDIT_NDD			14
#define MEDIT_SDD			15
#define MEDIT_NUM_HP_DICE		16
#define MEDIT_SIZE_HP_DICE		17
#define MEDIT_ADD_HP			18
#define MEDIT_AC			19
#define MEDIT_EXP			20
#define MEDIT_GOLD			21
#define MEDIT_POS			22
#define MEDIT_DEFAULT_POS		23
#define MEDIT_ATTACK			24
#define MEDIT_LEVEL			25
#define MEDIT_ALIGNMENT			26
#define MEDIT_MOB_PROG			27
#define MEDIT_MPROG_MENU		28
#define MEDIT_MPROG_TYPE		29
#define MEDIT_MPROG_ARG			30
#define MEDIT_MPROG_CMDS		31
#define MEDIT_SPEC_PROC			32

/*. Submodes of SEDIT connectedness 	.*/
#define SEDIT_MAIN_MENU              	0
#define SEDIT_CONFIRM_SAVESTRING	1
#define SEDIT_NOITEM1			2
#define SEDIT_NOITEM2			3
#define SEDIT_NOCASH1			4
#define SEDIT_NOCASH2			5
#define SEDIT_NOBUY			6
#define SEDIT_BUY			7
#define SEDIT_SELL			8
#define SEDIT_PRODUCTS_MENU		11
#define SEDIT_ROOMS_MENU		12
#define SEDIT_NAMELIST_MENU		13
#define SEDIT_NAMELIST			14
#define SEDIT_HELP			15
/*. Numerical responses .*/
#define SEDIT_NUMERICAL_RESPONSE	20
#define SEDIT_OPEN1			21
#define SEDIT_OPEN2			22
#define SEDIT_CLOSE1			23
#define SEDIT_CLOSE2			24
#define SEDIT_KEEPER			25
#define SEDIT_BUY_PROFIT		26
#define SEDIT_SELL_PROFIT		27
#define SEDIT_TYPE_MENU			29
#define SEDIT_DELETE_TYPE		30
#define SEDIT_DELETE_PRODUCT		31
#define SEDIT_NEW_PRODUCT		32
#define SEDIT_DELETE_ROOM		33
#define SEDIT_NEW_ROOM			34
#define SEDIT_SHOP_FLAGS		35
#define SEDIT_NOTRADE			36

/*. Submodes of GEDIT connectedness     . */
#define GEDIT_MAIN_MENU                 0
#define GEDIT_CONFIRM_SAVESTRING        1
#define GEDIT_NO_CASH                   2
#define GEDIT_NO_SKILL                  3
#define GEDIT_HELP			4
/*. Numerical responses . */
#define GEDIT_NUMERICAL_RESPONSE        5
#define GEDIT_CHARGE                    6
#define GEDIT_OPEN                      7
#define GEDIT_CLOSE                     8
#define GEDIT_TRAINER                   9
#define GEDIT_NO_TRAIN                  10
#define GEDIT_SELECT_SPELLS		11
#define GEDIT_SELECT_SKILLS		12

/*. Limit info .*/
#define MAX_ROOM_NAME	75
#define MAX_MOB_NAME	50
#define MAX_OBJ_NAME	50
#define MAX_ROOM_DESC	1024
#define MAX_EXIT_DESC	256
#define MAX_EXTRA_DESC  512
#define MAX_MOB_DESC	512
#define MAX_OBJ_DESC	512

#define	CHECK_MASK_PERM(d, mask)	((OLC_CLAN_EDIT(d) || GET_LEVEL(d->character) < LVL_CHBUILD) ? mask : NULL)
#define CHECK_RANGE(level, what, bottom, top)	{ 	\
  	  if (GET_LEVEL(d->character) <= level) { 	\
	    if (what < bottom) {			\
	      what = bottom;				\
	      perm_range_message(d, bottom, top);	\
	      return;					\
	    } else if (what > top) {			\
	      what = top;				\
	      perm_range_message(d, bottom, top);	\
	      return;					\
	    } 						\
	  }						\
	}

#endif
