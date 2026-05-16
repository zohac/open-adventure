import 'package:open_adventure/domain/entities/dwarf.dart';
import 'package:open_adventure/domain/entities/settings.dart';
import 'package:open_adventure/domain/enums/locations_ref.dart';
import 'package:open_adventure/domain/entities/game.dart';
import 'package:open_adventure/domain/services/lgc_random_number_generator.dart';
import 'package:open_adventure/domain/services/random_number_generator.dart';

class GameInitializerUseCase {
  final RandomNumberGenerator randomGenerator;
  final LcgRandomNumberGenerator lcgRandomGenerator;
  final Game game;
  final Settings settings;

  GameInitializerUseCase({
    required this.randomGenerator,
    required this.lcgRandomGenerator,
    required this.game,
    required this.settings
  });

  void initialize() {
    //TODO : Affichage Optionnel : Affiche un message d'initialisation
    // si le mode ancien style est activé.

    //Génération de la Graine Aléatoire : Initialise le générateur
    // de nombres aléatoires avec une graine basée sur l'heure actuelle.
    int seedVal = DateTime.now().millisecondsSinceEpoch;
    int seed = randomGenerator.generate(seedVal);
    lcgRandomGenerator.setSeed(seed);

    //TODO : Génération des Nains
    const Dwarf dwarf1 = Dwarf(id: 1, location: LocationRefs.locKinghall);

    //TODO : Placement Initial des Objets : Positionne tous les objets
    // du jeu à un emplacement par défaut (LOC_NOWHERE).

    //TODO : Configuration des Conditions de Localisation : Met à jour
    // les conditions des localisations en fonction de leurs descriptions et des clés associées.

    //TODO : Placement Spécifique des Objets : Utilise des boucles pour
    // placer les objets dans leurs localisations respectives,
    // en tenant compte des objets fixés et non fixés.

    //TODO : Initialisation des Trésors : Identifie et marque les trésors
    // du jeu comme non trouvés, en mettant à jour le compteur de trésors.

    //TODO : Initialisation des Conditions du Jeu : Définit les conditions
    // initiales du jeu nécessaires pour son fonctionnement.

    //TODO : Retour de la Graine Aléatoire : Retourne la graine utilisée
    // pour les opérations aléatoires, ce qui peut être utile pour le débogage ou la reproduction des sessions de jeu.
  }
}
