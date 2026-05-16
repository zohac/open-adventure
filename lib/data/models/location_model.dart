import 'package:open_adventure/data/models/item_model.dart';
import 'package:open_adventure/domain/entities/location.dart';

class LocationModel extends Location {
  LocationModel({
    required int id,
    required String name,
    required String description,
    String? shortDescription,
    Map<String, int>? exits,
    Map<int, ItemModel>? items,
  }) : super(
    id: id,
    name: name,
    description: description,
    shortDescription: shortDescription,
    exits: exits ?? {},
    items: items ?? {},
  );

  factory LocationModel.fromYaml(Map<String, dynamic> yaml) {
    return LocationModel(
      id: yaml['id'] as int,
      name: yaml['name'] as String,
      description: yaml['description'] as String,
      shortDescription: yaml['shortDescription'] as String?,
      exits: (yaml['exits'] as Map?)?.map((key, value) => MapEntry(key as String, value as int)) ?? {},
      items: List<int>.from(yaml['items'] ?? []),
    );
  }
}
