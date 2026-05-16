import '../../domain/entities/item.dart';

class ItemModel extends Item {

  const ItemModel({
    required super.id,
    required super.name,
    required super.description,
    required super.initialLocation,
    bool? isCollectible,
    Map<String, dynamic>? properties
  });

  factory ItemModel.fromYaml(Map<String, dynamic> yaml) {
    return ItemModel(
      id: yaml['id'],
      name: yaml['name'],
      description: yaml['description'],
      isCollectible: yaml['isCollectible'] ?? true,
      properties: Map<String, dynamic>.from(yaml['properties'] ?? {}),
      initialLocation: yaml['initialLocation'],
    );
  }
}
