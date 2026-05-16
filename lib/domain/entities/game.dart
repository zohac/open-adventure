import 'package:equatable/equatable.dart';
import 'package:open_adventure/data/constants/data_constants.dart';
import 'package:open_adventure/domain/enums/locations_ref.dart';

import 'hint.dart';
import 'player.dart';
import 'location.dart';
import 'item.dart';
import 'dwarf.dart';

class Game extends Equatable {
  int lcgX = 0;
  int abbnum = 5;                                       // How often to print int descriptions
  int? bonus;                                            // What kind of finishing bonus we are getting
  LocationRefs chloc = LocationRefs.locMazeend12;       // pirate chest location
  LocationRefs chloc2 = LocationRefs.locDeadend13;      // pirate chest alternate location
  int clock1 = DataConstants.warnTime;                  // # turns from finding last treasure to close
  int clock2 = DataConstants.flashTime;                 // # turns from warning till blinding flash
  bool? clshnt; // has player read the clue in the endgame?
  bool? closed; // whether we're all the way closed
  bool? closng; // whether it's closing time yet
  bool? lmwarn; // has player been warned about lamp going dim?
  bool? novice; // asked for instructions at start-up?
  bool? panic; // has player found out he's trapped?
  bool? wzdark; // whether the loc he's leaving was dark
  bool? blooded; // has player drunk of dragon's blood?
  int? conds; // min value for cond[loc] if loc has any hints
  int? detail; // level of detail in descriptions
  int? dflag; // dwarf activity level
  int? dkill; // dwarves killed
  int? dtotal; // total dwarves in loc
  int? foobar = DataConstants.gameLimit;                 // progress in saying "FEE FIE FOE FOO"
  int? holdng; // number of objects being carried
  int? igo; // # uses of "go" instead of a direction
  int? iwest; // # times he's said "west" instead of "w"
  int? knfloc; // knife location
  int limit = DataConstants.gameLimit;                  // lifetime of lamp
  LocationRefs loc = LocationRefs.locStart;             // where player is now
  LocationRefs newloc = LocationRefs.locStart;          // where player is going
  int? numdie; // number of times killed so far
  int? oldloc; // where player was
  int? oldlc2; // where player was two moves ago
  int? oldobj; // last object player handled
  int? saved; // point penalty for saves
  int? tally; // count of treasures gained
  int? thresh; // current threshold for endgame scoring tier
  bool? seenbigwords; // have we read the graffiti in the Giant's Room?
  int? trnluz; // # points lost due to turns used
  int? turns; // counts commands given
  String? zzWord;                                        // randomly generated magic word from bird

  // Structures complexes
  Player? player;
  Map<int, Location>? locations;                   // ID -> Location
  Map<int, Item>? items;                           // ID -> Item
  Map<int, Dwarf>? dwarves;                        // ID -> Dwarf
  Map<int, Hint>? hints;                           // Hints
  List<int>? link;                                 // Object-list links

  Game();

  @override
  // TODO: implement props
  List<Object?> get props => throw UnimplementedError();
  // List<Object> get props => [player, locations, items, dwarves, flags];

  void addDwarf(Dwarf dwarf) {
    dwarves?[dwarf.id] = dwarf;
  }

  Dwarf? getDwarf(int id) {
    return dwarves?[id];
  }

  // final Player player;
  // final Map<int, Location> locations; // ID -> Location
  // final Map<int, Item> items;         // ID -> Item
  // final Map<int, Dwarf> dwarves;      // ID -> Dwarf
  // final Map<String, dynamic> flags;
  //
  // const Game({
  //   required this.player,
  //   required this.locations,
  //   required this.items,
  //   required this.dwarves,
  //   this.flags = const {},
  // });

  // Game copyWith({
  //   Player? player,
  //   Map<int, Location>? locations,
  //   Map<int, Item>? items,
  //   Map<int, Dwarf>? dwarves,
  //   Map<String, dynamic>? flags,
  // }) {
  //   return Game(
  //     player: player ?? this.player,
  //     locations: locations ?? this.locations,
  //     items: items ?? this.items,
  //     dwarves: dwarves ?? this.dwarves,
  //     flags: flags ?? this.flags,
  //   );
  // }
  //
  // // Méthodes pour gérer l'état du jeu
  // // Exemple : obtenir un item par son nom
  // // Méthode pour obtenir un item par son nom
  // Item? getItemByName(String name) {
  //   return items.values.firstWhereOrNull(
  //         (item) => item.name.toLowerCase() == name.toLowerCase(),
  //   );
  // }
  //
  // // Méthode pour obtenir les items d'un lieu
  // List<Item> getItemsInLocation(int locationId) {
  //   Location location = locations[locationId]!;
  //   return location.items.map((itemId) => items[itemId]!).toList();
  // }
  //
  // // Obtenir la location actuelle du joueur
  // Location get currentLocation => player.currentLocation;
}
