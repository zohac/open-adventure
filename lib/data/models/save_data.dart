import 'game_model.dart';

class SaveData {
  String magic;
  int version;
  int canary;
  GameModel game;

  SaveData({
    required this.magic,
    required this.version,
    required this.canary,
    required this.game,
  });
}
