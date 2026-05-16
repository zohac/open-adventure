import 'package:flutter/material.dart';
import 'package:open_adventure/data/services/lcg_random_number_generator_impl.dart';
import 'package:open_adventure/data/services/random_number_generator_impl.dart';
import 'package:open_adventure/domain/usecases/game_initializer_usecase.dart';
import 'package:open_adventure/presentation/controllers/game_controller.dart';
import 'package:open_adventure/presentation/pages/game_page.dart';

import 'data/data_sources/game_data_source.dart';
import 'data/repositories/game_repository_impl.dart';
import 'domain/entities/dwarf.dart';
import 'domain/entities/game.dart';
import 'domain/entities/item.dart';
import 'domain/entities/location.dart';
import 'domain/entities/player.dart';
import 'domain/entities/settings.dart';
import 'domain/repositories/game_repository.dart';
import 'domain/services/lgc_random_number_generator.dart';
import 'domain/services/random_number_generator.dart';
import 'domain/usecases/attack_usecase.dart';
import 'domain/usecases/take_item_usecase.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // Charger les données
  GameDataSource dataSource = GameDataSource();
  Map<int, Item> items = await dataSource.loadItems();
  Map<int, Location> locations = await dataSource.loadLocations();

  Game game = Game();
  Settings settings = Settings(
    logfp: null,      // Correspond à `NULL` en C
    oldStyle: false,  // Correspond à `false` en C
    prompt: true,     // Correspond à `true` en C
  );

  // Créer le repository
  // GameRepository gameRepository = GameRepositoryImpl(game);

  // Créer l'état initial du jeu
  // Location startingLocation = locations[0]!; // Assurez-vous que l'ID 0 existe

  RandomNumberGenerator randomGenerator = RandomNumberGeneratorImpl();
  LcgRandomNumberGenerator lcgRandomGenerator = LcgRandomNumberGeneratorImpl(game: game);
  GameInitializerUseCase gameInitializer = GameInitializerUseCase(
      randomGenerator: randomGenerator,
      lcgRandomGenerator: lcgRandomGenerator,
      game: game,
      settings: settings
  );

  gameInitializer.initialize();

  // Game game = Game(
  //   player: Player(
  //     currentLocation: startingLocation,
  //     inventory: [],
  //     health: 100,
  //     score: 0,
  //     inventoryLimit: 7
  //   ),
  //   items: items,
  //   locations: locations,
  //   dwarves: dwarves,
  //   flags: {},
  // );



  // Créer les cas d'utilisation
  // AttackUseCase attackUseCase = AttackUseCase(repository: gameRepository);
  // TakeItemUseCase takeItemUseCase = TakeItemUseCase(repository: gameRepository);
  // Créez d'autres cas d'utilisation si nécessaire

  // Créer le contrôleur
  // GameController gameController = GameController(
  //   attackUseCase: attackUseCase,
  //   takeItemUseCase: takeItemUseCase,
  // );

  // runApp(MyApp(gameController: gameController));
}

class MyApp extends StatelessWidget {
  final GameController gameController;

  MyApp({required this.gameController});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Open Adventure',
      home: GamePage(gameController: gameController),
    );
  }
}
