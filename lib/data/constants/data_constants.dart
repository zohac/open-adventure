import 'package:open_adventure/domain/enums/locations_ref.dart';

class DataConstants {
  static const int lcgA = 1093;
  static const int lcgC = 221587;
  static const int lcgM = 1048576;

  static const int lineSize = 1024;
  static const int tokenLength = 5;
  static const int numberOfDwarves = 6;
  static const int pirateIndex = numberOfDwarves;
  static const int carried = -1;
  static const int isFixed = -1;
  static const int isFree = 0;
  static const int saveVersion = 31;
  static const String adventMagic = 'open-adventure\n';

  static const LocationRefs daltLC = LocationRefs.locNugget;  // alternate dwarf location
  static const int invLimit = 7;                              // inventory limit (# of objects)
  static const int intransitive = -1;                         // illegal object number
  static const int gameLimit = 330;                           // base limit of turns
  static const int noviceLimit = 1000;                        // limit of turns for novice
  static const int warnTime = 30;                             // late game starts at game.limit-this
  static const int flashTime = 50;                            // turns from first warning till blinding flash
  static const int panicTime = 15;                            // time left after closing
  static const int batteryLife = 2500;                        // turn limit increment from batteries
  static const int wordNotFound = -1;                         // "Word not found" flag value for the vocab hash functions.
  static const int wordEmpty = 0;                             // "Word empty" flag value for the vocab hash functions
  static const int pitKillProb = 35;	                        // Percentage probability of dying from fall in pit.
  static const String readMode = "rb";                        // b is not needed for POSIX but harmless
  static const String writeMode = "wb";                      // b is not needed for POSIX but harmless
}
