/*
 * Saving and resuming.
 *
 * (ESR) This replaces  a bunch of particularly nasty FORTRAN-derived code;
 * see the history.adoc file in the source distribution for discussion.
 *
 * Copyright (c) 1977, 2005 by Will Crowther and Don Woods
 * Copyright (c) 2017 by Eric S. Raymond
 * SPDX-License-Identifier: BSD-2-clause
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>

#include "advent.h"
#include "dungeon.h"

/*
 * Bump on save format change.
 *
 * Note: Verify that the tests run clean before bumping this, then rebuild the check
 * files afterwards.  Otherwise you will get a spurious failure due to the old version
 * having been generated into a check file.
 */
#define VRSION	29

/*
 * If you change the first three members, the resume function may not properly
 * reject saves from older versions.  Yes, this glues us to a hardware-
 * dependent length of int.  Later members can change, but bump the version
 * when you do that.
 */
struct save_t {
    int64_t savetime;
    int32_t mode;		/* not used, must be present for version detection */
    int32_t version;
    struct game_t game;
};
struct save_t save;

#define IGNORE(r) do{if (r){}}while(0)

int savefile(FILE *fp, int32_t version)
/* Save game to file. No input or output from user. */
{
    save.savetime = time(NULL);
    save.mode = -1;
    save.version = (version == 0) ? VRSION : version;

    save.game = game;
    IGNORE(fwrite(&save, sizeof(struct save_t), 1, fp));
    return (0);
}

/* Suspend and resume */

static char *strip(char *name)
{
    // Trim leading whitespace
    while(isspace((unsigned char)*name))
	name++;	// LCOV_EXCL_LINE
    if(*name != '\0') {
	// Trim trailing whitespace;
	// might be left there by autocomplete
	char *end = name + strlen(name) - 1;
	while(end > name && isspace((unsigned char)*end))
	    end--;
	// Write new null terminator character
	end[1] = '\0';
    }

    return name;
}

int suspend(void)
{
    /*  Suspend.  Offer to save things in a file, but charging
     *  some points (so can't win by using saved games to retry
     *  battles or to start over after learning zzword).
     *  If ADVENT_NOSAVE is defined, gripe instead. */

#if defined ADVENT_NOSAVE || defined ADVENT_AUTOSAVE
    rspeak(SAVERESUME_DISABLED);
    return GO_TOP;
#endif
    FILE *fp = NULL;

    rspeak(SUSPEND_WARNING);
    if (!yes_or_no(arbitrary_messages[THIS_ACCEPTABLE], arbitrary_messages[OK_MAN], arbitrary_messages[OK_MAN]))
        return GO_CLEAROBJ;
    game.saved = game.saved + 5;

    while (fp == NULL) {
        char* name = myreadline("\nFile name: ");
        if (name == NULL)
            return GO_TOP;
	name = strip(name);
	if (strlen(name) == 0)
            return GO_TOP;	// LCOV_EXCL_LINE
        fp = fopen(strip(name), WRITE_MODE);
        if (fp == NULL)
            printf("Can't open file %s, try again.\n", name);
        free(name);
    }

    savefile(fp, VRSION);
    fclose(fp);
    rspeak(RESUME_HELP);
    exit(EXIT_SUCCESS);
}

int resume(void)
{
    /*  Resume.  Read a suspended game back from a file.
     *  If ADVENT_NOSAVE is defined, gripe instead. */

#if defined ADVENT_NOSAVE || defined ADVENT_AUTOSAVE
    rspeak(SAVERESUME_DISABLED);
    return GO_TOP;
#endif
    FILE *fp = NULL;

    if (game.loc != 1 ||
        game.abbrev[1] != 1) {
        rspeak(RESUME_ABANDON);
        if (!yes_or_no(arbitrary_messages[THIS_ACCEPTABLE], arbitrary_messages[OK_MAN], arbitrary_messages[OK_MAN]))
            return GO_CLEAROBJ;
    }

    while (fp == NULL) {
        char* name = myreadline("\nFile name: ");
        if (name == NULL)
            return GO_TOP;
	name = strip(name);
	if (strlen(name) == 0)
            return GO_TOP;	// LCOV_EXCL_LINE
	fp = fopen(name, READ_MODE);
        if (fp == NULL)
            printf("Can't open file %s, try again.\n", name);
        free(name);
    }

    return restore(fp);
}

int restore(FILE* fp)
{
    /*  Read and restore game state from file, assuming
     *  sane initial state.
     *  If ADVENT_NOSAVE is defined, gripe instead. */
#ifdef ADVENT_NOSAVE
    rspeak(SAVERESUME_DISABLED)
    return GO_TOP;
#endif

    IGNORE(fread(&save, sizeof(struct save_t), 1, fp));
    fclose(fp);
    if (save.version != VRSION) {
        rspeak(VERSION_SKEW, save.version / 10, MOD(save.version, 10), VRSION / 10, MOD(VRSION, 10));
    } else if (!is_valid(save.game)) {
	rspeak(SAVE_TAMPERING);
	exit(EXIT_SUCCESS);
    } else {
        game = save.game;
    }
    return GO_TOP;
}

bool is_valid(struct game_t valgame)
{
    /*  Save files can be roughly grouped into three groups:
     *  With valid, reachable state, with valid, but unreachable
     *  state and with invalid state. We check that state is
     *  valid: no states are outside minimal or maximal value
     */

    /* Prevent division by zero */
    if (valgame.abbnum == 0) {
        return false;	// LCOV_EXCL_LINE
    }

    /* Check for RNG overflow. Truncate */
    if (valgame.lcg_x >= LCG_M) {
        valgame.lcg_x %= LCG_M; // LCOV_EXCL_LINE
    }

    /* Check for RNG underflow. Transpose */
    if (valgame.lcg_x < LCG_M) {
        valgame.lcg_x = LCG_M + (valgame.lcg_x % LCG_M);
    }

    /*  Bounds check for locations */
    if ( valgame.chloc  < -1 || valgame.chloc  > NLOCATIONS ||
         valgame.chloc2 < -1 || valgame.chloc2 > NLOCATIONS ||
         valgame.loc    <  0 || valgame.loc    > NLOCATIONS ||
         valgame.newloc <  0 || valgame.newloc > NLOCATIONS ||
         valgame.oldloc <  0 || valgame.oldloc > NLOCATIONS ||
         valgame.oldlc2 <  0 || valgame.oldlc2 > NLOCATIONS) {
        return false;	// LCOV_EXCL_LINE
    }
    /*  Bounds check for location arrays */
    for (int i = 0; i <= NDWARVES; i++) {
        if (valgame.dloc[i]  < -1 || valgame.dloc[i]  > NLOCATIONS  ||
            valgame.odloc[i] < -1 || valgame.odloc[i] > NLOCATIONS) {
            return false;	// LCOV_EXCL_LINE
        }
    }

    for (int i = 0; i <= NOBJECTS; i++) {
        if (valgame.place[i] < -1 || valgame.place[i] > NLOCATIONS  ||
            valgame.fixed[i] < -1 || valgame.fixed[i] > NLOCATIONS) {
            return false;	// LCOV_EXCL_LINE
        }
    }

    /*  Bounds check for dwarves */
    if (valgame.dtotal < 0 || valgame.dtotal > NDWARVES ||
        valgame.dkill < 0  || valgame.dkill  > NDWARVES) {
        return false;	// LCOV_EXCL_LINE
    }

    /*  Validate that we didn't die too many times in save */
    if (valgame.numdie >= NDEATHS) {
        return false;	// LCOV_EXCL_LINE
    }

    /* Recalculate tally, throw the towel if in disagreement */
    int temp_tally = 0;
    for (int treasure = 1; treasure <= NOBJECTS; treasure++) {
        if (objects[treasure].is_treasure) {
            if (valgame.prop[treasure] == STATE_NOTFOUND) {
                ++temp_tally;
            }
        }
    }
    if (temp_tally != valgame.tally) {
        return false;	// LCOV_EXCL_LINE
    }

    /* Check that properties of objects aren't beyond expected */
    for (obj_t obj = 0; obj <= NOBJECTS; obj++) {
	/* Magic number -2 allows a STASHED version of state 1 */
        if (valgame.prop[obj] < -2 || valgame.prop[obj] > 1) {
            switch (obj) {
            case RUG:
            case DRAGON:
            case BIRD:
            case BOTTLE:
            case PLANT:
            case PLANT2:
            case TROLL:
            case URN:
            case EGGS:
            case VASE:
            case CHAIN:
                if (valgame.prop[obj] == 2) // There are multiple different states, but it's convenient to clump them together
                    continue;	// LCOV_EXCL_LINE
            /* FALLTHRU */
            case BEAR:
                if (valgame.prop[BEAR] == CONTENTED_BEAR || valgame.prop[BEAR] == BEAR_DEAD)
                    continue;
            /* FALLTHRU */
            default:
                return false;	// LCOV_EXCL_LINE
            }
        }
    }

    /* Check that values in linked lists for objects in locations are inside bounds */
    for (loc_t loc = LOC_NOWHERE; loc <= NLOCATIONS; loc++) {
        if (valgame.atloc[loc] < NO_OBJECT || valgame.atloc[loc] > NOBJECTS * 2) {
            return false;	// LCOV_EXCL_LINE
        }
    }
    for (obj_t obj = 0; obj <= NOBJECTS * 2; obj++ ) {
        if (valgame.link[obj] < NO_OBJECT || valgame.link[obj] > NOBJECTS * 2) {
            return false;	// LCOV_EXCL_LINE
        }
    }

    return true;
}

/* end */
