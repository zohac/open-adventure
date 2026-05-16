import 'package:open_adventure/domain/entities/dwarf.dart';
import 'package:open_adventure/domain/entities/item.dart';
import 'package:open_adventure/domain/entities/location.dart';
import 'package:open_adventure/domain/entities/player.dart';
import 'package:open_adventure/domain/entities/game.dart';
import 'package:open_adventure/domain/repositories/game_repository.dart';

class GameRepositoryImpl implements GameRepository {
  Game _gameState;

  GameRepositoryImpl(this._gameState);

  @override
  Future<Dwarf> getDwarfById(int id) {
    // TODO: implement getDwarfById
    throw UnimplementedError();
  }

  @override
  Future<Game> getGameState() {
    // TODO: implement getGameState
    throw UnimplementedError();
  }

  @override
  Future<Item> getItemById(int id) {
    // TODO: implement getItemById
    throw UnimplementedError();
  }

  @override
  Future<Location> getLocationById(int id) {
    // TODO: implement getLocationById
    throw UnimplementedError();
  }

  @override
  Future<Player> getPlayer() {
    // TODO: implement getPlayer
    throw UnimplementedError();
  }

  @override
  Future<void> updateDwarf(Dwarf dwarf) {
    // TODO: implement updateDwarf
    throw UnimplementedError();
  }

  @override
  Future<void> updateGameState(Game game) {
    // TODO: implement updateGameState
    throw UnimplementedError();
  }

  @override
  Future<void> updateItem(Item item) {
    // TODO: implement updateItem
    throw UnimplementedError();
  }

  @override
  Future<void> updateLocation(Location location) {
    // TODO: implement updateLocation
    throw UnimplementedError();
  }

  @override
  Future<void> updatePlayer(Player player) {
    // TODO: implement updatePlayer
    throw UnimplementedError();
  }

  @override
  Future<void> saveGame(Game game) async {
    // TODO: implement saveGame
    throw UnimplementedError();
  }

  @override
  Future<Game> loadGame() async {
    // TODO: implement loadGame
    throw UnimplementedError();
  }
  
}
