/*
 * The author - Don Woods - apologises for the style of the code; it
 * is a result of running the original Fortran IV source through a
 * home-brew Fortran-to-C converter.)
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include "advent.h"
#include "database.h"

struct game_t game;

long LNLENG, LNPOSN, PARMS[MAXPARMS+1];
char rawbuf[LINESIZE], INLINE[LINESIZE+1], MAP1[129], MAP2[129];

long AMBER, AXE, BACK, BATTER, BEAR, BIRD, BLOOD,
		BOTTLE, CAGE, CAVE, CAVITY, CHAIN, CHASM, CHEST,
		CLAM, COINS, DOOR, DPRSSN, DRAGON, DWARF, EGGS,
		EMRALD, ENTER, ENTRNC, FIND, FISSUR, FOOD,
		GRATE, HINT, I, INVENT, J, JADE, K, KEYS,
		KNIFE, L, LAMP, LOCK, LOOK, MAGZIN,
		MESSAG, MIRROR, NUGGET, NUL, OGRE, OIL, OYSTER,
		PEARL, PILLOW, PLANT, PLANT2, PYRAM, RESER, ROD, ROD2,
		RUBY, RUG, SAPPH, SAY, SECT, SIGN, SNAKE, SPK,
    		STEPS, STREAM, THROW, TRIDNT, TROLL, TROLL2,
		URN, VASE, VEND,
		VOLCAN, VRSION = 25, WATER, WD1, WD1X, WD2, WD2X;
FILE  *logfp;
bool oldstyle = false;
lcg_state lcgstate;

extern void initialise();
extern void score(long);
extern int action(FILE *, long, long, long);

void sig_handler(int signo)
{
    if (signo == SIGINT)
	if (logfp != NULL)
	    fflush(logfp);
    exit(0);
}

/*
 * MAIN PROGRAM
 *
 *  Adventure (rev 2: 20 treasures)
 *
 *  History: Original idea & 5-treasure version (adventures) by Willie Crowther
 *           15-treasure version (adventure) by Don Woods, April-June 1977
 *           20-treasure version (rev 2) by Don Woods, August 1978
 *		Errata fixed: 78/12/25
 *	     Revived 2017 as Open Advebture.
 */

static bool do_command(FILE *);

int main(int argc, char *argv[])
{
    int ch;
	
/*  Options. */

    while ((ch = getopt(argc, argv, "l:o")) != EOF) {
	switch (ch) {
	case 'l':
	    logfp = fopen(optarg, "w");
	    if (logfp == NULL)
		fprintf(stderr,
			"advent: can't open logfile %s for write\n",
			optarg);
	    signal(SIGINT, sig_handler);
	    break;
	case 'o':
	    oldstyle = true;
	    break;
	}
    }

    /* Logical variables:
     *
     *  game.closed says whether we're all the way closed
     *  game.closng says whether it's closing time yet
     *  game.clshnt says whether he's read the clue in the endgame
     *  game.lmwarn says whether he's been warned about lamp going dim
     *  game.novice says whether he asked for instructions at start-up
     *  game.panic says whether he's found out he's trapped in the cave
     *  game.wzdark says whether the loc he's leaving was dark */

    /* Initialize our LCG PRNG with parameters tested against
     * Knuth vol. 2. by the original authors */
    lcgstate.a = 1093;
    lcgstate.c = 221587;
    lcgstate.m = 1048576;
    srand(time(NULL));
    long seedval = (long)rand();
    set_seed(seedval);

    /*  Initialize game variables */
    MAP2[1] = 0;
    if (!game.setup)
	initialise();

    /*  Unlike earlier versions, adventure is no longer restartable.  (This
     *  lets us get away with modifying things such as OBJSND(BIRD) without
     *  having to be able to undo the changes later.)  If a "used" copy is
     *  rerun, we come here and tell the player to run a fresh copy. */
    if(game.setup <= 0) {
	RSPEAK(201);
	exit(0);
    }

    /*  Start-up, dwarf stuff */
    game.setup= -1;
    game.zzword=RNDVOC(3,0);
    game.novice=YES(stdin, 65,1,0);
    game.newloc=1;
    game.loc=1;
    game.limit=330;
    if(game.novice)game.limit=1000;

    if (logfp)
	fprintf(logfp, "seed %ld\n", seedval);

    /* interpret commands ubtil EOF or interrupt */
    for (;;) {
	if (!do_command(stdin))
	    break;
    }
    /* show score and exit */
    score(1);
}

static bool fallback_handler(char *buf)
/* fallback handler for commands not handled by FORTRANish parser */
{
    long sv;
    if (sscanf(buf, "seed %ld", &sv) == 1) {
	set_seed(sv);
	printf("Seed set to %ld\n", sv);
	// autogenerated, so don't charge user time for it.
	--game.turns;
	// here we reconfigure any global game state that uses random numbers
	game.zzword=RNDVOC(3,0);
	return true;
    }
    return false;
}

static bool dwarfmove(void)
/* Dwarves move.  Return true if player survives, false if he dies. */
{
    int kk, stick, attack;
    long TK[21];

	/*  Dwarf stuff.  See earlier comments for description of
     *  variables.  Remember sixth dwarf is pirate and is thus
     *  very different except for motion rules. */

    /*  First off, don't let the dwarves follow him into a pit or
     *  a wall.  Activate the whole mess the first time he gets as
     *  far as the hall of mists (loc 15).  If game.newloc is
     *  forbidden to pirate (in particular, if it's beyond the
     *  troll bridge), bypass dwarf stuff.  That way pirate can't
     *  steal return toll, and dwarves can't meet the bear.  Also
     *  means dwarves won't follow him into dead end in maze, but
     *  c'est la vie.  They'll wait for him outside the dead
     *  end. */
    if(game.loc == 0 || FORCED(game.loc) || CNDBIT(game.newloc,3))
	return true;

    /* Dwarf activity level ratchets up */
    if(game.dflag == 0) {
	if(INDEEP(game.loc))
	    game.dflag=1;
	return true;
    }

    /*  When we encounter the first dwarf, we kill 0, 1, or 2 of
     *  the 5 dwarves.  If any of the survivors is at loc,
     *  replace him with the alternate. */
    if(game.dflag == 1) {
	if(!INDEEP(game.loc) || (PCT(95) && (!CNDBIT(game.loc,4) || PCT(85))))
	    return true;
	game.dflag=2;
	for (I=1; I<=2; I++) {
	    J=1+randrange(NDWARVES-1);
	    if(PCT(50))
		game.dloc[J]=0;
	}
	for (I=1; I<=NDWARVES-1; I++) {
	    if(game.dloc[I] == game.loc)
		game.dloc[I]=DALTLC;
	    game.odloc[I]=game.dloc[I];
	}
	RSPEAK(3);
	DROP(AXE,game.loc);
	return true;
    }

    /*  Things are in full swing.  Move each dwarf at random,
     *  except if he's seen us he sticks with us.  Dwarves stay
     *  deep inside.  If wandering at random, they don't back up
     *  unless there's no alternative.  If they don't have to
     *  move, they attack.  And, of course, dead dwarves don't do
     *  much of anything. */
    game.dtotal=0;
    attack=0;
    stick=0;
    for (I=1; I<=NDWARVES; I++) {
	if(game.dloc[I] == 0)
	    continue;
	/*  Fill TK array with all the places this dwarf might go. */
	J=1;
	kk=KEY[game.dloc[I]];
	if(kk != 0)
	    do {
		game.newloc=MOD(labs(TRAVEL[kk])/1000,1000);
		if(game.newloc > 300 || !INDEEP(game.newloc) || game.newloc == game.odloc[I] || (J > 1 && game.newloc == TK[J-1]) || J >= 20 || game.newloc == game.dloc[I] ||
		   FORCED(game.newloc) || (I == 6 && CNDBIT(game.newloc,3)) ||
		   labs(TRAVEL[kk])/1000000 == 100)
		    goto L6014;
		TK[J]=game.newloc;
		J=J+1;
	    L6014:
		kk=kk+1;
	    } while
		(TRAVEL[kk-1] >= 0);
	TK[J]=game.odloc[I];
	if(J >= 2)J=J-1;
	J=1+randrange(J);
	game.odloc[I]=game.dloc[I];
	game.dloc[I]=TK[J];
	game.dseen[I]=(game.dseen[I] && INDEEP(game.loc)) || (game.dloc[I] == game.loc || game.odloc[I] == game.loc);
	if(!game.dseen[I]) continue;
	game.dloc[I]=game.loc;
	if(I == PIRATE) {
	    /*  The pirate's spotted him.  He leaves him alone once we've
	     *  found chest.  K counts if a treasure is here.  If not, and
	     *  tally=1 for an unseen chest, let the pirate be spotted.
	     *  Note that game.place(CHEST)=0 might mean that he's thrown
	     *  it to the troll, but in that case he's seen the chest
	     *  (game.prop=0). */
	    if(game.loc == game.chloc || game.prop[CHEST] >= 0) continue;
	    K=0;
	    for (J=50; J<=MAXTRS; J++) {
               /*  Pirate won't take pyramid from plover room or dark
		*  room (too easy!). */
		if(J == PYRAM && (game.loc == PLAC[PYRAM] || game.loc == PLAC[EMRALD]))
		    goto L6020;
		if(TOTING(J))
		    goto L6021;
	    L6020:
		if(HERE(J))K=1;
	    } /* end loop */
	    if(game.tally == 1 && K == 0 && game.place[CHEST] == 0 && HERE(LAMP) && game.prop[LAMP] == 1)
		goto L6025;
	    if(game.odloc[PIRATE] != game.dloc[PIRATE] && PCT(20))
		RSPEAK(127);
	    continue;

	L6021:	if(game.place[CHEST] != 0) goto L6022;
	    /*  Install chest only once, to insure it is the last treasure in
	     *  the list. */
	    MOVE(CHEST,game.chloc);
	    MOVE(MESSAG,game.chloc2);
	L6022:	RSPEAK(128);
	    /* 6023 */ for (J=50; J<=MAXTRS; J++) {
		if(J == PYRAM && (game.loc == PLAC[PYRAM] || game.loc == PLAC[EMRALD])) goto L6023;
		if(AT(J) && game.fixed[J] == 0)CARRY(J,game.loc);
		if(TOTING(J))DROP(J,game.chloc);
	    L6023:	/*etc*/ ;
	    }
	L6024:
	    game.dloc[PIRATE]=game.chloc;
	    game.odloc[PIRATE]=game.chloc;
	    game.dseen[PIRATE]=false;
	    continue;

	L6025:
	    RSPEAK(186);
	    MOVE(CHEST,game.chloc);
	    MOVE(MESSAG,game.chloc2);
	    goto L6024;

	}

	/* This threatening little dwarf is in the room with him! */
	++game.dtotal;
	if(game.odloc[I] == game.dloc[I]) {
	    ++attack;
	    if(game.knfloc >= 0)
		game.knfloc=game.loc;
	    if(randrange(1000) < 95*(game.dflag-2))
		++stick;
	}
    }

    /*  Now we know what's happening.  Let's tell the poor sucker about it.
     *  Note that various of the "knife" messages must have specific relative
     *  positions in the RSPEAK database. */
    if(game.dtotal == 0)
	return true;
    SETPRM(1,game.dtotal,0);
    RSPEAK(4+1/game.dtotal);
    if(attack == 0)
	return true;
    if(game.dflag == 2)game.dflag=3;
    SETPRM(1,attack,0);
    K=6;
    if(attack > 1)K=250;
    RSPEAK(K);
    SETPRM(1,stick,0);
    RSPEAK(K+1+2/(1+stick));
    if(stick == 0)
	return true;
    game.oldlc2=game.loc;
    return false;
}

static bool do_command(FILE *cmdin) {
	long LL, KQ, VERB, KK, K2, V1, V2;
	long obj, i;
	static long IGO = 0;

	/*  Can't leave cave once it's closing (except by main office). */
	if(OUTSID(game.newloc) && game.newloc != 0 && game.closng) {
	    RSPEAK(130);
	    game.newloc=game.loc;
	    if(!game.panic)game.clock2=15;
	    game.panic=true;
	}

	/*  See if a dwarf has seen him and has come from where he
	 *  wants to go.  If so, the dwarf's blocking his way.  If
	 *  coming from place forbidden to pirate (dwarves rooted in
	 *  place) let him get out (and attacked). */
	if(game.newloc != game.loc && !FORCED(game.loc) && !CNDBIT(game.loc,3)) {
		for (i=1; i<=NDWARVES-1; i++) {
		    if(game.odloc[i] == game.newloc && game.dseen[i]) {
			game.newloc=game.loc;
			RSPEAK(2);
			break;
		    }
		}
	}
	game.loc=game.newloc;

	if (!dwarfmove())
	    goto L99;

/*  Describe the current location and (maybe) get next command. */

/*  Print text for current loc. */

L2000:	if(game.loc == 0) goto L99;
	KK=STEXT[game.loc];
	if(MOD(game.abbrev[game.loc],game.abbnum) == 0 || KK == 0)KK=LTEXT[game.loc];
	if(FORCED(game.loc) || !DARK(0)) goto L2001;
	if(game.wzdark && PCT(35)) goto L90;
	KK=RTEXT[16];
L2001:	if(TOTING(BEAR))RSPEAK(141);
	SPEAK(KK);
	K=1;
	if(FORCED(game.loc)) goto L8;
	if(game.loc == 33 && PCT(25) && !game.closng)RSPEAK(7);

/*  Print out descriptions of objects at this location.  If not closing and
 *  property value is negative, tally off another treasure.  Rug is special
 *  case; once seen, its game.prop is 1 (dragon on it) till dragon is killed.
 *  Similarly for chain; game.prop is initially 1 (locked to bear).  These hacks
 *  are because game.prop=0 is needed to get full score. */

	if(DARK(0)) goto L2012;
	game.abbrev[game.loc]=game.abbrev[game.loc]+1;
	I=game.atloc[game.loc];
L2004:	if(I == 0) goto L2012;
	obj=I;
	if(obj > NOBJECTS)obj=obj-NOBJECTS;
	if(obj == STEPS && TOTING(NUGGET)) goto L2008;
	if(game.prop[obj] >= 0) goto L2006;
	if(game.closed) goto L2008;
	game.prop[obj]=0;
	if(obj == RUG || obj == CHAIN)game.prop[obj]=1;
	game.tally=game.tally-1;
/*  Note: There used to be a test here to see whether the player had blown it
 *  so badly that he could never ever see the remaining treasures, and if so
 *  the lamp was zapped to 35 turns.  But the tests were too simple-minded;
 *  things like killing the bird before the snake was gone (can never see
 *  jewelry), and doing it "right" was hopeless.  E.G., could cross troll
 *  bridge several times, using up all available treasures, breaking vase,
 *  using coins to buy batteries, etc., and eventually never be able to get
 *  across again.  If bottle were left on far side, could then never get eggs
 *  or trident, and the effects propagate.  So the whole thing was flushed.
 *  anyone who makes such a gross blunder isn't likely to find everything
 *  else anyway (so goes the rationalisation). */
L2006:	KK=game.prop[obj];
	if(obj == STEPS && game.loc == game.fixed[STEPS])KK=1;
	PSPEAK(obj,KK);
L2008:	I=game.link[I];
	 goto L2004;

L2009:	K=54;
L2010:	SPK=K;
L2011:	RSPEAK(SPK);

L2012:	VERB=0;
	game.oldobj=obj;
	obj=0;

/*  Check if this loc is eligible for any hints.  If been here long enough,
 *  branch to help section (on later page).  Hints all come back here eventually
 *  to finish the loop.  Ignore "HINTS" < 4 (special stuff, see database notes).
 */
L2600:	if(COND[game.loc] < game.conds) goto L2603;
	/* 2602 */ for (HINT=1; HINT<=HNTMAX; HINT++) {
	if(game.hinted[HINT]) goto L2602;
	if(!CNDBIT(game.loc,HINT+10))game.hintlc[HINT]= -1;
	game.hintlc[HINT]=game.hintlc[HINT]+1;
	if(game.hintlc[HINT] >= HINTS[HINT][1]) goto L40000;
L2602:	/*etc*/ ;
	} /* end loop */

	/*  If closing time, check for any objects being toted with
	 *  game.prop < 0 and set the prop to -1-game.prop.  This way
	 *  objects won't be described until they've been picked up
	 *  and put down separate from their respective piles.  Don't
	 *  tick game.clock1 unless well into cave (and not at Y2). */
L2603:	if(game.closed) {
	    if(game.prop[OYSTER] < 0 && TOTING(OYSTER))
		PSPEAK(OYSTER,1);
	    for (i=1; i<=NOBJECTS; i++) {
		if(TOTING(i) && game.prop[i] < 0)
		    game.prop[i] = -1-game.prop[i];
	    }
	}
	game.wzdark=DARK(0);
	if(game.knfloc > 0 && game.knfloc != game.loc)
	    game.knfloc=0;

	/* This is where we get a new command from the user */
	if (!GETIN(cmdin, WD1,WD1X,WD2,WD2X))
	    return false;

	/*  Every input, check "game.foobar" flag.  If zero, nothing's
	 *  going on.  If pos, make neg.  If neg, he skipped a word,
	 *  so make it zero. */
L2607:	game.foobar=(game.foobar>0 ? -game.foobar : 0);
	game.turns=game.turns+1;
	if(game.turns == game.thresh) {
	SPEAK(TTEXT[game.trndex]);
	game.trnluz=game.trnluz+TRNVAL[game.trndex]/100000;
	game.trndex=game.trndex+1;
	game.thresh= -1;
	if(game.trndex <= TRNVLS)
	    game.thresh=MOD(TRNVAL[game.trndex],100000)+1;
	}
	if(VERB == SAY && WD2 > 0)VERB=0;
	if(VERB == SAY) goto L4090;
	if(game.tally == 0 && INDEEP(game.loc) && game.loc != 33)game.clock1=game.clock1-1;
	if(game.clock1 == 0) goto L10000;
	if(game.clock1 < 0)game.clock2=game.clock2-1;
	if(game.clock2 == 0) goto L11000;
	if(game.prop[LAMP] == 1)game.limit=game.limit-1;
	if(game.limit <= 30 && HERE(BATTER) && game.prop[BATTER] == 0 && HERE(LAMP)) goto
		L12000;
	if(game.limit == 0) goto L12400;
	if(game.limit <= 30) goto L12200;
L19999: K=43;
	if(LIQLOC(game.loc) == WATER)K=70;
	V1=VOCAB(WD1,-1);
	V2=VOCAB(WD2,-1);
	if(V1 == ENTER && (V2 == STREAM || V2 == 1000+WATER)) goto L2010;
	if(V1 == ENTER && WD2 > 0) goto L2800;
	if((V1 != 1000+WATER && V1 != 1000+OIL) || (V2 != 1000+PLANT && V2 !=
		1000+DOOR)) goto L2610;
	{long x = V2-1000; if(AT(x))WD2=MAKEWD(16152118);}
L2610:	if(V1 == 1000+CAGE && V2 == 1000+BIRD && HERE(CAGE) && HERE(BIRD))
		WD1=MAKEWD(301200308);
L2620:	if(WD1 == MAKEWD(23051920)) {
		game.iwest=game.iwest+1;
		if(game.iwest == 10)RSPEAK(17);
	}
	if(WD1 != MAKEWD( 715) || WD2 == 0) goto L2630;
	IGO=IGO+1;
	if(IGO == 10)RSPEAK(276);
L2630:	I=VOCAB(WD1,-1);
	if(I == -1) goto L3000;
	K=MOD(I,1000);
	KQ=I/1000+1;
	 switch (KQ-1) { case 0: goto L8; case 1: goto L5000; case 2: goto L4000;
		case 3: goto L2010; }
	BUG(22);

/*  Get second word for analysis. */

L2800:	WD1=WD2;
	WD1X=WD2X;
	WD2=0;
	 goto L2620;

/*  Gee, I don't understand. */

L3000:	SETPRM(1,WD1,WD1X);
	 if (fallback_handler(rawbuf))
	     return true;
	RSPEAK(254);
	 goto L2600;

/* Verb and object analysis moved to separate module. */

L4000:	I=4000; VERB=K; goto Laction;
L4090:	I=4090; goto Laction;
L5000:	I=5000;
Laction:
	 switch (action(cmdin, I, VERB, obj)) {
	   case 2: return true;
	   case 8: goto L8;
	   case 2000: goto L2000;
	   case 2009: goto L2009;
	   case 2010: goto L2010;
	   case 2011: goto L2011;
	   case 2012: goto L2012;
	   case 2600: goto L2600;
	   case 2607: goto L2607;
	   case 2630: goto L2630;
	   case 2800: goto L2800;
	   case 8000: goto L8000;
	   case 18999: goto L18999;
	   case 19000: goto L19000;
	   }
	BUG(99);

/*  Random intransitive verbs come here.  Clear obj just in case (see "attack").
		*/

L8000:	SETPRM(1,WD1,WD1X);
	RSPEAK(257);
	obj=0;
	goto L2600;

/*  Figure out the new location
 *
 *  Given the current location in "game.loc", and a motion verb number in
 *  "K", put the new location in "game.newloc".  The current loc is saved
 *  in "game.oldloc" in case he wants to retreat.  The current
 *  game.oldloc is saved in game.oldlc2, in case he dies.  (if he
 *  does, game.newloc will be limbo, and OLgame.dloc will be what killed
 *  him, so we need game.oldlc2, which is the last place he was
 *  safe.) */

L8:	KK=KEY[game.loc];
	game.newloc=game.loc;
	if(KK == 0)BUG(26);
	if(K == NUL) return true;
	if(K == BACK) goto L20;
	if(K == LOOK) goto L30;
	if(K == CAVE) goto L40;
	game.oldlc2=game.oldloc;
	game.oldloc=game.loc;

L9:	LL=labs(TRAVEL[KK]);
	if(MOD(LL,1000) == 1 || MOD(LL,1000) == K) goto L10;
	if(TRAVEL[KK] < 0) goto L50;
	KK=KK+1;
	 goto L9;

L10:	LL=LL/1000;
L11:	game.newloc=LL/1000;
	 K=MOD(game.newloc,100);	/* ESR: an instance of NOBJECTS? */
	if(game.newloc <= 300) goto L13;
	if(game.prop[K] != game.newloc/100-3) goto L16;
L12:	if(TRAVEL[KK] < 0)BUG(25);
	KK=KK+1;
	game.newloc=labs(TRAVEL[KK])/1000;
	if(game.newloc == LL) goto L12;
	LL=game.newloc;
	 goto L11;

L13:	if(game.newloc <= 100) goto L14;	/* ESR: an instance of NOBJECTS? */
	if(TOTING(K) || (game.newloc > 200 && AT(K))) goto L16;
	 goto L12;

L14:	if(game.newloc != 0 && !PCT(game.newloc)) goto L12;
L16:	game.newloc=MOD(LL,1000);
	if(game.newloc <= 300) return true;
	if(game.newloc <= 500) goto L30000;
	RSPEAK(game.newloc-500);
	game.newloc=game.loc;
	 return true;

/*  Special motions come here.  Labelling convention: statement numbers NNNXX
 *  (XX=00-99) are used for special case number NNN (NNN=301-500). */

L30000: game.newloc=game.newloc-300;
	 switch (game.newloc) { case 1: goto L30100; case 2: goto L30200; case 3: goto
		L30300; }
	BUG(20);

/*  Travel 301.  Plover-alcove passage.  Can carry only emerald.  Note: travel
 *  table must include "useless" entries going through passage, which can never
 *  be used for actual motion, but can be spotted by "go back". */

L30100: game.newloc=99+100-game.loc;	/* ESR: an instance of NOBJECTS? */
	if(game.holdng == 0 || (game.holdng == 1 && TOTING(EMRALD))) return true;
	game.newloc=game.loc;
	RSPEAK(117);
	return true;

/*  Travel 302.  Plover transport.  Drop the emerald (only use special travel if
 *  toting it), so he's forced to use the plover-passage to get it out.  Having
 *  dropped it, go back and pretend he wasn't carrying it after all. */

L30200: DROP(EMRALD,game.loc);
	 goto L12;

/*  Travel 303.  Troll bridge.  Must be done only as special motion so that
 *  dwarves won't wander across and encounter the bear.  (They won't follow the
 *  player there because that region is forbidden to the pirate.)  If
 *  game.prop(TROLL)=1, he's crossed since paying, so step out and block him.
 *  (standard travel entries check for game.prop(TROLL)=0.)  Special stuff for bear. */

L30300: if(game.prop[TROLL] != 1) goto L30310;
	PSPEAK(TROLL,1);
	game.prop[TROLL]=0;
	MOVE(TROLL2,0);
	MOVE(TROLL2+NOBJECTS,0);
	MOVE(TROLL,PLAC[TROLL]);
	MOVE(TROLL+NOBJECTS,FIXD[TROLL]);
	JUGGLE(CHASM);
	game.newloc=game.loc;
	return true;

L30310: game.newloc=PLAC[TROLL]+FIXD[TROLL]-game.loc;
	if(game.prop[TROLL] == 0)game.prop[TROLL]=1;
	if(!TOTING(BEAR)) return true;
	RSPEAK(162);
	game.prop[CHASM]=1;
	game.prop[TROLL]=2;
	DROP(BEAR,game.newloc);
	game.fixed[BEAR]= -1;
	game.prop[BEAR]=3;
	game.oldlc2=game.newloc;
	 goto L99;

/*  End of specials. */

/*  Handle "go back".  Look for verb which goes from game.loc to
 *  game.oldloc, or to game.oldlc2 If game.oldloc has forced-motion.
 *  K2 saves entry -> forced loc -> previous loc. */

L20:	K=game.oldloc;
	if(FORCED(K))K=game.oldlc2;
	game.oldlc2=game.oldloc;
	game.oldloc=game.loc;
	K2=0;
	if(K == game.loc)K2=91;
	if(CNDBIT(game.loc,4))K2=274;
	if(K2 == 0) goto L21;
	RSPEAK(K2);
	return true;

L21:	LL=MOD((labs(TRAVEL[KK])/1000),1000);
	if(LL != K) {
		if(LL <= 300) {
			J=KEY[LL];
			if(FORCED(LL) && MOD((labs(TRAVEL[J])/1000),1000) == K)
				K2=KK;
		}
		if(TRAVEL[KK] < 0) goto L23;
		KK=KK+1;
		goto L21;

L23:		KK=K2;
		if(KK == 0) {
			RSPEAK(140);
			return true;
		}
	}

	K=MOD(labs(TRAVEL[KK]),1000);
	KK=KEY[game.loc];
	 goto L9;

/*  Look.  Can't give more detail.  Pretend it wasn't dark (though it may "now"
 *  be dark) so he won't fall into a pit while staring into the gloom. */

L30:	if(game.detail < 3)RSPEAK(15);
	game.detail=game.detail+1;
	game.wzdark=false;
	game.abbrev[game.loc]=0;
	return true;

/*  Cave.  Different messages depending on whether above ground. */

L40:	K=58;
	if(OUTSID(game.loc) && game.loc != 8)K=57;
	RSPEAK(K);
	return true;

/*  Non-applicable motion.  Various messages depending on word given. */

L50:	SPK=12;
	if(K >= 43 && K <= 50)SPK=52;
	if(K == 29 || K == 30)SPK=52;
	if(K == 7 || K == 36 || K == 37)SPK=10;
	if(K == 11 || K == 19)SPK=11;
	if(VERB == FIND || VERB == INVENT)SPK=59;
	if(K == 62 || K == 65)SPK=42;
	if(K == 17)SPK=80;
	RSPEAK(SPK);
	return true;

/*  "You're dead, Jim."
 *
 *  If the current loc is zero, it means the clown got himself killed.  We'll
 *  allow this maxdie times.  MAXDIE is automatically set based on the number of
 *  snide messages available.  Each death results in a message (81, 83, etc.)
 *  which offers reincarnation; if accepted, this results in message 82, 84,
 *  etc.  The last time, if he wants another chance, he gets a snide remark as
 *  we exit.  When reincarnated, all objects being carried get dropped at game.oldlc2
 *  (presumably the last place prior to being killed) without change of props.
 *  the loop runs backwards to assure that the bird is dropped before the cage.
 *  (this kluge could be changed once we're sure all references to bird and cage
 *  are done by keywords.)  The lamp is a special case (it wouldn't do to leave
 *  it in the cave).  It is turned off and left outside the building (only if he
 *  was carrying it, of course).  He himself is left inside the building (and
 *  heaven help him if he tries to xyzzy back into the cave without the lamp!).
 *  game.oldloc is zapped so he can't just "retreat". */

/*  The easiest way to get killed is to fall into a pit in pitch darkness. */

L90:	RSPEAK(23);
	game.oldlc2=game.loc;

/*  Okay, he's dead.  Let's get on with it. */

L99:	if(game.closng) {
	/*  He died during closing time.  No resurrection.  Tally up a
	 *  death and exit. */
	    RSPEAK(131);
	    ++game.numdie;
	    score(0);
	} else {
	    ++game.numdie;
	    if(!YES(cmdin,79+game.numdie*2,80+game.numdie*2,54))
		score(0);
	    if(game.numdie == MAXDIE)
		score(0);
	    game.place[WATER]=0;
	    game.place[OIL]=0;
	    if(TOTING(LAMP))
		game.prop[LAMP]=0;
	    for (J=1; J<=NOBJECTS; J++) {
		I=NOBJECTS + 1 - J;
		if(TOTING(I)) {
		    K=game.oldlc2;
		    if(I == LAMP)
			K=1;
		    DROP(I,K);
		}
	    }
	    game.loc=3;
	    game.oldloc=game.loc;
	    goto L2000;
	}

/*  Hints */

/*  Come here if he's been long enough at required loc(s) for some unused hint.
 *  hint number is in variable "hint".  Branch to quick test for additional
 *  conditions, then come back to do neat stuff.  Goto 40010 if conditions are
 *  met and we want to offer the hint.  Goto 40020 to clear game.hintlc back to zero,
 *  40030 to take no action yet. */

L40000:    switch (HINT-1) { case 0: goto L40100; case 1: goto L40200; case 2: goto
		L40300; case 3: goto L40400; case 4: goto L40500; case 5: goto
		L40600; case 6: goto L40700; case 7: goto L40800; case 8: goto
		L40900; case 9: goto L41000; }
/*		CAVE  BIRD  SNAKE MAZE  DARK  WITT  URN   WOODS OGRE
 *		JADE */
	BUG(27);

L40010: game.hintlc[HINT]=0;
	if(!YES(cmdin,HINTS[HINT][3],0,54)) goto L2602;
	SETPRM(1,HINTS[HINT][2],HINTS[HINT][2]);
	RSPEAK(261);
	game.hinted[HINT]=YES(cmdin,175,HINTS[HINT][4],54);
	if(game.hinted[HINT] && game.limit > 30)game.limit=game.limit+30*HINTS[HINT][2];
L40020: game.hintlc[HINT]=0;
L40030:  goto L2602;

/*  Now for the quick tests.  See database description for one-line notes. */

L40100: if(game.prop[GRATE] == 0 && !HERE(KEYS)) goto L40010;
	 goto L40020;

L40200: if(game.place[BIRD] == game.loc && TOTING(ROD) && game.oldobj == BIRD) goto L40010;
	 goto L40030;

L40300: if(HERE(SNAKE) && !HERE(BIRD)) goto L40010;
	 goto L40020;

L40400: if(game.atloc[game.loc] == 0 && game.atloc[game.oldloc] == 0 && game.atloc[game.oldlc2] == 0 && game.holdng >
		1) goto L40010;
	 goto L40020;

L40500: if(game.prop[EMRALD] != -1 && game.prop[PYRAM] == -1) goto L40010;
	 goto L40020;

L40600:  goto L40010;

L40700: if(game.dflag == 0) goto L40010;
	 goto L40020;

L40800: if(game.atloc[game.loc] == 0 && game.atloc[game.oldloc] == 0 && game.atloc[game.oldlc2] == 0) goto
		L40010;
	 goto L40030;

L40900: I=ATDWRF(game.loc);
	if(I < 0) goto L40020;
	if(HERE(OGRE) && I == 0) goto L40010;
	 goto L40030;

L41000: if(game.tally == 1 && game.prop[JADE] < 0) goto L40010;
	 goto L40020;





/*  Cave closing and scoring */


/*  These sections handle the closing of the cave.  The cave closes "clock1"
 *  turns after the last treasure has been located (including the pirate's
 *  chest, which may of course never show up).  Note that the treasures need not
 *  have been taken yet, just located.  Hence clock1 must be large enough to get
 *  out of the cave (it only ticks while inside the cave).  When it hits zero,
 *  we branch to 10000 to start closing the cave, and then sit back and wait for
 *  him to try to get out.  If he doesn't within clock2 turns, we close the
 *  cave; if he does try, we assume he panics, and give him a few additional
 *  turns to get frantic before we close.  When clock2 hits zero, we branch to
 *  11000 to transport him into the final puzzle.  Note that the puzzle depends
 *  upon all sorts of random things.  For instance, there must be no water or
 *  oil, since there are beanstalks which we don't want to be able to water,
 *  since the code can't handle it.  Also, we can have no keys, since there is a
 *  grate (having moved the fixed object!) there separating him from all the
 *  treasures.  Most of these problems arise from the use of negative prop
 *  numbers to suppress the object descriptions until he's actually moved the
 *  objects. */

/*  When the first warning comes, we lock the grate, destroy the bridge, kill
 *  all the dwarves (and the pirate), remove the troll and bear (unless dead),
 *  and set "closng" to true.  Leave the dragon; too much trouble to move it.
 *  from now until clock2 runs out, he cannot unlock the grate, move to any
 *  location outside the cave, or create the bridge.  Nor can he be
 *  resurrected if he dies.  Note that the snake is already gone, since he got
 *  to the treasure accessible only via the hall of the mountain king. Also, he's
 *  been in giant room (to get eggs), so we can refer to it.  Also also, he's
 *  gotten the pearl, so we know the bivalve is an oyster.  *And*, the dwarves
 *  must have been activated, since we've found chest. */

L10000: game.prop[GRATE]=0;
	game.prop[FISSUR]=0;
	for (I=1; I<=NDWARVES; I++) {
	game.dseen[I]=false;
	game.dloc[I]=0;
	} /* end loop */
	MOVE(TROLL,0);
	MOVE(TROLL+NOBJECTS,0);
	MOVE(TROLL2,PLAC[TROLL]);
	MOVE(TROLL2+NOBJECTS,FIXD[TROLL]);
	JUGGLE(CHASM);
	if(game.prop[BEAR] != 3)DSTROY(BEAR);
	game.prop[CHAIN]=0;
	game.fixed[CHAIN]=0;
	game.prop[AXE]=0;
	game.fixed[AXE]=0;
	RSPEAK(129);
	game.clock1= -1;
	game.closng=true;
	 goto L19999;

/*  Once he's panicked, and clock2 has run out, we come here to set up the
 *  storage room.  The room has two locs, hardwired as 115 (ne) and 116 (sw).
 *  At the ne end, we place empty bottles, a nursery of plants, a bed of
 *  oysters, a pile of lamps, rods with stars, sleeping dwarves, and him.  At
 *  the sw end we place grate over treasures, snake pit, covey of caged birds,
 *  more rods, and pillows.  A mirror stretches across one wall.  Many of the
 *  objects come from known locations and/or states (e.g. the snake is known to
 *  have been destroyed and needn't be carried away from its old "place"),
 *  making the various objects be handled differently.  We also drop all other
 *  objects he might be carrying (lest he have some which could cause trouble,
 *  such as the keys).  We describe the flash of light and trundle back. */

L11000: game.prop[BOTTLE]=PUT(BOTTLE,115,1);
	game.prop[PLANT]=PUT(PLANT,115,0);
	game.prop[OYSTER]=PUT(OYSTER,115,0);
	OBJTXT[OYSTER]=3;
	game.prop[LAMP]=PUT(LAMP,115,0);
	game.prop[ROD]=PUT(ROD,115,0);
	game.prop[DWARF]=PUT(DWARF,115,0);
	game.loc=115;
	game.oldloc=115;
	game.newloc=115;

/*  Leave the grate with normal (non-negative) property.  Reuse sign. */

	I=PUT(GRATE,116,0);
	I=PUT(SIGN,116,0);
	OBJTXT[SIGN]=OBJTXT[SIGN]+1;
	game.prop[SNAKE]=PUT(SNAKE,116,1);
	game.prop[BIRD]=PUT(BIRD,116,1);
	game.prop[CAGE]=PUT(CAGE,116,0);
	game.prop[ROD2]=PUT(ROD2,116,0);
	game.prop[PILLOW]=PUT(PILLOW,116,0);

	game.prop[MIRROR]=PUT(MIRROR,115,0);
	game.fixed[MIRROR]=116;

	for (I=1; I<=NOBJECTS; I++) {
		if(TOTING(I))
			DSTROY(I);
	} /* end loop */

	RSPEAK(132);
	game.closed=true;
	return true;

/*  Another way we can force an end to things is by having the lamp give out.
 *  When it gets close, we come here to warn him.  We go to 12000 if the lamp
 *  and fresh batteries are here, in which case we replace the batteries and
 *  continue.  12200 is for other cases of lamp dying.  12400 is when it goes
 *  out.  Even then, he can explore outside for a while if desired. */

L12000: RSPEAK(188);
	game.prop[BATTER]=1;
	if(TOTING(BATTER))DROP(BATTER,game.loc);
	game.limit=game.limit+2500;
	game.lmwarn=false;
	 goto L19999;

L12200: if(game.lmwarn || !HERE(LAMP)) goto L19999;
	game.lmwarn=true;
	SPK=187;
	if(game.place[BATTER] == 0)SPK=183;
	if(game.prop[BATTER] == 1)SPK=189;
	RSPEAK(SPK);
	 goto L19999;

L12400: game.limit= -1;
	game.prop[LAMP]=0;
	if(HERE(LAMP))RSPEAK(184);
	 goto L19999;

/*  Oh dear, he's disturbed the dwarves. */

L18999: RSPEAK(SPK);
L19000: RSPEAK(136);
	score(0);
	return true;
}
