// lib/domain/entities/dwarf.dart
import 'package:equatable/equatable.dart';

import 'location.dart';

class Dwarf extends Equatable {
  final int id;
  final String name;
  final Location location;
  final bool isAlive;
  final bool hasSeenPlayer;

  const Dwarf({
    required this.id,
    required this.location,
    this.name = '',
    this.isAlive = true,
    this.hasSeenPlayer = false,
  });

  Dwarf copyWith({
    Location? location,
    bool? isAlive,
    bool? hasSeenPlayer,
  }) {
    return Dwarf(
      id: id,
      name: name,
      location: location ?? this.location,
      isAlive: isAlive ?? this.isAlive,
      hasSeenPlayer: hasSeenPlayer ?? this.hasSeenPlayer,
    );
  }

  @override
  List<Object> get props => [id, location, isAlive, hasSeenPlayer];
}
