// lib/domain/usecases/pick_up_item_usecase.dart
import '../entities/player.dart';
import '../entities/item.dart';
import '../repositories/game_repository.dart';

class PickUpItemUseCase {
  final GameRepository repository;

  PickUpItemUseCase(this.repository);

  Future<void> call(Player player, Item item) async {
    if (item.isCollectible && player.currentLocation.items.contains(item)) {
      final updatedInventory = List<Item>.from(player.inventory)..add(item);
      final updatedPlayer = player.copyWith(inventory: updatedInventory);

      final updatedLocationItems = List<Item>.from(player.currentLocation.items)..remove(item);
      final updatedLocation = player.currentLocation.copyWith(items: updatedLocationItems);

      await repository.updatePlayer(updatedPlayer);
      await repository.updateLocation(updatedLocation);
    } else {
      throw Exception('Vous ne pouvez pas ramasser cet objet.');
    }
  }
}
