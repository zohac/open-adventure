import 'dart:math';
import '../../domain/entities/game.dart';
import '../../domain/services/lgc_random_number_generator.dart';
import '../constants/data_constants.dart';

class LcgRandomNumberGeneratorImpl implements LcgRandomNumberGenerator {
  int lcgA = DataConstants.lcgA;
  int lcgC = DataConstants.lcgC;
  int lcgM = DataConstants.lcgM;

  String zzWord = "";
  final Game game;

  LcgRandomNumberGeneratorImpl({
    required this.game
  });

  @override
  void setSeed(int seed) {
    game.lcgX = seed % lcgM;
    if (game.lcgX < 0) {
      game.lcgX = lcgM + game.lcgX;
    }

    // Génération du mot 'ZZZZ' aléatoire
    game.zzWord = List.generate(DataConstants.tokenLength, (index) {
      if (index == 1) {
        return '\'';
      }
      return String.fromCharCode(65 + randRange(26)); // 65 est 'A' en ASCII
    }).join();
  }

  @override
  int getNextLcgValue() {
    int oldX = game.lcgX;
    game.lcgX = (lcgA * game.lcgX + lcgC) % lcgM;
    return oldX;
  }

  @override
  int randRange(int range) {
    return (range * getNextLcgValue()) ~/ lcgM;
  }
}
