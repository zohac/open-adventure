// lib/domain/entities/player.dart
import 'package:equatable/equatable.dart';

import '../constants.dart';
import 'location.dart';
import 'item.dart';

class Player extends Equatable {
  final Location currentLocation;
  final Location? previousLocation;
  final List<Item> inventory;
  final int score;
  final int turns;
  final int health;
  final int inventoryLimit;

  const Player({
    required this.currentLocation,
    this.previousLocation,
    this.inventory = const [],
    this.score = 0,
    this.turns = 0,
    this.health = 100,
    this.inventoryLimit = GameConstants.inventoryLimit,
  });

  Player copyWith({
    Location? currentLocation,
    Location? previousLocation,
    List<Item>? inventory,
    int? score,
    int? turns,
    int? health,
  }) {
    return Player(
      currentLocation: currentLocation ?? this.currentLocation,
      previousLocation: previousLocation ?? this.previousLocation,
      inventory: inventory ?? this.inventory,
      score: score ?? this.score,
      turns: turns ?? this.turns,
      health: health ?? this.health,
    );
  }

  @override
  List<Object?> get props => [
    currentLocation,
    previousLocation,
    inventory,
    score,
    turns,
    health,
  ];
}
