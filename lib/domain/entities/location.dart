import 'package:equatable/equatable.dart';
import 'item.dart';

class Location extends Equatable {
  final int id;
  final String name;
  final String description;
  final String? shortDescription;
  final Map<String, int>? exits; // Direction -> Location ID
  final Map<int, Item>? items;

  const Location({
    required this.id,
    required this.name,
    required this.description,
    this.shortDescription,
    this.exits,
    this.items,
  });

  /// Ajoutez cette méthode copyWith
  Location copyWith({
    int? id,
    String? name,
    String? description,
    String? shortDescription,
    Map<String, int>? exits,
    Map<int, Item>? items,
  }) {
    return Location(
      id: id ?? this.id,
      name: name ?? this.name,
      description: description ?? this.description,
      shortDescription: shortDescription ?? this.shortDescription,
      exits: exits ?? this.exits,
      items: this.items,
    );
  }

  @override
  List<Object?> get props => throw UnimplementedError();
}
