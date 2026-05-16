// lib/domain/usecases/move_player_usecase.dart
import '../entities/player.dart';
import '../repositories/game_repository.dart';

class MovePlayerUseCase {
  final GameRepository repository;

  MovePlayerUseCase(this.repository);

  Future<void> call(Player player, String direction) async {
    final currentLocation = player.currentLocation;
    final exits = currentLocation.exits;

    if (exits.containsKey(direction)) {
      final nextLocationId = exits[direction]!;
      final nextLocation = await repository.getLocationById(nextLocationId);

      final updatedPlayer = player.copyWith(
        previousLocation: player.currentLocation,
        currentLocation: nextLocation,
        turns: player.turns + 1,
      );

      await repository.updatePlayer(updatedPlayer);
    } else {
      throw Exception('Vous ne pouvez pas aller dans cette direction.');
    }
  }
}
