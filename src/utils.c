/* ************************************************************************
*   File: utils.c                                       Part of CircleMUD *
*  Usage: various internal functions of a utility nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "screen.h"
#include "spells.h"
#include "handler.h"

extern struct time_data time_info;
extern struct room_data *world;
extern int sunlight;
extern struct descriptor_data *descriptor_list;
extern int top_of_world;

/* local functions */
struct time_info_data *real_time_passed(time_t t2, time_t t1);
struct time_info_data *mud_time_passed(time_t t2, time_t t1);
void die_follower(struct char_data * ch);
void add_follower(struct char_data * ch, struct char_data * leader);

/* extern functions */
void make_corpse(struct char_data * ch);
char *one_argument(char *argument, char *first_arg);

/* creates a random number in interval [from;to] */
int number(int from, int to)
{
  /* error checking in case people call number() incorrectly */
  if (from > to) {
    int tmp = from;
    from = to;
    to = tmp;
    log("SYSERR: number() should be called with lowest, then highest. number(%d, %d), not number(%d, %d).", from, to, to, from);
  }

  return ((circle_random() % (to - from + 1)) + from);
}


/* simulates dice roll */
int dice(int number, int size)
{
  int sum = 0;

  if (size <= 0 || number <= 0)
    return 0;

  while (number-- > 0)
    sum += ((circle_random() % size) + 1);

  return sum;
}


int MIN(int a, int b)
{
  return a < b ? a : b;
}


int MAX(int a, int b)
{
  return a > b ? a : b;
}



/* Create a duplicate of a string */
char *str_dup(const char *source)
{
  char *new_z;

  CREATE(new_z, char, strlen(source) + 1);
  return (strcpy(new_z, source));
}



/* str_cmp: a case-insensitive version of strcmp */
/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different or end of both                 */
int str_cmp(const char *arg1, const char *arg2)
{
  int chk, i;

  for (i = 0; *(arg1 + i) || *(arg2 + i); i++)
    if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))) {
      if (chk < 0)
	return (-1);
      else
	return (1);
    }
  return (0);
}


/* strn_cmp: a case-insensitive version of strncmp */
/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different, end of both, or n reached     */
int strn_cmp(const char *arg1, const char *arg2, int n)
{
  int chk, i;

  for (i = 0; (*(arg1 + i) || *(arg2 + i)) && (n > 0); i++, n--)
    if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))) {
      if (chk < 0)
	return (-1);
      else
	return (1);
    }
  return (0);
}


/* log a death trap hit */
void log_death_trap(struct char_data * ch)
{
  char buf[150];

  sprintf(buf, "%s hit death trap #%d (%s)", GET_NAME(ch),
	  GET_ROOM_VNUM(IN_ROOM(ch)), world[ch->in_room].name);
  mudlog(buf, BRF, LVL_IMMORT, TRUE);
}

/*
 * New variable argument log() function.  Works the same as the old for
 * previously written code but is very nice for new code.
 */
void basic_mud_log(const char *format, ...)
{
  va_list args;
  time_t ct = time(0);
  char *time_s = asctime(localtime(&ct));

  time_s[strlen(time_s) - 1] = '\0';

  fprintf(logfile, "%-15.15s :: ", time_s + 4);

  va_start(args, format);
  vfprintf(logfile, format, args);
  va_end(args);

  fprintf(logfile, "\n");
  fflush(logfile);
}

/* the "touch" command, essentially. */
int touch(const char *path)
{
  FILE *fl;

  if (!(fl = fopen(path, "a"))) {
    perror(path);
    return -1;
  } else {
    fclose(fl);
    return 0;
  }
}


/*
 * mudlog -- log mud messages to a file & to online imm's syslogs
 * based on syslog by Fen Jul 3, 1992
 */
void mudlog(const char *str, int type, int level, int file)
{
  char buf[MAX_STRING_LENGTH], tp;
  struct descriptor_data *i;

  if (file)
    log(str);
  if (level < 0)
    return;

  sprintf(buf, "[ %s ]\r\n", str);

  for (i = descriptor_list; i; i = i->next)
    if (STATE(i) == CON_PLAYING && !IS_NPC(i->character) &&
      !PLR_FLAGGED(i->character, PLR_WRITING)) {
      tp = ((PRF_FLAGGED(i->character, PRF_LOG1) ? 1 : 0) +
	    (PRF_FLAGGED(i->character, PRF_LOG2) ? 2 : 0));
      if ((GET_LEVEL(i->character) >= level) && (tp >= type)) {
	send_to_char(CCGRN(i->character, C_NRM), i->character);
	send_to_char(buf, i->character);
	send_to_char(CCNRM(i->character, C_NRM), i->character);
      }
    }
}



void sprintbit(long bitvector, const char *names[], char *result)
{
  long nr;

  *result = '\0';

  if (bitvector < 0) {
    strcpy(result, "<INVALID BITVECTOR>");
    return;
  }
  for (nr = 0; bitvector; bitvector >>= 1) {
    if (IS_SET(bitvector, 1)) {
      if (*names[nr] != '\n') {
	strcat(result, names[nr]);
	strcat(result, " ");
      } else
	strcat(result, "UNDEFINED ");
    }
    if (*names[nr] != '\n')
      nr++;
  }

  if (!*result)
    strcpy(result, "NOBITS ");
}


void sprintbit_multi(long bitvector[], const char *names[], char *result)
{
  long nr, m;

  *result = '\0';
  for (nr = 0, m = 1; *names[nr] != '\n'; nr++) {
/*    sprintf(loc_buf, "%s, %d, %d, %d", names[nr], nr, nr / (sizeof(long) * 8), m);
    log(loc_buf); */
    if (IS_SET(bitvector[nr / (sizeof(long) * 8)], m)) {
      strcat(result, names[nr]);
      strcat(result, " ");
    }
    m <<= 1;
    if (!m) m = 1;
  }

  if (!*result)
    strcpy(result, "NOBITS ");
}



void sprinttype(int type, const char *names[], char *result)
{
  int nr = 0;

  while (type && *names[nr] != '\n') {
    type--;
    nr++;
  }

  if (*names[nr] != '\n')
    strcpy(result, names[nr]);
  else
    strcpy(result, "UNDEFINED");
}


/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
struct time_info_data *real_time_passed(time_t t2, time_t t1)
{
  long secs;
  static struct time_info_data now;

  secs = (long) (t2 - t1);

  now.hours = (secs / SECS_PER_REAL_HOUR) % 24;	/* 0..23 hours */
  secs -= SECS_PER_REAL_HOUR * now.hours;

  now.day = (secs / SECS_PER_REAL_DAY);	/* 0..34 days  */
  secs -= SECS_PER_REAL_DAY * now.day;

  now.month = -1;
  now.year = -1;

  return &now;
}



/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data *mud_time_passed(time_t t2, time_t t1)
{
  long secs;
  static struct time_info_data now;

  secs = (long) (t2 - t1);

  now.hours = (secs / SECS_PER_MUD_HOUR) % 24;	/* 0..23 hours */
  secs -= SECS_PER_MUD_HOUR * now.hours;

  now.day = (secs / SECS_PER_MUD_DAY) % 35;	/* 0..34 days  */
  secs -= SECS_PER_MUD_DAY * now.day;

  now.month = (secs / SECS_PER_MUD_MONTH) % 17;	/* 0..16 months */
  secs -= SECS_PER_MUD_MONTH * now.month;

  now.year = (secs / SECS_PER_MUD_YEAR);	/* 0..XX? years */

  return &now;
}



struct time_info_data *age(struct char_data * ch)
{
  static struct time_info_data player_age;

  player_age = *mud_time_passed(time(0), ch->player.time.birth);

  player_age.year += 17;	/* All players start at 17 */

  return &player_age;
}


/* Check if making CH follow VICTIM will create an illegal */
/* Follow "Loop/circle"                                    */
bool circle_follow(struct char_data * ch, struct char_data * victim)
{
  struct char_data *k;

  for (k = victim; k; k = k->master) {
    if (k == ch)
      return TRUE;
  }

  return FALSE;
}



/* Called when stop following persons, or stopping charm */
/* This will NOT do if a character quits/dies!!          */
void stop_follower(struct char_data * ch)
{
  struct follow_type *j, *k;

  if (ch->master == NULL) {
    core_dump();
    return;
  }

  if (AFF_FLAGGED(ch, AFF_CHARM)) {
    act("You realize that $N is a jerk!", FALSE, ch, 0, ch->master, TO_CHAR);
    act("$n realizes that $N is a jerk!", FALSE, ch, 0, ch->master, TO_NOTVICT);
    act("$n hates your guts!", FALSE, ch, 0, ch->master, TO_VICT);
    if (affected_by_spell(ch, SPELL_CHARM))
      affect_from_char(ch, SPELL_CHARM);
  } else {
    act("You stop following $N.", FALSE, ch, 0, ch->master, TO_CHAR);
    act("$n stops following $N.", TRUE, ch, 0, ch->master, TO_NOTVICT);
    act("$n stops following you.", TRUE, ch, 0, ch->master, TO_VICT);
  }

  if (ch->master->followers->follower == ch) {	/* Head of follower-list? */
    k = ch->master->followers;
    ch->master->followers = k->next;
    free(k);
  } else {			/* locate follower who is not head of list */
    for (k = ch->master->followers; k->next->follower != ch; k = k->next);

    j = k->next;
    k->next = j->next;
    free(j);
  }

  ch->master = NULL;
  REMOVE_BIT(AFF_FLAGS(ch), AFF_CHARM | AFF_GROUP);
}



/* Called when a character that follows/is followed dies */
void die_follower(struct char_data * ch)
{
  struct follow_type *j, *k;
  struct char_data *vict;

  if (ch->master)
    stop_follower(ch);

  for (k = ch->followers; k; k = j) {
    j = k->next;
    vict = k->follower;
    stop_follower(vict);
    if (IS_NPC(ch) && MOB_FLAGGED(ch, MOB_PET)) {
	die_follower(ch);
	act("Without a master, $n perishes!", FALSE, ch, 0, 0, TO_ROOM);
	make_corpse(ch);
	extract_char(ch);
    } else if (IS_NPC(vict) && MOB_FLAGGED(vict, MOB_PET)) {
	die_follower(vict);
	act("Without a master, $n perishes!", FALSE, vict, 0, 0, TO_ROOM);
	make_corpse(vict);
	extract_char(vict);
    }
  }
}



/* Do NOT call this before having checked if a circle of followers */
/* will arise. CH will follow leader                               */
void add_follower(struct char_data * ch, struct char_data * leader)
{
  struct follow_type *k;

  if (ch->master) {
    core_dump();
    return;
  }

  ch->master = leader;

  CREATE(k, struct follow_type, 1);

  k->follower = ch;
  k->next = leader->followers;
  leader->followers = k;

  act("You now follow $N.", FALSE, ch, 0, leader, TO_CHAR);
  if (CAN_SEE(leader, ch))
    act("$n starts following you.", TRUE, ch, 0, leader, TO_VICT);
  act("$n starts to follow $N.", TRUE, ch, 0, leader, TO_NOTVICT);
}

/*
 * get_line reads the next non-blank line off of the input stream.
 * The newline character is removed from the input.  Lines which begin
 * with '*' are considered to be comments.
 *
 * Returns the number of lines advanced in the file.
 */
int get_line(FILE * fl, char *buf)
{
  char temp[256];
  int lines = 0;

  do {
    lines++;
    fgets(temp, 256, fl);
    if (*temp)
      temp[strlen(temp) - 1] = '\0';
  } while (!feof(fl) && (*temp == '*' || !*temp));

  if (feof(fl)) {
    *buf = '\0';
    return 0;
  } else {
    strcpy(buf, temp);
    return lines;
  }
}


int get_filename(char *orig_name, char *filename, int mode)
{
  const char *prefix, *middle, *suffix;
  char name[64], *ptr;

  switch (mode) {
  case CRASH_FILE:
    prefix = LIB_PLROBJS;
    suffix = SUF_OBJS;
    break;
  case ETEXT_FILE:
    prefix = LIB_PLRTEXT;
    suffix = SUF_TEXT;
    break;
  case ALIAS_FILE:
    prefix = "plralias";
    suffix = "alias";
    break;
  case OBJPOS_FILE:
    prefix = LIB_PLROBJS;
    suffix = "objpos";
    break;
  case SAVE_FILE:
    prefix = LIB_PLROBJS;
    suffix = SUF_SAVE;
    break;
  default:
    return 0;
  }

  if (!*orig_name)
    return 0;

  strcpy(name, orig_name);
  for (ptr = name; *ptr; ptr++)
    *ptr = LOWER(*ptr);

  switch (LOWER(*name)) {
  case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
    middle = "A-E";
    break;
  case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
    middle = "F-J";
    break;
  case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
    middle = "K-O";
    break;
  case 'p':  case 'q':  case 'r':  case 's':  case 't':
    middle = "P-T";
    break;
  case 'u':  case 'v':  case 'w':  case 'x':  case 'y':  case 'z':
    middle = "U-Z";
    break;
  default:
    middle = "ZZZ";
    break;
  }

  sprintf(filename, "%s%s"SLASH"%s.%s", prefix, middle, name, suffix);
  return 1;
}

/*
 * This function (derived from basic fork(); abort(); idea by Erwin S.
 * Andreasen) causes your MUD to dump core (assuming you can) but
 * continue running.  The core dump will allow post-mortem debugging
 * that is less severe than assert();  Don't call this directly as
 * core_dump_unix() but as simply 'core_dump()' so that it will be
 * excluded from systems not supporting them. (e.g. Windows '95).
 *
 * XXX: Wonder if flushing streams includes sockets?
 */
void core_dump_real(const char *who, ush_int line)
{
  log("SYSERR: Assertion failed at %s:%d!", who, line);

#if defined(CIRCLE_UNIX)
  /* These would be duplicated otherwise... */
  fflush(stdout);
  fflush(stderr);
  fflush(logfile);

  /*
   * Kill the child so the debugger or script doesn't think the MUD
   * crashed.  The 'autorun' script would otherwise run it again.
   */
  if (fork() == 0)
    abort();
#endif
}


int num_pc_in_room(struct room_data *room)
{
  int i = 0;
  struct char_data *ch;

  for (ch = room->people; ch != NULL; ch = ch->next_in_room)
    if (!IS_NPC(ch))
      i++;

  return i;
}


/* string manipulation fucntion originally by Darren Wilson */
/* (wilson@shark.cc.cc.ca.us) improved and bug fixed by Chris (zero@cnw.com) */
/* completely re-written again by M. Scott 10/15/96 (scottm@workcommn.net), */
/* substitute appearances of 'pattern' with 'replacement' in string */
/* and return the # of replacements */
int replace_str(char **string, char *pattern, char *replacement, int rep_all,
		int max_size) {
   char *replace_buffer = NULL;
   char *flow, *jetsam, temp;
   int len, i;
   
   if ((strlen(*string) - strlen(pattern)) + strlen(replacement) > max_size)
     return -1;
   
   CREATE(replace_buffer, char, max_size);
   i = 0;
   jetsam = *string;
   flow = *string;
   *replace_buffer = '\0';
   if (rep_all) {
      while ((flow = (char *)strstr(flow, pattern)) != NULL) {
	 i++;
	 temp = *flow;
	 *flow = '\0';
	 if ((strlen(replace_buffer) + strlen(jetsam) + strlen(replacement)) > max_size) {
	    i = -1;
	    break;
	 }
	 strcat(replace_buffer, jetsam);
	 strcat(replace_buffer, replacement);
	 *flow = temp;
	 flow += strlen(pattern);
	 jetsam = flow;
      }
      strcat(replace_buffer, jetsam);
   }
   else {
      if ((flow = (char *)strstr(*string, pattern)) != NULL) {
	 i++;
	 flow += strlen(pattern);  
	 len = ((char *)flow - (char *)*string) - strlen(pattern);
   
	 strncpy(replace_buffer, *string, len);
	 strcat(replace_buffer, replacement);
	 strcat(replace_buffer, flow);
      }
   }
   if (i == 0) return 0;
   if (i > 0) {
      RECREATE(*string, char, strlen(replace_buffer) + 3);
      strcpy(*string, replace_buffer);
   }
   free(replace_buffer);
   return i;
}


/* re-formats message type formatted char * */
/* (for strings edited with d->str) (mostly olc and mail)     */
void format_text(char **ptr_string, int mode, struct descriptor_data *d, int maxlen) {
   int total_chars, cap_next = TRUE, cap_next_next = FALSE;
   char *flow, *start = NULL, temp;
   /* warning: do not edit messages with max_str's of over this value */
   char formated[MAX_STRING_LENGTH];
   
   flow   = *ptr_string;
   if (!flow) return;

   if (IS_SET(mode, FORMAT_INDENT)) {
      strcpy(formated, "   ");
      total_chars = 3;
   }
   else {
      *formated = '\0';
      total_chars = 0;
   } 

   while (*flow != '\0') {
      while ((*flow == '\n') ||
	     (*flow == '\r') ||
	     (*flow == '\f') ||
	     (*flow == '\t') ||
	     (*flow == '\v') ||
	     (*flow == ' ')) flow++;

      if (*flow != '\0') {

	 start = flow++;
	 while ((*flow != '\0') &&
		(*flow != '\n') &&
		(*flow != '\r') &&
		(*flow != '\f') &&
		(*flow != '\t') &&
		(*flow != '\v') &&
		(*flow != ' ') &&
		(*flow != '.') &&
		(*flow != '?') &&
		(*flow != '!')) flow++;

	 if (cap_next_next) {
	    cap_next_next = FALSE;
	    cap_next = TRUE;
	 }

	 /* this is so that if we stopped on a sentance .. we move off the sentance delim. */
	 while ((*flow == '.') || (*flow == '!') || (*flow == '?')) {
	    cap_next_next = TRUE;
	    flow++;
	 }
	 
	 temp = *flow;
	 *flow = '\0';

	 if ((total_chars + strlen(start) + 1) > 79) {
	    strcat(formated, "\r\n");
	    total_chars = 0;
	 }

	 if (!cap_next) {
	    if (total_chars > 0) {
	       strcat(formated, " ");
	       total_chars++;
	    }
	 }
	 else {
	    cap_next = FALSE;
	    *start = UPPER(*start);
	 }

	 total_chars += strlen(start);
	 strcat(formated, start);

	 *flow = temp;
      }

      if (cap_next_next) {
	 if ((total_chars + 3) > 79) {
	    strcat(formated, "\r\n");
	    total_chars = 0;
	 }
	 else {
	    strcat(formated, "  ");
	    total_chars += 2;
	 }
      }
   }
   strcat(formated, "\r\n");

   if (strlen(formated) > maxlen) formated[maxlen] = '\0';
   RECREATE(*ptr_string, char, MIN(maxlen, strlen(formated)+3));
   strcpy(*ptr_string, formated);
}
  
/* strips \r's from line */
char *stripcr(char *dest, const char *src) {
   int i, length;
   char *temp;

   if (!dest || !src) return NULL;
   temp = &dest[0];
   length = strlen(src);
   for (i = 0; *src && (i < length); i++, src++)
     if (*src != '\r') *(temp++) = *src;
   *temp = '\0';
   return dest;
}

void menubit(long_bitv bitvector, const char *names[], char *result, long_bitv mask)
{
  int m, nr, c;
  
  *result = '\0';
  c = 0;
  for (nr = 0, m = 1; *names[nr] != '\n'; nr++) {
    if (mask != NULL && IS_SET(mask[nr / (8*sizeof(long))], m))
      strcat(result, "&w");
    else strcat(result, "&W");
    sprintf(result + strlen(result), "%3d&w [%s&w] &c%-15s", nr+1,
      YESNO(IS_SET(bitvector[nr / (8*sizeof(long))], m)), names[nr]);
    if (c == 2) {
      strcat(result, "\r\n");
      c = 0;
    } else c++;
    m <<= 1;
    if (!m) m = 1;
  }
  strcat(result, "&w");
  return;
}

void updatebit(long_bitv bitvector, int num_flags, int arg, long_bitv mask)
{
  if (arg > num_flags || arg <= 0) return;
  if (mask != NULL && IS_SET(mask[(arg-1) / (8 * sizeof(long))], 
    1 << ((arg - 1) % (8 * sizeof(long))))) return;
  TOGGLE_BIT(bitvector[(arg-1) / (8 * sizeof(long))], 
    1 << ((arg - 1) % (8 * sizeof(long))));
}

void show_spec_proc_table(struct specproc_info table[], int level, char *result)
{
  int nr, c, l;
  char local_buf[128];
  
  *result  = '\0';
  l = 0;
  for (nr = 0, c = 0; table[nr].name[0] != '\n'; nr++)
    if (table[nr].minimum_level <= level) {
      sprintf(local_buf,"&W%2d &c%-15s", l, table[nr].name);
      strcat(result, local_buf);
      if (c == 3) {
        strcat(result, "\r\n");
        c = 0;
      } else c++;
      l++;
    }
  strcat(result, "&w");
  return;
}

int get_spec_proc_index(struct specproc_info table[], int level, int arg)
{
  int nr, l;
  
  for (l = 0, nr = 0; table[nr].name[0] != '\n'; nr++)
    if (table[nr].minimum_level <= level) {
      if (l == arg) return nr;
      l++;
    }
  return 0;
}

int get_spec_proc(struct specproc_info table[], const char* arg)
{
  int nr;
  
  for (nr = 0; table[nr].name[0] != '\n'; nr++)
    if (str_cmp(table[nr].name, arg) == 0) return nr;
  return 0;
}

int get_spec_name(const struct specproc_info table[], SPECIAL(fname))
{
  int i;
  if (fname != NULL) {
    for (i = 0; table[i].name[0] != '\n'; i++)
      if (table[i].sp_pointer == fname) return i;
  }
  return 0;
}

void writebit_multi(long_bitv bitvector, int sizebitv, char *buf)
{
  int i;
  buf[0] = '\0';
  for (i = 0; i < sizebitv; i++)
    sprintf(buf + strlen(buf), "%ld%s", bitvector[i], 
      (i == (sizebitv + 1)) ? "\0": " ");
  return;
}

int is_setbit_multi(long_bitv bitvector, int sizebitv)
{
  int i;
  for (i = 0; i < sizebitv; i++)
    if (bitvector[i]) return 1;
  return 0;
} 

int search_block_flag(char *str, const char *list[])
{
  int i = 0;
  while (list[i] && *list[i] && *list[i] != '\n') {
    if (!str_cmp(str, list[i])) return i;
    i++;
  } 
  return -1;
}

int get_bitv_from_string(char *str, const char *list[], long_bitv bitvector, int max)
{
  char *ptr = str;
  int i;
  
  while (ptr && *ptr) {
    ptr = one_argument(ptr, buf2);
    i = search_block_flag(buf2, list);
    if (i < 0 || i >= max) return 0;
    if ((bitvector[i / (8*sizeof(long))] & (1 << (i % (8*sizeof(long))))) != 0)
      return 1;
  }
  return 0;
}
