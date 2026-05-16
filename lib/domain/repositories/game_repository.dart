import '../entities/game.dart';
import '../entities/player.dart';
import '../entities/location.dart';
import '../entities/item.dart';
import '../entities/dwarf.dart';

abstract class GameRepository {
  Future<Location> getLocationById(int id);
  Future<void> updateLocation(Location location);

  Future<Item> getItemById(int id);
  Future<void> updateItem(Item item);

  Future<Player> getPlayer();
  Future<void> updatePlayer(Player player);

  Future<Dwarf> getDwarfById(int id);
  Future<void> updateDwarf(Dwarf dwarf);

  Future<Game> getGameState();
  Future<void> updateGameState(Game game);

  Future<void> saveGame(Game game);
  Future<Game> loadGame();
}
