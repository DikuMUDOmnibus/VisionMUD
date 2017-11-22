/* ************************************************************************
*   File: config.c                                      Part of CircleMUD *
*  Usage: Configuration of various aspects of CircleMUD operation         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __CONFIG_C__

#include "conf.h"
#include "sysdep.h"

#include "structs.h"

#define TRUE	1
#define YES	1
#define FALSE	0
#define NO	0

/*
 * Below are several constants which you can change to alter certain aspects
 * of the way CircleMUD acts.  Since this is a .c file, all you have to do
 * to change one of the constants (assuming you keep your object files around)
 * is change the constant in this file and type 'make'.  Make will recompile
 * this file and relink; you don't have to wait for the whole thing to
 * recompile as you do if you change a header file.
 *
 * I realize that it would be slightly more efficient to have lots of
 * #defines strewn about, so that, for example, the autowiz code isn't
 * compiled at all if you don't want to use autowiz.  However, the actual
 * code for the various options is quite small, as is the computational time
 * in checking the option you've selected at run-time, so I've decided the
 * convenience of having all your options in this one file outweighs the
 * efficency of doing it the other way.
 *
 */

/****************************************************************************/
/****************************************************************************/


/* GAME PLAY OPTIONS */

/*
 * pk_allowed sets the tone of the entire game.  If pk_allowed is set to
 * NO, then players will not be allowed to kill, summon, charm, or sleep
 * other players, as well as a variety of other "asshole player" protections.
 * However, if you decide you want to have an all-out knock-down drag-out
 * PK Mud, just set pk_allowed to YES - and anything goes.
 */
 
/* All theses settings are overridden by do_world command and automatic boot
   loader */
int pk_allowed = NO;
int summon_allowed = NO;
int charm_allowed = NO;
int sleep_allowed = NO;
int roomaffect_allowed = NO;

/* is playerthieving allowed? */
int pt_allowed = NO;

/* God olc allowed? */
int olc_allowed = YES;

/* Clan olc allowed */
int clan_olc_allowed = YES;

/* minimum level a player must be to shout/holler/gossip/auction */
int level_can_shout = 1;

/* number of movement points it costs to holler */
int holler_move_cost = 20;

/* exp change limits */
int max_exp_gain = 100000;	/* max gainable per kill */
int max_exp_loss = 500000;	/* max losable per death */

/* number of tics (usually 75 seconds) before PC/NPC corpses decompose */
int max_npc_corpse_time = 5;
int max_pc_corpse_time = 10;

/* How many ticks before a player is sent to the void or idle-rented. */
int idle_void = 8;
int idle_rent_time = 48;

/* This level and up is immune to idling, LVL_IMPL+1 will disable it. */
int idle_max_level = LVL_GOD;

/* should items in death traps automatically be junked? */
int dts_are_dumps = NO;

/* Max level difference to PK */
int max_level_diff = 10;

/*
 * Whether you want items that immortals load to appear on the ground or not.
 * It is most likely best to set this to 'YES' so that something else doesn't
 * grab the item before the immortal does, but that also means people will be
 * able to carry around things like boards.  That's not necessarily a bad
 * thing, but this will be left at a default of 'NO' for historic reasons.
 */
int load_into_inventory = YES;

/* "okay" etc. */
const char *OK = "Okay.\r\n";
const char *NOPERSON = "No-one by that name here.\r\n";
const char *NOEFFECT = "Nothing seems to happen.\r\n";
const char *NONESTR = "<NONE>";

/****************************************************************************/
/****************************************************************************/


/* RENT/CRASHSAVE OPTIONS */

/*
 * Should the MUD allow you to 'rent' for free?  (i.e. if you just quit,
 * your objects are saved at no cost, as in Merc-type MUDs.)
 */
int free_rent = NO;

/* maximum number of items players are allowed to rent */
int max_obj_save = 30;

/* receptionist's surcharge on top of item costs */
int min_rent_cost = 1;

/*
 * Should the game automatically save people?  (i.e., save player data
 * every 4 kills (on average), and Crash-save as defined below.  This
 * option has an added meaning past bpl13.  If auto_save is YES, then
 * the 'save' command will be disabled to prevent item duplication via
 * game crashes.
 */
int auto_save = YES;

/*
 * if auto_save (above) is yes, how often (in minutes) should the MUD
 * Crash-save people's objects?   Also, this number indicates how often
 * the MUD will Crash-save players' houses.
 */
int autosave_time = 10;

/* Lifetime of crashfiles and forced-rent (idlesave) files in days */
int crash_file_timeout = 10;

/* Lifetime of normal rent files in days */
int rent_file_timeout = 30;


/****************************************************************************/
/****************************************************************************/


/* ROOM NUMBERS */

/* virtual number of room that mortals should enter at */
sh_int mortal_start_room = 3001;

/* virtual number of room that immorts should enter at by default */
sh_int immort_start_room = 1204;

/* virtual number of room that frozen players should enter at */
sh_int frozen_start_room = 1202;

/*
 * virtual numbers of donation rooms.  note: you must change code in
 * do_drop of act.item.c if you change the number of non-NOWHERE
 * donation rooms.
 */
sh_int donation_room_1 = 3063;
sh_int donation_room_2 = 6602;	        /* unused - room for expansion */
sh_int donation_room_3 = NOWHERE;	/* unused - room for expansion */

/* is teleport rooms working yet? */
	int teleport_on = YES;
/****************************************************************************/
/****************************************************************************/


/* GAME OPERATION OPTIONS */

/*
 * This is the default port on which the game should run if no port is
 * given on the command-line.  NOTE WELL: If you're using the
 * 'autorun' script, the port number there will override this setting.
 * Change the PORT= line in autorun instead of (or in addition to)
 * changing this.
 */
int DFLT_PORT = 4000;

/*
 * IP address to which the MUD should bind.  This is only useful if
 * you're running Circle on a host that host more than one IP interface,
 * and you only want to bind to *one* of them instead of all of them.
 * Setting this to NULL (the default) causes Circle to bind to all
 * interfaces on the host.  Otherwise, specify a numeric IP address in
 * dotted quad format, and Circle will only bind to that IP address.  (Of
 * course, that IP address must be one of your host's interfaces, or it
 * won't work.)
 */
const char *DFLT_IP = NULL; /* bind to all interfaces */
/* const char *DFLT_IP = "192.168.1.1";  -- bind only to one interface */

/* default directory to use as data directory */
const char *DFLT_DIR = "lib";

/*
 * What file to log messages to (ex: "log/syslog").  Setting this to NULL
 *  means you want to log to stderr, which was the default in earlier
 * versions of Circle.  If you specify a file, you don't get messages to
 * the screen. (Hint: Try 'tail -f' if you have a UNIX machine.)
 */
const char *LOGNAME = NULL;
/* const char *LOGNAME = "log/syslog";  -- useful for Windows users */

/* maximum number of players allowed before game starts to turn people away */
int MAX_PLAYERS = 300;

/* maximum size of bug, typo and idea files in bytes (to prevent bombing) */
int max_filesize = 50000;

/* maximum number of password attempts before disconnection */
int max_bad_pws = 3;

/*
 * Rationale for enabling this, as explained by naved@bird.taponline.com.
 *
 * Usually, when you select ban a site, it is because one or two people are
 * causing troubles while there are still many people from that site who you
 * want to still log on.  Right now if I want to add a new select ban, I need
 * to first add the ban, then SITEOK all the players from that site except for
 * the one or two who I don't want logging on.  Wouldn't it be more convenient
 * to just have to remove the SITEOK flags from those people I want to ban
 * rather than what is currently done?
 */
int siteok_everyone = TRUE;

/*
 * Some nameservers are very slow and cause the game to lag terribly every 
 * time someone logs in.  The lag is caused by the gethostbyaddr() function
 * which is responsible for resolving numeric IP addresses to alphabetic names.
 * Sometimes, nameservers can be so slow that the incredible lag caused by
 * gethostbyaddr() isn't worth the luxury of having names instead of numbers
 * for players' sitenames.
 *
 * If your nameserver is fast, set the variable below to NO.  If your
 * nameserver is slow, of it you would simply prefer to have numbers
 * instead of names for some other reason, set the variable to YES.
 *
 * You can experiment with the setting of nameserver_is_slow on-line using
 * the SLOWNS command from within the MUD.
 */

int nameserver_is_slow = NO;

const char *ANSI = "Do you support the ANSI color standard (Yn)? ";

const char *MENU =
"\r\n"
"&MWelcome to &YVisionMUD!&w\r\n\r\n"
"  [&W0&w]  Exit from VisionMUD.\r\n"
"  [&W1&w]  &GEnter the game.&w\r\n"
"  [&W2&w]  Enter description.\r\n"
"  [&W3&w]  Read the background story.\r\n"
"  [&W4&w]  Change password.\r\n"
"  [&W5&w]  Delete this character.\r\n"
"  [&W6&w]  VisionMUD statistics.\r\n"
"\r\n"
"   Make your choice: ";



const char *GREETINGS =
"\r\n\r\n"
"                       &YD E S P E R A T E   V I S I O N S&w\r\n"
"                       &y---------------------------------&w\r\n"
"                       &R       a.k.a   &CVision&cMUD&w\r\n"
"\r\n"
"                            &mBased on CircleMUD 3.0,\r\n"
"                            &MCreated by Jeremy Elson\r\n"
"\r\n"
"                      &wA derivative of DikuMUD (GAMMA 0.0)\r\n"
"                                  Created by\r\n"
"                     &WHans Henrik Staerfeldt, Katja Nyboe,\r\n"
"               Tom Madsen, Michael Seifert, and Sebastian Hammer\r\n"
"\r\n"
"                             &gAdditional code by\r\n"
"               &GHarvey Gilpin, Jeremy Hess, David Koepke, Haddix\r\n"
"            Wizard Crimson, Petr Vilim, George Greer and others...\r\n"
"\r\n"
"                    &cChief Implementor && Coder: &CYilard\r\n"
"\r\n\r\n"
"&gBy what name do you wish to be known?&w ";


const char *WELC_MESSG =
"\r\n"
"&CWelcome to the land of VisionMUD!  May your visit here be... Interesting.&w"
"\r\n\r\n";

const char *START_MESSG =
"&WWelcome.  This is your new VisionMUD character!  You can now earn gold,\r\n"
"gain experience, find weapons and equipment, and much more -- while\r\n"
"meeting people from around the world!&w\r\n";

/****************************************************************************/
/****************************************************************************/


/* AUTOWIZ OPTIONS */

/*
 * Should the game automatically create a new wizlist/immlist every time
 * someone immorts, or is promoted to a higher (or lower) god level?
 * NOTE: this only works under UNIX systems.
 */
int use_autowiz = NO;

/* If yes, what is the lowest level which should be on the wizlist?  (All
   immort levels below the level you specify will go on the immlist instead.) */
/* NOTE: Not used for now (YIL) */
int min_wizlist_lev = LVL_GOD;
