import '../repositories/game_repository.dart';
import '../entities/command.dart';
import '../entities/item.dart';

class TakeItemUseCase {
  final GameRepository repository;

  TakeItemUseCase({required this.repository});

  Future<void> execute(Command command) async {
    var gameState = await repository.getGameState();
    String? objectName = command.object;

    if (objectName == null) {
      // Gérer le cas où aucun objet n'est spécifié
      print("Que voulez-vous prendre ?");
      return;
    }

    Item? item = gameState.getItemByName(objectName);

    if (item == null) {
      print("Vous ne voyez pas cela ici.");
      return;
    }

    if (gameState.player.inventory.contains(item)) {
      print("Vous portez déjà ${item.name}.");
      return;
    }

    if (!item.isCollectible) {
      print("Vous ne pouvez pas prendre ${item.name}.");
      return;
    }

    // Vérifier si l'objet est présent dans le lieu actuel
    var currentLocation = gameState.currentLocation;
    if (!currentLocation.items.contains(item.id)) {
      print("${item.name} n'est pas ici.");
      return;
    }

    // Vérifier la limite d'inventaire
    if (gameState.player.inventory.length >= gameState.player.inventoryLimit) {
      print("Vous ne pouvez pas porter plus d'objets.");
      return;
    }

    // Ajouter l'objet à l'inventaire du joueur
    gameState.player.inventory.add(item);

    // Retirer l'objet du lieu actuel
    currentLocation.items.remove(item.id);

    print("Vous prenez ${item.name}.");

    // Mettre à jour l'état du jeu
    repository.updateGameState(gameState);
  }
}
