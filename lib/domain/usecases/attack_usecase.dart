import '../entities/game.dart';
import '../repositories/game_repository.dart';
import '../entities/command.dart';

class AttackUseCase {
  final GameRepository repository;

  AttackUseCase({required this.repository});

  Future<void> execute(Command command) async {
    // Récupérer l'état du jeu
    Game game = await repository.getGameState();

    // Implémenter la logique d'attaque
    // ...

    // Mettre à jour l'état du jeu
    repository.updateGameState(game);
  }
}
