// lib/domain/entities/item.dart
import 'package:equatable/equatable.dart';

class Item extends Equatable {
  final int id;
  final String name;
  final String description;
  final bool isCollectible;
  final Map<String, dynamic> properties;
  final String initialLocation;

  const Item({
    required this.id,
    required this.name,
    required this.description,
    required this.initialLocation,
    this.isCollectible = true,
    this.properties = const {},
  });

  /// Ajoutez cette méthode copyWith
  Item copyWith({
    int? id,
    String? name,
    String? description,
    bool? isCollectible,
    Map<String, dynamic>? properties,
    String? initialLocation,
  }) {
    return Item(
      id: id ?? this.id,
      name: name ?? this.name,
      description: description ?? this.description,
      isCollectible: isCollectible ?? this.isCollectible,
      properties: properties ?? this.properties,
      initialLocation: initialLocation ?? this.initialLocation
    );
  }

  @override
  List<Object> get props => [id, name, description, isCollectible, properties];
}
