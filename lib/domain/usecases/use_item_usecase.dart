// lib/domain/usecases/use_item_usecase.dart
import '../entities/player.dart';
import '../entities/item.dart';
import '../entities/game.dart';
import '../repositories/game_repository.dart';

class UseItemUseCase {
  final GameRepository repository;

  UseItemUseCase(this.repository);

  Future<void> call(Player player, Item item) async {
    if (player.inventory.contains(item)) {
      // Obtenir l'état actuel du jeu
      final game = await repository.getGameState();

      // Récupérer la localisation actuelle
      final locationId = player.currentLocation.id;

      if (game.locations.containsKey(locationId)) {
        final currentLocation = game.locations[locationId]!;

        // Mettre à jour la localisation
        final updatedLocation = currentLocation.copyWith(
          // mises à jour spécifiques à l'utilisation de l'objet
        );

        // Mettre à jour l'objet si nécessaire
        final updatedItem = item.copyWith(
          // mises à jour de l'objet
        );

        // Mettre à jour le joueur si nécessaire
        final updatedPlayer = player.copyWith(
          // mises à jour du joueur
        );

        // Mettre à jour l'état du jeu
        final updatedGame = game.copyWith(
          locations: {
            ...game.locations,
            locationId: updatedLocation,
          },
          items: {
            ...game.items,
            item.id: updatedItem,
          },
          player: updatedPlayer,
          // Autres mises à jour si nécessaire
        );

        await repository.updateGameState(updatedGame);
      } else {
        throw Exception('La localisation actuelle est introuvable.');
      }
    } else {
      throw Exception('Vous ne possédez pas cet objet.');
    }
  }
}
