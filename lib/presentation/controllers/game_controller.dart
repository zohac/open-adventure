import '../../domain/entities/command_parser.dart';
import '../../domain/usecases/attack_usecase.dart';
import '../../domain/usecases/take_item_usecase.dart';
import '../../domain/entities/command.dart';

class GameController {
  final AttackUseCase attackUseCase;
  final TakeItemUseCase takeItemUseCase;
  // Ajoutez d'autres cas d'utilisation si nécessaire

  GameController({
    required this.attackUseCase,
    required this.takeItemUseCase,
  });

  void handleUserInput(String input) {
    CommandParser parser = CommandParser();
    Command command = parser.parse(input);

    switch (command.verb) {
      case 'attaquer':
        attack(command);
        break;
      case 'prendre':
        takeItem(command);
        break;
    // Ajoutez d'autres actions ici
      default:
        print("Je ne comprends pas cette action.");
    }
  }

  void attack(Command command) {
    attackUseCase.execute(command);
  }

  void takeItem(Command command) {
    takeItemUseCase.execute(command);
  }

// Ajoutez d'autres méthodes d'action ici
}
